using Dispenser.Dtos.Profiles;
using FluentValidation;

namespace Dispenser.Validators.Profiles;

public class SetRfidValidator : AbstractValidator<SetRfidRequest>
{
    public SetRfidValidator()
    {
        RuleFor(x => x.ProfileId).NotEmpty();
        RuleFor(x => x.Rfid)
            .Must(x => x.Length == 20)
            .WithMessage(r => $"Rfid must be 20 bytes long. Got {r.Rfid.Length} bytes.");
    }
}