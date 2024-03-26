using Dispenser.Models.Profiles;

namespace Dispenser.Services.Authentication;

public interface IJwtService
{
    string GenerateJwtToken(Owner owner);
}