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
                    Quantity = ps.Quantity
                })
                .ToArray(),
            RfidTag = profile.RfidTag
        };
    }
}