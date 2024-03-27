namespace Dispenser.Dtos.PillSchedules;

public record PillSlotResponse(
    Guid PillSlotId,
    int SlotNumber,
    string PillName,
    uint PillCount,
    List<PillScheduleResponse> PillSchedules
);