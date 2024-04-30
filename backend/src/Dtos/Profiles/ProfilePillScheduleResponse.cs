using Dispenser.Dtos.PillSchedules;

namespace Dispenser.Dtos.Profiles;

public record ProfilePillScheduleResponse
{

    public required Guid Id { get; init; }
    public required IList<TimeOfDay> Times { get; init; }
    public required int Quantity { get; init; }
}