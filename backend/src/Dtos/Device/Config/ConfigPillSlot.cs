using Dispenser.Models.PillSchedules;

namespace Dispenser.Dtos.Device.Config;

public record ConfigPillSlot
{
    public required int SlotNumber { get; init; }
    public required string PillName { get; init; }
    public required uint PillCount { get; init; }
    public static ConfigPillSlot FromPillSlot(PillSlot pillSlot) => new()
    {
        SlotNumber = pillSlot.SlotNumber,
        PillName = pillSlot.PillName,
        PillCount = pillSlot.PillCount
    };
}