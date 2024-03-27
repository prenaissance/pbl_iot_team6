namespace Dispenser.Api.Routes;

using Dispenser.Api.Filters;
using Dispenser.DataAccess;
using Dispenser.Dtos.PillSchedules;
using Dispenser.Models.PillSchedules;
using Dispenser.Services.Authentication;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.EntityFrameworkCore;

public static class PillRoutes
{
    public static IEndpointRouteBuilder RegisterPillRoutes(this IEndpointRouteBuilder routes)
    {
        var group = routes.MapGroup("/pills");
        group.MapGet("/slots", GetPillSlots);
        group.MapPut("/slots/{id}", EditPillSlot).WithValidation<EditPillSlotRequest>();
        group.MapPost("/slots/{id}/schedules", AddPillSchedules).WithValidation<AddPillSchedulesRequest>();

        return routes;
    }

    private static async Task<Ok<PillSlotResponse[]>> GetPillSlots(
        Db db,
        ICallerService callerService
    )
    {
        var callerData = callerService.GetCallerData();
        var pillSlots = await db.PillSlots
            .Where(ps => ps.OwnerId == callerData.Id)
            .Include(ps => ps.PillSchedules)
            .ToArrayAsync();

        var pillSlotsResponses = pillSlots
            .Select(ps => ps.ToPillSlotResponse())
            .ToArray();

        return TypedResults.Ok(pillSlotsResponses);
    }

    private static async Task<Results<
        Ok<PillSlotResponse>, NotFound>
    > EditPillSlot(
        Db db,
        ICallerService callerService,
        Guid id,
        EditPillSlotRequest request
    )
    {
        var callerData = callerService.GetCallerData();
        var pillSlot = await db.PillSlots
            .Where(ps => ps.OwnerId == callerData.Id)
            .FirstOrDefaultAsync(ps => ps.Id == id);

        if (pillSlot is null)
        {
            return TypedResults.NotFound();
        }

        pillSlot.PillCount = request.PillCount;
        pillSlot.PillName = request.PillName;

        await db.SaveChangesAsync();

        return TypedResults.Ok(pillSlot.ToPillSlotResponse());
    }

    private static async Task<Results<
      Ok<PillSlotResponse>, NotFound>
    > AddPillSchedules(
        Db db,
        ICallerService callerService,
        Guid id,
        AddPillSchedulesRequest request
    )
    {
        var callerData = callerService.GetCallerData();
        var pillSlot = await db.PillSlots
            .Where(ps => ps.OwnerId == callerData.Id)
            .FirstOrDefaultAsync(ps => ps.Id == id);

        if (pillSlot is null)
        {
            return TypedResults.NotFound();
        }

        var pillSchedules = request.PillSchedules
            .Select(ps => new PillSchedule
            {
                PillSlotId = pillSlot.Id,
                ProfileId = ps.ProfileId,
                Times = ps.Times.Select(t => t.ToTimeOnly()).ToList()
            })
            .ToArray();

        pillSlot.PillSchedules.AddRange(pillSchedules);

        await db.SaveChangesAsync();

        return TypedResults.Ok(pillSlot.ToPillSlotResponse());
    }
}