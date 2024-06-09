namespace Dispenser.Dtos.Device.Config;

public record ConfigProfile
{
    public required int ProfileId { get; set; }
    public required string Username { get; set; }
    public byte[]? Rfid { get; set; }
    public required IList<ConfigPillSchedule> PillSchedules { get; set; }
}