using Dispenser.Dtos.Profiles;

namespace Dispenser.Dtos.Device.Events;

public record ProcessedEventResponse
{
    public required Guid Id { get; set; }
    public required string EventType { get; set; }
    public required string EventData { get; set; }
    public required DateTime CreatedAt { get; set; }
    public ProfileResponse? Profile { get; set; }
}