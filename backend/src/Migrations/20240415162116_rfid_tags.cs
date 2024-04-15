using System;
using System.Collections.Generic;
using Microsoft.EntityFrameworkCore.Migrations;
using Npgsql.EntityFrameworkCore.PostgreSQL.Metadata;

#nullable disable

namespace Dispenser.Migrations
{
    /// <inheritdoc />
    public partial class rfid_tags : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "RfidTags");

            migrationBuilder.AddColumn<DateTime>(
                name: "RfidTag_CreatedAt",
                table: "Profiles",
                type: "timestamp with time zone",
                nullable: true,
                defaultValueSql: "now()");

            migrationBuilder.AddColumn<byte[]>(
                name: "RfidTag_Rfid",
                table: "Profiles",
                type: "bytea",
                nullable: true);

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b1b1b1b1-b1b1-b1b1-b1b1-b1b1b1b1b1b1"),
                column: "Times",
                value: new List<TimeOnly> { new TimeOnly(8, 0, 0), new TimeOnly(20, 0, 0) });

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b2b2b2b2-b2b2-b2b2-b2b2-b2b2b2b2b2b2"),
                column: "Times",
                value: new List<TimeOnly> { new TimeOnly(12, 0, 0) });

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("c1c1c1c1-c1c1-c1c1-c1c1-c1c1c1c1c1c1"),
                column: "Times",
                value: new List<TimeOnly> { new TimeOnly(7, 0, 0), new TimeOnly(19, 0, 0) });

            migrationBuilder.UpdateData(
                table: "ProcessedEvents",
                keyColumn: "Id",
                keyValue: new Guid("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"),
                columns: new[] { "CreatedAt", "EventData" },
                values: new object[] { new DateTime(2024, 4, 15, 16, 21, 15, 682, DateTimeKind.Utc).AddTicks(9697), "{\r\n  \"slotNumber\": 1,\r\n  \"pillName\": \"Pill 1\"\r\n}" });

            migrationBuilder.UpdateData(
                table: "Profiles",
                keyColumn: "Id",
                keyValue: 1,
                columns: new[] { "RfidTag_CreatedAt", "RfidTag_Rfid" },
                values: new object[] { new DateTime(2024, 4, 15, 16, 21, 15, 682, DateTimeKind.Utc).AddTicks(6971), new byte[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 } });
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "RfidTag_CreatedAt",
                table: "Profiles");

            migrationBuilder.DropColumn(
                name: "RfidTag_Rfid",
                table: "Profiles");

            migrationBuilder.CreateTable(
                name: "RfidTags",
                columns: table => new
                {
                    Id = table.Column<int>(type: "integer", nullable: false)
                        .Annotation("Npgsql:ValueGenerationStrategy", NpgsqlValueGenerationStrategy.IdentityByDefaultColumn),
                    ProfileId = table.Column<int>(type: "integer", nullable: false),
                    Description = table.Column<string>(type: "text", nullable: true),
                    Tag = table.Column<string>(type: "text", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_RfidTags", x => x.Id);
                    table.ForeignKey(
                        name: "FK_RfidTags_Profiles_ProfileId",
                        column: x => x.ProfileId,
                        principalTable: "Profiles",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b1b1b1b1-b1b1-b1b1-b1b1-b1b1b1b1b1b1"),
                column: "Times",
                value: new List<TimeOnly> { new TimeOnly(8, 0, 0), new TimeOnly(20, 0, 0) });

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b2b2b2b2-b2b2-b2b2-b2b2-b2b2b2b2b2b2"),
                column: "Times",
                value: new List<TimeOnly> { new TimeOnly(12, 0, 0) });

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("c1c1c1c1-c1c1-c1c1-c1c1-c1c1c1c1c1c1"),
                column: "Times",
                value: new List<TimeOnly> { new TimeOnly(7, 0, 0), new TimeOnly(19, 0, 0) });

            migrationBuilder.UpdateData(
                table: "ProcessedEvents",
                keyColumn: "Id",
                keyValue: new Guid("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"),
                columns: new[] { "CreatedAt", "EventData" },
                values: new object[] { new DateTime(2024, 3, 28, 7, 10, 0, 644, DateTimeKind.Utc).AddTicks(6931), "{\n  \"slotNumber\": 1,\n  \"pillName\": \"Pill 1\"\n}" });

            migrationBuilder.CreateIndex(
                name: "IX_RfidTags_ProfileId",
                table: "RfidTags",
                column: "ProfileId",
                unique: true);
        }
    }
}
