namespace Dispenser.Dtos.PillSchedules;

public record PillScheduleResponse
{
    public required Guid Id { get; init; }
    public required List<TimeOnly> Times { get; init; }
}