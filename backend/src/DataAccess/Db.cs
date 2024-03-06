using Dispenser.Models.PillSchedule;
using Microsoft.EntityFrameworkCore;

namespace Dispenser.DataAccess;

public class Db(DbContextOptions options) : DbContext(options)
{

    public DbSet<PillSlot> PillSlots { get; set; }
    public DbSet<PillSchedule> PillSchedules { get; set; }
}