using Dispenser.Models.Profiles;

namespace Dispenser.Dtos.Profiles;

public record ProfileWithSchedulesResponse
{
    public int Id { get; init; }
    public required string Username { get; init; }
    public required IList<ProfilePillScheduleResponse> PillSchedules { get; init; }
    public RfidTag? RfidTag { get; init; }
}