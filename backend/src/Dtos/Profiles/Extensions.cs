using Dispenser.Dtos.PillSchedules;
using Dispenser.Models.Profiles;

namespace Dispenser.Dtos.Profiles;

public static class Extensions
{
    public static ProfileResponse ToProfileResponse(this Profile profile)
    {
        return new ProfileResponse
        {
            Id = profile.Id,
            Username = profile.Username
        };
    }

    public static ProfileWithSchedulesResponse ToProfileWithSchedulesResponse(this Profile profile)
    {
        return new ProfileWithSchedulesResponse
        {
            Id = profile.Id,
            Username = profile.Username,
            PillSchedules = profile.PillSchedules
                .Select(ps => new ProfilePillScheduleResponse
                {
                    Id = ps.Id,
                    Times = ps.Times
                        .Select(TimeOfDay.FromTimeOnly)
                        .ToArray(),
                    Quantity = ps.Quantity,
                    PillSlot = new ProfilePillSlotResponse
                    {
                        Id = ps.PillSlot.Id,
                        SlotNumber = ps.PillSlot.SlotNumber,
                        PillName = ps.PillSlot.PillName,
                        PillCount = ps.PillSlot.PillCount
                    }
                })
                .ToArray(),
            RfidTag = profile.RfidTag
        };
    }
}