namespace Dispenser.Dtos.PillSchedules;

public record PillSlotResponse
{
    public required Guid Id { get; init; }
    public required int SlotNumber { get; init; }
    public required string PillName { get; init; }
    public required uint PillCount { get; init; }
    public required IList<PillScheduleResponse> PillSchedules { get; init; }
}