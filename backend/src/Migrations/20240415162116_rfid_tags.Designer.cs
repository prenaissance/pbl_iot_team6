﻿// <auto-generated />
using System;
using System.Collections.Generic;
using Dispenser.DataAccess;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Infrastructure;
using Microsoft.EntityFrameworkCore.Migrations;
using Microsoft.EntityFrameworkCore.Storage.ValueConversion;
using Npgsql.EntityFrameworkCore.PostgreSQL.Metadata;

#nullable disable

namespace Dispenser.Migrations
{
    [DbContext(typeof(Db))]
    [Migration("20240415162116_rfid_tags")]
    partial class rfid_tags
    {
        /// <inheritdoc />
        protected override void BuildTargetModel(ModelBuilder modelBuilder)
        {
#pragma warning disable 612, 618
            modelBuilder
                .HasAnnotation("ProductVersion", "8.0.2")
                .HasAnnotation("Relational:MaxIdentifierLength", 63);

            NpgsqlModelBuilderExtensions.UseIdentityByDefaultColumns(modelBuilder);

            modelBuilder.Entity("Dispenser.Models.Audit.ProcessedEvent", b =>
                {
                    b.Property<Guid>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("uuid");

                    b.Property<DateTime>("CreatedAt")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("timestamp with time zone")
                        .HasDefaultValueSql("now()");

                    b.Property<Guid?>("DeviceId")
                        .HasColumnType("uuid");

                    b.Property<string>("EventData")
                        .IsRequired()
                        .HasColumnType("jsonb");

                    b.Property<string>("EventType")
                        .IsRequired()
                        .HasColumnType("text");

                    b.Property<int?>("ProfileId")
                        .HasColumnType("integer");

                    b.HasKey("Id");

                    b.HasIndex("ProfileId");

                    b.ToTable("ProcessedEvents");

                    b.HasData(
                        new
                        {
                            Id = new Guid("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"),
                            CreatedAt = new DateTime(2024, 4, 15, 16, 21, 15, 682, DateTimeKind.Utc).AddTicks(9697),
                            DeviceId = new Guid("11111111-1111-1111-1111-111111111111"),
                            EventData = "{\r\n  \"slotNumber\": 1,\r\n  \"pillName\": \"Pill 1\"\r\n}",
                            EventType = "PillDispensed"
                        });
                });

            modelBuilder.Entity("Dispenser.Models.PillSchedules.PillSchedule", b =>
                {
                    b.Property<Guid>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("uuid");

                    b.Property<Guid>("PillSlotId")
                        .HasColumnType("uuid");

                    b.Property<int>("ProfileId")
                        .HasColumnType("integer");

                    b.Property<List<TimeOnly>>("Times")
                        .IsRequired()
                        .HasColumnType("time without time zone[]");

                    b.HasKey("Id");

                    b.HasIndex("PillSlotId");

                    b.HasIndex("ProfileId");

                    b.ToTable("PillSchedules");

                    b.HasData(
                        new
                        {
                            Id = new Guid("b1b1b1b1-b1b1-b1b1-b1b1-b1b1b1b1b1b1"),
                            PillSlotId = new Guid("aca05c8c-b4f9-4716-ab98-27fa259bc114"),
                            ProfileId = 1,
                            Times = new List<TimeOnly> { new TimeOnly(8, 0, 0), new TimeOnly(20, 0, 0) }
                        },
                        new
                        {
                            Id = new Guid("b2b2b2b2-b2b2-b2b2-b2b2-b2b2b2b2b2b2"),
                            PillSlotId = new Guid("c7bb0dce-793d-48be-8ede-8f01ffd5615a"),
                            ProfileId = 1,
                            Times = new List<TimeOnly> { new TimeOnly(12, 0, 0) }
                        },
                        new
                        {
                            Id = new Guid("c1c1c1c1-c1c1-c1c1-c1c1-c1c1c1c1c1c1"),
                            PillSlotId = new Guid("aca05c8c-b4f9-4716-ab98-27fa259bc114"),
                            ProfileId = 2,
                            Times = new List<TimeOnly> { new TimeOnly(7, 0, 0), new TimeOnly(19, 0, 0) }
                        });
                });

            modelBuilder.Entity("Dispenser.Models.PillSchedules.PillSlot", b =>
                {
                    b.Property<Guid>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("uuid");

                    b.Property<int>("OwnerId")
                        .HasColumnType("integer");

                    b.Property<long>("PillCount")
                        .HasColumnType("bigint");

                    b.Property<string>("PillName")
                        .IsRequired()
                        .HasColumnType("text");

                    b.Property<int>("SlotNumber")
                        .HasColumnType("integer");

                    b.HasKey("Id");

                    b.HasIndex("OwnerId");

                    b.ToTable("PillSlots");

                    b.HasData(
                        new
                        {
                            Id = new Guid("aca05c8c-b4f9-4716-ab98-27fa259bc114"),
                            OwnerId = 1,
                            PillCount = 0L,
                            PillName = "Pill 1",
                            SlotNumber = 1
                        },
                        new
                        {
                            Id = new Guid("c7bb0dce-793d-48be-8ede-8f01ffd5615a"),
                            OwnerId = 1,
                            PillCount = 0L,
                            PillName = "Pill 2",
                            SlotNumber = 2
                        });
                });

            modelBuilder.Entity("Dispenser.Models.Profiles.Owner", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("integer");

                    NpgsqlPropertyBuilderExtensions.UseIdentityByDefaultColumn(b.Property<int>("Id"));

                    b.Property<Guid>("DeviceId")
                        .HasColumnType("uuid");

                    b.Property<byte[]>("PasswordHash")
                        .IsRequired()
                        .HasColumnType("bytea");

                    b.Property<byte[]>("PasswordSalt")
                        .IsRequired()
                        .HasColumnType("bytea");

                    b.Property<string>("Username")
                        .IsRequired()
                        .HasColumnType("text");

                    b.HasKey("Id");

                    b.HasIndex("DeviceId")
                        .IsUnique();

                    b.HasIndex("Username")
                        .IsUnique();

                    b.ToTable("Owners");

                    b.HasData(
                        new
                        {
                            Id = 1,
                            DeviceId = new Guid("11111111-1111-1111-1111-111111111111"),
                            PasswordHash = new byte[] { 133, 134, 151, 154, 100, 198, 174, 189, 72, 159, 68, 169, 31, 132, 245, 46, 225, 233, 141, 94, 96, 215, 61, 142, 130, 152, 85, 190, 200, 133, 255, 21, 17, 244, 124, 130, 151, 164, 42, 143, 91, 205, 237, 243, 220, 243, 49, 70, 232, 135, 54, 123, 185, 211, 114, 132, 119, 214, 44, 186, 221, 2, 184, 43 },
                            PasswordSalt = new byte[] { 165, 175, 148, 227, 243, 204, 158, 75, 32, 80, 159, 241, 212, 93, 40, 79, 98, 142, 22, 133, 71, 252, 74, 72, 26, 157, 216, 154, 68, 28, 193, 217, 44, 0, 130, 193, 86, 254, 100, 172, 94, 33, 146, 181, 161, 31, 42, 113, 88, 25, 190, 215, 43, 13, 175, 195, 224, 68, 120, 25, 97, 89, 10, 165, 227, 225, 251, 169, 143, 145, 248, 227, 103, 81, 166, 4, 77, 0, 79, 80, 197, 172, 65, 59, 240, 48, 92, 33, 243, 200, 10, 157, 80, 198, 195, 131, 130, 117, 108, 238, 118, 151, 64, 92, 62, 149, 19, 83, 89, 57, 127, 158, 14, 12, 122, 183, 161, 222, 194, 98, 139, 234, 44, 9, 61, 84, 195, 216 },
                            Username = "admin"
                        });
                });

