namespace Dispenser.Dtos.Profiles;

public record ProfileResponse
{
    public required int Id { get; init; }
    public required string Username { get; init; }
}