using Dispenser.DataAccess;
using Dispenser.Dtos.Device.Config;
using Dispenser.Dtos.PillSchedules;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.EntityFrameworkCore;
using Microsoft.OpenApi.Any;
using Microsoft.OpenApi.Models;

namespace Dispenser.Api.Routes;

public static class DeviceRoutes
{
    public static IEndpointRouteBuilder RegisterDeviceRoutes(this IEndpointRouteBuilder routes)
    {
        var group = routes.MapGroup("/devices");

        group.MapGet("/{deviceId}/config", GetConfig)
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

        return routes;
    }

    public static async Task<Results<Ok<ConfigResponse>, NotFound<string>>> GetConfig(Guid deviceId, Db db)
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
                .DistinctBy(ps => ps.PillSlot.SlotNumber)
                .OrderBy(ps => ps.PillSlot.SlotNumber)
                .AsEnumerable()
                .Select(ps => ConfigPillSlot.FromPillSlot(ps.PillSlot))
                .ToArray()
        };

        return TypedResults.Ok(config);
    }
}