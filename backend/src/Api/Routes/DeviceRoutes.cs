using Dispenser.Api.Filters;
using Dispenser.DataAccess;
using Dispenser.Dtos.Device.Config;
using Dispenser.Dtos.Device.Events;
using Dispenser.Dtos.PillSchedules;
using Dispenser.Models.Audit;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using Microsoft.OpenApi.Any;
using Microsoft.OpenApi.Models;

namespace Dispenser.Api.Routes;

public static class DeviceRoutes
{
    private static readonly Func<OpenApiOperation, OpenApiOperation> eventOpenapi =
        config =>
        {
            var existingDeviceId = config.Parameters.FirstOrDefault(p => p.Name == "deviceId");
            if (existingDeviceId is not null)
            {
                config.Parameters.Remove(existingDeviceId);
            }
            config.Parameters.Add(
                new OpenApiParameter
                {
                    Name = "deviceId",
                    In = ParameterLocation.Path,
                    Required = true,
                    Schema = new OpenApiSchema
                    {
                        Type = "string",
                        Format = "uuid"
                    },
                    Example = new OpenApiString("11111111-1111-1111-1111-111111111111")
                }
            );
            return config;
        };
    public static IEndpointRouteBuilder RegisterDeviceRoutes(this IEndpointRouteBuilder routes)
    {
        var group = routes.MapGroup("/devices");

        group.MapGet("/{deviceId:guid}/config", GetConfig)
            .WithDescription("Get the device configuration. Rfid encoded in base64.")
            .WithOpenApi(
                config =>
                {
                    config.Parameters = [
                        new OpenApiParameter
                        {
                            Name = "deviceId",
                            In = ParameterLocation.Path,
                            Required = true,
                            Schema = new OpenApiSchema
                            {
                                Type = "string",
                                Format = "uuid"

                            },
                            Example = new OpenApiString("11111111-1111-1111-1111-111111111111")
                        }
                    ];
                    return config;
                }
            );

        group.MapPost("/{deviceId:guid}/event", AddEvent)
            .WithValidation<AddEventRequest>()
            .WithDescription("Add an event to the device audit log")
            .WithOpenApi(eventOpenapi);

        group.MapGet("/{deviceId:guid}/event", AddEventWithGet)
            .WithDescription("Add an event to the device audit log with GET (workaround)")
            .WithOpenApi(eventOpenapi);

        return routes;
    }

    private static async Task<Results<Ok<ConfigResponse>, NotFound<string>>> GetConfig(Guid deviceId, Db db)
    {
        var owner = await db.Owners
            .Where(o => o.DeviceId == deviceId)
            .Include(o => o.CreatedProfiles)
            .ThenInclude(p => p.PillSchedules)
            .ThenInclude(ps => ps.PillSlot)
            .FirstOrDefaultAsync();

        if (owner is null)
        {
            return TypedResults.NotFound("Device not found");
        }

        var config = new ConfigResponse
        {
            Profiles = owner.CreatedProfiles.Select(p => new ConfigProfile
            {
                ProfileId = p.Id,
                Username = p.Username,
                Rfid = p.RfidTag?.Rfid,
                PillSchedules = p.PillSchedules.SelectMany(
                    ps => ps.Times.Select(
                        t => new ConfigPillSchedule
                        {
                            SlotNumber = ps.PillSlot.SlotNumber,
                            Time = TimeOfDay.FromTimeOnly(t),
                            Quantity = ps.Quantity
                        }
                    )).ToArray()
            }).ToArray(),
            PillSlots = db.PillSchedules
                .Where(ps => ps.PillSlot.OwnerId == owner.Id)
                .OrderBy(ps => ps.PillSlot.SlotNumber)
                .AsEnumerable()
                .DistinctBy(ps => ps.PillSlot.SlotNumber)
                .Select(ps => ConfigPillSlot.FromPillSlot(ps.PillSlot))
                .ToArray()
        };

        return TypedResults.Ok(config);
    }

    private static async Task<Created> AddEvent(
        [AsParameters] AddEventRequest request,
        Db db)
    {
        await db.ProcessedEvents.AddAsync(new ProcessedEvent
        {
            DeviceId = request.DeviceId,
            EventType = request.Body.EventType,
            EventData = request.Body.EventData,
            ProfileId = request.Body.ProfileId
        });

        await db.SaveChangesAsync();

        return TypedResults.Created();
    }

    private static async Task<Created> AddEventWithGet(
        [FromRoute] Guid deviceId,
        string eventType,
        string eventData,
        int? profileId,
        Db db
    )
    {
        await db.ProcessedEvents.AddAsync(new ProcessedEvent
        {
            DeviceId = deviceId,
            EventType = eventType,
            EventData = eventData,
            ProfileId = profileId
        });

        await db.SaveChangesAsync();

        return TypedResults.Created();
    }
}