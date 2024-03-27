namespace Dispenser.Dtos.PillSchedules;

public record EditPillSlotRequest(
    Guid PillSlotId,
    string PillName,
    uint PillCount
);