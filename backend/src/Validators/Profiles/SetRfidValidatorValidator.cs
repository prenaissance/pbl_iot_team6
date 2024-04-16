using Dispenser.Dtos.Profiles;
using FluentValidation;

namespace Dispenser.Validators.Profiles;

public class SetRfidValidator : AbstractValidator<SetRfidRequest>
{
    public SetRfidValidator()
    {
        RuleFor(x => x.ProfileId).NotEmpty();
        RuleFor(x => x.Rfid)
            .NotEmpty()
            .Must(x => x.Length <= 10)
            .WithMessage(r => $"Rfid must be at most 10 bytes long. Got {r.Rfid.Length} bytes.");
    }
}