            modelBuilder.Entity("Dispenser.Models.Profiles.Profile", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("integer");

                    NpgsqlPropertyBuilderExtensions.UseIdentityByDefaultColumn(b.Property<int>("Id"));

                    b.Property<int>("CreatedById")
                        .HasColumnType("integer");

                    b.Property<string>("Username")
                        .IsRequired()
                        .HasColumnType("text");

                    b.HasKey("Id");

                    b.HasIndex("CreatedById");

                    b.ToTable("Profiles");

                    b.HasData(
                        new
                        {
                            Id = 1,
                            CreatedById = 1,
                            Username = "Little Brother"
                        },
                        new
                        {
                            Id = 2,
                            CreatedById = 1,
                            Username = "Grandma"
                        });
                });

            modelBuilder.Entity("Dispenser.Models.Audit.ProcessedEvent", b =>
                {
                    b.HasOne("Dispenser.Models.Profiles.Profile", "Profile")
                        .WithMany()
                        .HasForeignKey("ProfileId");

                    b.Navigation("Profile");
                });

            modelBuilder.Entity("Dispenser.Models.PillSchedules.PillSchedule", b =>
                {
                    b.HasOne("Dispenser.Models.PillSchedules.PillSlot", "PillSlot")
                        .WithMany("PillSchedules")
                        .HasForeignKey("PillSlotId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.HasOne("Dispenser.Models.Profiles.Profile", "Profile")
                        .WithMany("PillSchedules")
                        .HasForeignKey("ProfileId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.Navigation("PillSlot");

                    b.Navigation("Profile");
                });

            modelBuilder.Entity("Dispenser.Models.PillSchedules.PillSlot", b =>
                {
                    b.HasOne("Dispenser.Models.Profiles.Owner", "Owner")
                        .WithMany("PillSlots")
                        .HasForeignKey("OwnerId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.Navigation("Owner");
                });

            modelBuilder.Entity("Dispenser.Models.Profiles.Profile", b =>
                {
                    b.HasOne("Dispenser.Models.Profiles.Owner", "CreatedBy")
                        .WithMany("CreatedProfiles")
                        .HasForeignKey("CreatedById")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.OwnsOne("Dispenser.Models.Profiles.RfidTag", "RfidTag", b1 =>
                        {
                            b1.Property<int>("ProfileId")
                                .HasColumnType("integer");

                            b1.Property<DateTime>("CreatedAt")
                                .ValueGeneratedOnAdd()
                                .HasColumnType("timestamp with time zone")
                                .HasDefaultValueSql("now()");

                            b1.Property<byte[]>("Rfid")
                                .IsRequired()
                                .HasColumnType("bytea");

                            b1.HasKey("ProfileId");

                            b1.ToTable("Profiles");

                            b1.WithOwner()
                                .HasForeignKey("ProfileId");

                            b1.HasData(
                                new
                                {
                                    ProfileId = 1,
                                    CreatedAt = new DateTime(2024, 4, 15, 16, 21, 15, 682, DateTimeKind.Utc).AddTicks(6971),
                                    Rfid = new byte[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 }
                                });
                        });

                    b.Navigation("CreatedBy");

                    b.Navigation("RfidTag");
                });

            modelBuilder.Entity("Dispenser.Models.PillSchedules.PillSlot", b =>
                {
                    b.Navigation("PillSchedules");
                });

            modelBuilder.Entity("Dispenser.Models.Profiles.Owner", b =>
                {
                    b.Navigation("CreatedProfiles");

                    b.Navigation("PillSlots");
                });

            modelBuilder.Entity("Dispenser.Models.Profiles.Profile", b =>
                {
                    b.Navigation("PillSchedules");
                });
#pragma warning restore 612, 618
        }
    }
}