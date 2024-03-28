using Dispenser.Dtos.PillSchedules;
using FluentValidation;

namespace Dispenser.Validators.Pills;

public class EditPillSlotRequestValidator : AbstractValidator<EditPillSlotRequest>
{
    public EditPillSlotRequestValidator()
    {
        RuleFor(x => x.PillSlotId)
            .NotEmpty();

        RuleFor(x => x.PillName)
            .NotEmpty();

        RuleFor(x => (int)x.PillCount)
            .GreaterThanOrEqualTo(0);
    }
}