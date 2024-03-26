namespace Dispenser.Models.Profiles;

public class Owner
{
    public int Id { get; set; }
    public Guid DeviceId { get; set; }
    public required string Username { get; set; }
    public required string PasswordHash { get; set; }
    public required string PasswordSalt { get; set; }
    public List<Profile> CreatedProfiles { get; set; } = [];
}