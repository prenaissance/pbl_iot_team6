using System.Security.Cryptography;
using System.Text;

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
}