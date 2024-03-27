namespace Dispenser.Dtos.PillSchedules;

public record TimeOfDay
{
    public required int Hour { get; set; }
    public required int Minutes { get; set; }
    public TimeOnly ToTimeOnly() => new(Hour, Minutes);
    public static TimeOfDay FromTimeOnly(TimeOnly timeOnly) => new()
    {
        Hour = timeOnly.Hour,
        Minutes = timeOnly.Minute
    };
}