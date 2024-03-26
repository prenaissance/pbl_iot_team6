using Dispenser.DataAccess;
using Dispenser.Dtos.Authentication;
using FluentValidation;
using Microsoft.EntityFrameworkCore;

namespace Dispenser.Validators.Authentication;

public class RegistrationDtoValidator : AbstractValidator<RegistrationDto>
{
    public RegistrationDtoValidator(Db db)
    {
        RuleFor(x => x.Username)
            .NotEmpty()
            .MustAsync(async (username, cancellationToken) =>
            {
                return await db.Owners
                    .AllAsync(o => o.Username != username, cancellationToken: cancellationToken);
            })
            .WithMessage("Username is already taken");

        RuleFor(x => x.Password)
            .NotEmpty()
            .MinimumLength(8);

        RuleFor(x => x.DeviceId)
            .NotEmpty();
    }
}
