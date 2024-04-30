namespace Dispenser.Validators.Pills;

using Dispenser.DataAccess;
using Dispenser.Dtos.PillSchedules;
using Dispenser.Services.Identity;
using FluentValidation;

public class AddPillSchedulesRequestValidator : AbstractValidator<AddPillSchedulesRequest>
{
    public AddPillSchedulesRequestValidator(ICallerService callerService, Db db)
    {
        RuleForEach(x => x.PillSchedules)
            .SetValidator(new AddPillScheduleRequestValidator(
                callerService,
                db
            ));
    }
}