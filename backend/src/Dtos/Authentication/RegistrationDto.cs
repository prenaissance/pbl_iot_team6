namespace Dispenser.Dtos.Authentication;

public record RegistrationDto(
    string Username,
    string Password,
    Guid DeviceId
);
