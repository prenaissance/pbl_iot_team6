namespace Dispenser.Services.Authentication;

using Dispenser.Services.Identity;

public interface ICallerService
{
    CallerData GetCallerData();
}