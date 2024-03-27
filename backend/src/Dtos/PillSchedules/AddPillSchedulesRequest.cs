namespace Dispenser.Dtos.PillSchedules;

public record AddPillScheduleRequest(
    List<TimeOfDay> Times,
    int ProfileId
);

public record AddPillSchedulesRequest(
    List<AddPillScheduleRequest> PillSchedules
);