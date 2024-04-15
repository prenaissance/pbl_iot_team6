using Dispenser.Dtos.PillSchedules;

namespace Dispenser.Dtos.Device.Config;

public record ConfigPillSchedule
{
    public required TimeOfDay Time { get; set; }
    public required int SlotNumber { get; set; }
}