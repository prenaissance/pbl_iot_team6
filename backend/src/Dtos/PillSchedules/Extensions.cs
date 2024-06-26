using Dispenser.Dtos.PillSchedules;
using Dispenser.Dtos.Profiles;
using Dispenser.Models.PillSchedules;

namespace Dispenser.Dtos.PillSchedules;

public static class Extensions
{
    public static PillScheduleResponse ToPillScheduleResponse(this PillSchedule pillSchedule) =>
        new()
        {
            Id = pillSchedule.Id,
            Times = pillSchedule.Times.Select(TimeOfDay.FromTimeOnly).ToArray(),
            Quantity = pillSchedule.Quantity,
            Profile = pillSchedule.Profile.ToProfileResponse()
        };

    public static PillSlotResponse ToPillSlotResponse(this PillSlot pillSlot) =>
        new()
        {
            Id = pillSlot.Id,
            SlotNumber = pillSlot.SlotNumber,
            PillCount = pillSlot.PillCount,
            PillName = pillSlot.PillName,
            PillSchedules = pillSlot.PillSchedules
                .Select(ToPillScheduleResponse)
                .ToArray()
        };
}