namespace Dispenser.Validators.Device;

using Dispenser.DataAccess;
using Dispenser.Dtos.Device.Events;
using FluentValidation;
using Microsoft.EntityFrameworkCore;

public class AddEventRequestValidator : AbstractValidator<AddEventRequest>
{
    public AddEventRequestValidator(Db db)
    {
        RuleFor(x => x.DeviceId)
            .NotEmpty()
            .MustAsync((deviceId, cancellationToken) =>
                db.Owners.AnyAsync(o => o.DeviceId == deviceId, cancellationToken)
            );
        RuleFor(x => x.Body.EventType).NotEmpty();
        RuleFor(x => x.Body.EventData).NotEmpty();
    }
}