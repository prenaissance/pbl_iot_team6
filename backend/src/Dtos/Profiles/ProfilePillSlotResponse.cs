namespace Dispenser.Dtos.Profiles;

public record ProfilePillSlotResponse
{
    public required Guid Id { get; init; }
    public required int SlotNumber { get; init; }
    public required string PillName { get; init; }
    public required uint PillCount { get; init; }
}