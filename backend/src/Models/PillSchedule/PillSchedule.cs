namespace Dispenser.Models.PillSchedule;

public class PillSchedule
{
    public Guid Id { get; set; }
    public List<TimeOnly> Times { get; set; } = [];
    public Guid PillSlotId { get; set; }
    public required PillSlot PillSlot { get; set; }
    public Guid ProfileId { get; set; }
}