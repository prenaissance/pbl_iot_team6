namespace Dispenser.Validators.Pills;

using Dispenser.Dtos.PillSchedules;
using FluentValidation;

public class AddPillScheduleRequestValidator : AbstractValidator<AddPillScheduleRequest>
{
    public AddPillScheduleRequestValidator()
    {
        RuleFor(x => x.Times)
            .NotEmpty();
    }
}