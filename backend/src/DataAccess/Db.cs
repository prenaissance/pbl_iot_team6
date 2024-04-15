using System.Text.Json;
using Dispenser.Models.Audit;
using Dispenser.Models.PillSchedules;
using Dispenser.Models.Profiles;
using Microsoft.EntityFrameworkCore;

namespace Dispenser.DataAccess;

public class Db(DbContextOptions options) : DbContext(options)
{
    private static readonly int OWNER_ID = 1;
    private static readonly Guid PILLSLOT1_ID = new("aca05c8c-b4f9-4716-ab98-27fa259bc114");
    private static readonly Guid PILLSLOT2_ID = new("c7bb0dce-793d-48be-8ede-8f01ffd5615a");
    private static readonly Guid DEVICE_ID = new("11111111-1111-1111-1111-111111111111");
    private static readonly JsonSerializerOptions jsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase
    };
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

        owner.HasData([
            new {
                Id = OWNER_ID,
                DeviceId = DEVICE_ID,
                Username = "admin",
                // Password is "Test12345!
                PasswordHash = Convert.FromBase64String("hYaXmmTGrr1In0SpH4T1LuHpjV5g1z2OgphVvsiF/xUR9HyCl6Qqj1vN7fPc8zFG6Ic2e7nTcoR31iy63QK4Kw=="),
                PasswordSalt = Convert.FromBase64String("pa+U4/PMnksgUJ/x1F0oT2KOFoVH/EpIGp3YmkQcwdksAILBVv5krF4hkrWhHypxWBm+1ysNr8PgRHgZYVkKpePh+6mPkfjjZ1GmBE0AT1DFrEE78DBcIfPICp1QxsODgnVs7naXQFw+lRNTWTl/ng4Mereh3sJii+osCT1Uw9g="),
            }
        ]);

        var profile = builder.Entity<Profile>();
        var rfidTag = profile
            .OwnsOne(p => p.RfidTag);

        rfidTag.Property(r => r.CreatedAt)
            .HasDefaultValueSql("now()");
        rfidTag.HasData([new {
                ProfileId = 1,
                Rfid = Enumerable.Range(0, 20).Select(i => (byte)i).ToArray(),
                CreatedAt = DateTime.UtcNow }
            ]);
        profile.HasData([
            new() {
                Id = 1,
                Username = "Little Brother",
                CreatedById = 1,
            },
            new() {
                Id = 2,
                Username = "Grandma",
                CreatedById = 1,
            }
        ]);

        var processedEvent = builder.Entity<ProcessedEvent>();
        processedEvent.Property(e => e.EventData)
            .HasColumnType("jsonb");
        processedEvent.Property(e => e.CreatedAt)
            .HasDefaultValueSql("now()");
        processedEvent.HasData([
            new() {
                Id = new Guid("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"),
                EventType = "PillDispensed",
                EventData = JsonSerializer.Serialize(
                    new { SlotNumber = 1, PillName = "Pill 1" },
                    jsonOptions),
                DeviceId = DEVICE_ID,
            }
        ]);

        var pillSlot = builder.Entity<PillSlot>();

        pillSlot.HasData([
            new() { Id = PILLSLOT1_ID, SlotNumber = 1, PillName = "Pill 1", OwnerId = OWNER_ID},
            new() { Id = PILLSLOT2_ID, SlotNumber = 2, PillName = "Pill 2", OwnerId = OWNER_ID}
        ]);

        var pillSchedule = builder.Entity<PillSchedule>();
        pillSchedule
            .HasOne(s => s.Profile)
            .WithMany(p => p.PillSchedules)
            .HasForeignKey(s => s.ProfileId)
            .OnDelete(DeleteBehavior.Cascade);
        pillSchedule.HasData([
            new() {
                Id = new Guid("b1b1b1b1-b1b1-b1b1-b1b1-b1b1b1b1b1b1"),
                PillSlotId = PILLSLOT1_ID,
                Times = [new(8, 0), new(20, 0)],
                ProfileId = 1
            },
            new() {
                Id = new Guid("b2b2b2b2-b2b2-b2b2-b2b2-b2b2b2b2b2b2"),
                PillSlotId = PILLSLOT2_ID,
                Times = [new(12, 0)],
                ProfileId = 1
            },
            new () {
                Id = new Guid("c1c1c1c1-c1c1-c1c1-c1c1-c1c1c1c1c1c1"),
                PillSlotId = PILLSLOT1_ID,
                Times = [new(7, 0), new(19, 0)],
                ProfileId = 2
            },
        ]);
    }
    public DbSet<PillSlot> PillSlots { get; set; }
    public DbSet<PillSchedule> PillSchedules { get; set; }
    public DbSet<Owner> Owners { get; set; }
    public DbSet<Profile> Profiles { get; set; }
    public DbSet<ProcessedEvent> ProcessedEvents { get; set; }
}