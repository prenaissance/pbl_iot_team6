using System;
using System.Collections.Generic;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Dispenser.Migrations
{
    /// <inheritdoc />
    public partial class add_pillschedule_unique_index : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropIndex(
                name: "IX_PillSchedules_PillSlotId",
                table: "PillSchedules");

            // delete existing duplicates against the new unique index
            migrationBuilder.Sql("""
                DELETE FROM "public"."PillSchedules"
                WHERE "Id" NOT IN (
                    SELECT DISTINCT ON ("PillSlotId", "ProfileId") "Id"
                    FROM "public"."PillSchedules"
                );
            """);

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
                values: new object[] { new DateTime(2024, 5, 1, 13, 2, 22, 84, DateTimeKind.Utc).AddTicks(7992), "{\n  \"slotNumber\": 1,\n  \"pillName\": \"Pill 1\"\n}" });

            migrationBuilder.UpdateData(
                table: "Profiles",
                keyColumn: "Id",
                keyValue: 1,
                column: "RfidTag_CreatedAt",
                value: new DateTime(2024, 5, 1, 13, 2, 22, 84, DateTimeKind.Utc).AddTicks(5767));

            migrationBuilder.CreateIndex(
                name: "IX_PillSchedules_PillSlotId_ProfileId",
                table: "PillSchedules",
                columns: new[] { "PillSlotId", "ProfileId" },
                unique: true);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropIndex(
                name: "IX_PillSchedules_PillSlotId_ProfileId",
                table: "PillSchedules");

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
                values: new object[] { new DateTime(2024, 4, 21, 8, 34, 17, 361, DateTimeKind.Utc).AddTicks(2532), "{\r\n  \"slotNumber\": 1,\r\n  \"pillName\": \"Pill 1\"\r\n}" });

            migrationBuilder.UpdateData(
                table: "Profiles",
                keyColumn: "Id",
                keyValue: 1,
                column: "RfidTag_CreatedAt",
                value: new DateTime(2024, 4, 21, 8, 34, 17, 360, DateTimeKind.Utc).AddTicks(8207));

            migrationBuilder.CreateIndex(
                name: "IX_PillSchedules_PillSlotId",
                table: "PillSchedules",
                column: "PillSlotId");
        }
    }
}
