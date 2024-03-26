namespace Dispenser.Models.Profiles;

public class RfidTag
{
    public int Id { get; set; }
    public string Tag { get; set; } = null!;
    public string? Description { get; set; }
    public int ProfileId { get; set; }
    public Profile Profile { get; set; } = null!;
}