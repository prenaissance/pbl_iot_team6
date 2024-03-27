using Dispenser.Models.Profiles;

namespace Dispenser.Models.PillSchedules;

public class PillSlot
{
    public Guid Id { get; set; } = Guid.NewGuid();
    public required int SlotNumber { get; init; }
    public required string PillName { get; set; }
    public uint PillCount { get; set; }
    public List<PillSchedule> PillSchedules { get; set; } = [];
    public int OwnerId { get; set; }
    public Owner Owner { get; set; } = null!;
}