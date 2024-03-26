namespace Dispenser.Models.PillSchedules;

public class PillSlot
{
    public Guid Id { get; set; }
    public required string PillName { get; set; }
    public uint PillCount { get; set; }
    public required List<PillSchedule> PillSchedules { get; set; }
}