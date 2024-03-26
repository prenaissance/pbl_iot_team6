namespace Dispenser.Dtos.Authentication;

public record LoginDto(
    string Username,
    string Password
);