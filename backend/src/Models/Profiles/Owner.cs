using System.Security.Cryptography;
using System.Text;
using Dispenser.Models.PillSchedules;

namespace Dispenser.Models.Profiles;

public class Owner
{
    public int Id { get; set; }
    public Guid DeviceId { get; set; }
    public required string Username { get; set; }
    public byte[] PasswordHash { get; set; } = null!;
    public byte[] PasswordSalt { get; set; } = null!;
    public List<Profile> CreatedProfiles { get; set; } = [];
    public bool DoesPasswordMatch(string password)
    {
        using var hmac = new HMACSHA512(PasswordSalt);
        var computedHash = hmac.ComputeHash(Encoding.UTF8.GetBytes(password));
        return computedHash.SequenceEqual(PasswordHash);
    }

    public void SetPassword(string password)
    {
        using var hmac = new HMACSHA512();
        PasswordSalt = hmac.Key;
        PasswordHash = hmac.ComputeHash(Encoding.UTF8.GetBytes(password));
    }
    public PillSlot[] PillSlots { get; } = [
        new PillSlot { SlotNumber = 1, PillName = "Pill 1" },
        new PillSlot { SlotNumber = 2, PillName = "Pill 2" }
    ];
}