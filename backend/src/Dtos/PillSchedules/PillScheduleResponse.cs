namespace Dispenser.Dtos.PillSchedules;

public record PillScheduleResponse
{
    public required Guid Id { get; init; }
    public required IList<TimeOfDay> Times { get; init; }
}