using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Text;
using Dispenser.Models.Profiles;
using Microsoft.IdentityModel.Tokens;

namespace Dispenser.Services.Authentication;

public class JwtService(IConfiguration config) : IJwtService
{
    public string GenerateJwtToken(Owner owner)
    {
        var tokenHandler = new JwtSecurityTokenHandler();
        var secret = config["JwtSecret"] ?? "supersecretkey";
        var key = Encoding.ASCII.GetBytes(secret.PadRight(64, ' '));
        var tokenDescriptor = new SecurityTokenDescriptor
        {
            Subject = new ClaimsIdentity([
                new Claim(ClaimTypes.Name, owner.Username),
                new Claim(ClaimTypes.NameIdentifier, owner.Id.ToString()),
                new Claim("deviceId", owner.DeviceId.ToString())
            ]),
            Expires = DateTime.UtcNow.AddDays(1),
            SigningCredentials = new SigningCredentials(new SymmetricSecurityKey(key), SecurityAlgorithms.HmacSha256Signature)
        };

        var token = tokenHandler.CreateToken(tokenDescriptor);
        return tokenHandler.WriteToken(token);
    }
}
