namespace Dispenser.Validators.Pills;

using Dispenser.Dtos.PillSchedules;
using FluentValidation;

public class AddPillSchedulesRequestValidator : AbstractValidator<AddPillSchedulesRequest>
{
    public AddPillSchedulesRequestValidator()
    {
        RuleForEach(x => x.PillSchedules)
            .SetValidator(new AddPillScheduleRequestValidator());
    }
}