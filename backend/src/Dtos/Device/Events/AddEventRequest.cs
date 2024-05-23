using Microsoft.AspNetCore.Mvc;

namespace Dispenser.Dtos.Device.Events;

public record AddEventRequest
{
    public record RequestBody
    {
        public required string EventType { get; set; }
        public required string EventData { get; set; }
        public int? ProfileId { get; set; }
    }
    [FromRoute] public required Guid DeviceId { get; set; }
    [FromBody] public required RequestBody Body { get; set; }
}