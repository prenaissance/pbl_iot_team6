using Dispenser.Dtos.Profiles;

namespace Dispenser.Dtos.PillSchedules;

public record PillScheduleResponse
{
    public required Guid Id { get; init; }
    public required IList<TimeOfDay> Times { get; init; }
    public required int Quantity { get; init; }
    public required ProfileResponse Profile { get; init; }
}