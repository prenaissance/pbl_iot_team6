namespace Dispenser.Dtos.Device.Config;

public record ConfigResponse
{
    public required IList<ConfigProfile> Profiles { get; init; }
}