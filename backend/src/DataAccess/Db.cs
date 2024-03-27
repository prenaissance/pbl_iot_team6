using Dispenser.Models.Audit;
using Dispenser.Models.PillSchedules;
using Dispenser.Models.Profiles;
using Microsoft.EntityFrameworkCore;

namespace Dispenser.DataAccess;

public class Db(DbContextOptions options) : DbContext(options)
{
    protected override void OnModelCreating(ModelBuilder builder)
    {
        var owner = builder.Entity<Owner>();
        owner.HasMany(o => o.CreatedProfiles)
            .WithOne(p => p.CreatedBy)
            .HasForeignKey(p => p.CreatedById)
            .OnDelete(DeleteBehavior.Cascade);
        owner.HasIndex(o => o.Username)
            .IsUnique();
        owner.HasIndex(o => o.DeviceId)
            .IsUnique();

        builder.Entity<Profile>()
            .HasOne(p => p.RfidTag)
            .WithOne(t => t.Profile)
            .HasForeignKey<RfidTag>(t => t.ProfileId)
            .OnDelete(DeleteBehavior.Cascade);

        var processedEvent = builder.Entity<ProcessedEvent>();
        processedEvent.Property(e => e.EventData)
            .HasColumnType("jsonb");
        processedEvent.Property(e => e.CreatedAt)
            .HasDefaultValueSql("now()");

        builder.Entity<PillSchedule>()
            .HasOne(s => s.Profile)
            .WithMany(p => p.PillSchedules)
            .HasForeignKey(s => s.ProfileId)
            .OnDelete(DeleteBehavior.Cascade);
    }
    public DbSet<PillSlot> PillSlots { get; set; }
    public DbSet<PillSchedule> PillSchedules { get; set; }
    public DbSet<Owner> Owners { get; set; }
    public DbSet<Profile> Profiles { get; set; }
    public DbSet<RfidTag> RfidTags { get; set; }
    public DbSet<ProcessedEvent> ProcessedEvents { get; set; }
}