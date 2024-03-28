using Dispenser.Models.Profiles;

namespace Dispenser.Models.Audit;

public class ProcessedEvent
{
    public Guid Id { get; set; }
    public required string EventType { get; set; }
    public required string EventData { get; set; }
    public DateTime CreatedAt { get; } = DateTime.UtcNow;
    public Guid? DeviceId { get; set; }
    public int? ProfileId { get; set; }
    public Profile Profile { get; set; } = null!;
}