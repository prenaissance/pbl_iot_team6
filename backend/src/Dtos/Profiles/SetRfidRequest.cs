namespace Dispenser.Dtos.Profiles;

public record SetRfidRequest(
    int ProfileId,
    byte[] Rfid
);