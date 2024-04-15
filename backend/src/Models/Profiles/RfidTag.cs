namespace Dispenser.Models.Profiles;

public record RfidTag
{
    public byte[] Rfid { get; set; } = new byte[20];
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
}
