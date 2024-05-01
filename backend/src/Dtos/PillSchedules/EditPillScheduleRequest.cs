namespace Dispenser.Dtos.PillSchedules;

public record EditPillScheduleRequest(
    Guid Id,
    List<TimeOfDay> Times,
    int Quantity
);
