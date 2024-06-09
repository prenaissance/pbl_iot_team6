using Dispenser.Api.Extensions;
using Dispenser.DataAccess;
using Dispenser.Dtos.Device.Events;
using Dispenser.Dtos.Profiles;
using Dispenser.Services.Identity;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.EntityFrameworkCore;

namespace Dispenser.Api.Routes;

public static class ProcessedEventsRoutes
{
    public static IEndpointRouteBuilder RegisterProcessedEventsRoutes(this IEndpointRouteBuilder routes)
    {
        var group = routes.MapGroup("/events");

        group.MapGet("", GetRecentEvents)
            .WithAuthorization()
            .WithDescription("Get the most recent events for the device");

        return routes;
    }

    private static async Task<Ok<ProcessedEventResponse[]>> GetRecentEvents(
        Db db,
        ICallerService callerService)
    {
        var callerData = callerService.GetCallerData();
        var twoDaysAgo = DateTime.UtcNow.AddDays(-2);
        var events = await db.ProcessedEvents.AsNoTracking()
            .Where(e => e.DeviceId == callerData.DeviceId)
            .Where(e => e.CreatedAt > twoDaysAgo)
            .OrderByDescending(e => e.CreatedAt)
            .Include(e => e.Profile)
            .ToArrayAsync();

        var eventsResponse = events
            .Select(e => new ProcessedEventResponse
            {
                Id = e.Id,
                EventType = e.EventType,
                EventData = e.EventData,
                CreatedAt = e.CreatedAt,
                Profile = e.Profile?.ToProfileResponse()
            }).ToArray();

        return TypedResults.Ok(eventsResponse);
    }
}