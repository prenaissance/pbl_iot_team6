namespace Dispenser.Api.Routes;

using Dispenser.Api.Extensions;
using Dispenser.Api.Filters;
using Dispenser.DataAccess;
using Dispenser.Dtos.PillSchedules;
using Dispenser.Models.PillSchedules;
using Dispenser.Services.Identity;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.EntityFrameworkCore;

public static class PillRoutes
{
    public static IEndpointRouteBuilder RegisterPillRoutes(this IEndpointRouteBuilder routes)
    {
        var group = routes.MapGroup("/pills");
        group.MapGet("/slots", GetPillSlots)
            .WithAuthorization()
            .WithDescription("Get all pill slots with schedules");
        group.MapPut("/slots", EditPillSlot)
            .WithAuthorization()
            .WithValidation<EditPillSlotRequest>()
            .WithDescription("Edit a pill slot, returns the pill slot without schedules");
        group.MapPost("/slots/{id:guid}/schedules", AddPillSchedules)
            .WithAuthorization()
            .WithValidation<AddPillSchedulesRequest>();
        group.MapDelete("/slots/schedules", DeletePillSchedules)
            .WithAuthorization()
            .WithDescription("Delete pill schedules by ids");
        group.MapPut("/slots/schedules", EditPillSchedule)
            .WithAuthorization()
            .WithValidation<EditPillScheduleRequest>()
            .WithDescription("Edit pill schedule times");

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
            .ThenInclude(ps => ps.Profile)
            .ToArrayAsync();

        var pillSlotsResponses = pillSlots
            .Select(ps => ps.ToPillSlotResponse())
            .ToArray();

        return TypedResults.Ok(pillSlotsResponses);
    }

    private static async Task<Results<
        Ok<PillSlotResponse>, NotFound>
    > EditPillSlot(
        EditPillSlotRequest request,
        Db db,
        ICallerService callerService
    )
    {
        var callerData = callerService.GetCallerData();
        var pillSlot = await db.PillSlots
            .Where(ps => ps.OwnerId == callerData.Id)
            .FirstOrDefaultAsync(ps => ps.Id == request.PillSlotId);

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
        AddPillSchedulesRequest request,
        Guid id,
        Db db,
        ICallerService callerService
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
                Times = ps.Times.Select(t => t.ToTimeOnly()).ToList(),
                Quantity = ps.Quantity
            })
            .ToArray();

        pillSlot.PillSchedules.AddRange(pillSchedules);

        await db.SaveChangesAsync();

        return TypedResults.Ok(pillSlot.ToPillSlotResponse());
    }

    private static async Task<Results<
        NoContent, NotFound<string>>
    > DeletePillSchedules(
        Guid[] ids,
        Db db,
        ICallerService callerService
    )
    {
        var callerData = callerService.GetCallerData();
        var pillSchedules = await db.PillSchedules
            .Where(ps => ps.PillSlot.OwnerId == callerData.Id)
            .Where(ps => ids.Contains(ps.Id))
            .ToArrayAsync();

        if (pillSchedules.Length != ids.Length)
        {
            var notFoundIds = ids
                .Except(pillSchedules.Select(ps => ps.Id))
                .Select(id => id.ToString())
                .ToArray();
            return TypedResults.NotFound($"Pill schedules with ids {string.Join(", ", notFoundIds)} not found");
        }

        db.PillSchedules.RemoveRange(pillSchedules);

        await db.SaveChangesAsync();

        return TypedResults.NoContent();
    }

    private static Results<Ok<PillScheduleResponse>, NotFound> EditPillSchedule(
        EditPillScheduleRequest request,
        Db db,
        ICallerService callerService
    )
    {
        var callerData = callerService.GetCallerData();
        var pillSchedule = db.PillSchedules
            .Include(ps => ps.PillSlot)
            .Where(ps => ps.PillSlot.OwnerId == callerData.Id)
            .FirstOrDefault(ps => ps.Id == request.Id);

        if (pillSchedule is null)
        {
            return TypedResults.NotFound();
        }

        pillSchedule.Times = request.Times.Select(t => t.ToTimeOnly()).ToList();
        pillSchedule.Quantity = request.Quantity;

        db.SaveChanges();

        return TypedResults.Ok(pillSchedule.ToPillScheduleResponse());
    }
}