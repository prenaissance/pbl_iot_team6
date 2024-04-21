namespace Dispenser.Validators.Pills;

using Dispenser.Dtos.PillSchedules;
using FluentValidation;

public class AddPillScheduleRequestValidator : AbstractValidator<AddPillScheduleRequest>
{
    public AddPillScheduleRequestValidator()
    {
        RuleFor(x => x.Times)
            .NotEmpty();
        RuleFor(x => x.Quantity)
            .GreaterThan(0);
    }
}