using Dispenser.Models.Profiles;

namespace Dispenser.Models.PillSchedules;

public class PillSchedule
{
    public Guid Id { get; set; }
    public List<TimeOnly> Times { get; set; } = [];
    public Guid PillSlotId { get; set; }
    public PillSlot PillSlot { get; set; } = null!;
    public int ProfileId { get; set; }
    public Profile Profile { get; set; } = null!;
}