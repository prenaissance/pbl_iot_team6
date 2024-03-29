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
    [Migration("20240327181123_add_owner_pillslots")]
    partial class add_owner_pillslots
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
                });

            modelBuilder.Entity("Dispenser.Models.Profiles.RfidTag", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("integer");

                    NpgsqlPropertyBuilderExtensions.UseIdentityByDefaultColumn(b.Property<int>("Id"));

                    b.Property<string>("Description")
                        .HasColumnType("text");

                    b.Property<int>("ProfileId")
                        .HasColumnType("integer");

                    b.Property<string>("Tag")
                        .IsRequired()
                        .HasColumnType("text");

                    b.HasKey("Id");

                    b.HasIndex("ProfileId")
                        .IsUnique();

                    b.ToTable("RfidTags");
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

                    b.Navigation("CreatedBy");
                });

            modelBuilder.Entity("Dispenser.Models.Profiles.RfidTag", b =>
                {
                    b.HasOne("Dispenser.Models.Profiles.Profile", "Profile")
                        .WithOne("RfidTag")
                        .HasForeignKey("Dispenser.Models.Profiles.RfidTag", "ProfileId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.Navigation("Profile");
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

                    b.Navigation("RfidTag")
                        .IsRequired();
                });
#pragma warning restore 612, 618
        }
    }
}
