namespace Dispenser.Dtos.PillSchedules;

public record AddPillScheduleRequest(
    List<TimeOfDay> Times,
    int ProfileId,
    int Quantity
);

public record AddPillSchedulesRequest(
    List<AddPillScheduleRequest> PillSchedules
);