namespace Dispenser.Validators.Pills;

using Dispenser.DataAccess;
using Dispenser.Dtos.PillSchedules;
using Dispenser.Services.Identity;
using FluentValidation;
using Microsoft.EntityFrameworkCore;

public class AddPillScheduleRequestValidator : AbstractValidator<AddPillScheduleRequest>
{
    public AddPillScheduleRequestValidator(ICallerService callerService, Db db)
    {
        var callerData = callerService.GetCallerData();
        RuleFor(x => x.Times)
            .NotEmpty();
        RuleFor(x => x.Quantity)
            .GreaterThan(0);
        RuleFor(x => x.ProfileId)
            .MustAsync(async (profileId, cancellationToken) =>
            {
                var profile = await db.Profiles
                    .Where(p => p.Id == profileId && p.CreatedById == callerData.Id)
                    .FirstOrDefaultAsync(cancellationToken);

                return profile is not null;
            })
            .WithMessage(
                "Profile does not exist or does not belong to the current user"
            );
    }
}