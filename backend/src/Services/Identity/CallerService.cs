using System.Security.Claims;
using Dispenser.Services.Authentication;

namespace Dispenser.Services.Identity;

public class CallerService(IHttpContextAccessor httpContextAccessor) : ICallerService
{
    public CallerData GetCallerData()
    {
        var user = httpContextAccessor.HttpContext?.User;
        if (user is null)
        {
            throw new ArgumentNullException(nameof(user));
        }
        var username = user.FindFirst(ClaimTypes.Name)!.Value;
        var id = int.Parse(user.FindFirst(ClaimTypes.NameIdentifier)!.Value);
        var deviceId = Guid.Parse(user.FindFirst("deviceId")!.Value);

        return new CallerData
        {
            Username = username,
            Id = id,
            DeviceId = deviceId
        };
    }
}