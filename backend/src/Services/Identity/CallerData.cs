namespace Dispenser.Services.Identity;

public record CallerData
{
    public required string Username { get; init; }
    public required int Id { get; init; }
    public required Guid DeviceId { get; init; }
}