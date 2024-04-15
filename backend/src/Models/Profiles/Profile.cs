using Dispenser.Models.PillSchedules;

namespace Dispenser.Models.Profiles;

public class Profile
{
    public int Id { get; set; }
    public required string Username { get; set; }
    public int CreatedById { get; set; }
    public Owner CreatedBy { get; set; } = null!;
    public RfidTag? RfidTag { get; set; } = null!;
    public List<PillSchedule> PillSchedules { get; set; } = [];
}