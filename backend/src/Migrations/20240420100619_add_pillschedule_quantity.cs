using System;
using System.Collections.Generic;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Dispenser.Migrations
{
    /// <inheritdoc />
    public partial class add_pillschedule_quantity : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<int>(
                name: "Quantity",
                table: "PillSchedules",
                type: "integer",
                nullable: false,
                defaultValue: 0);

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b1b1b1b1-b1b1-b1b1-b1b1-b1b1b1b1b1b1"),
                columns: new[] { "Quantity", "Times" },
                values: new object[] { 0, new List<TimeOnly> { new TimeOnly(8, 0, 0), new TimeOnly(20, 0, 0) } });

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b2b2b2b2-b2b2-b2b2-b2b2-b2b2b2b2b2b2"),
                columns: new[] { "Quantity", "Times" },
                values: new object[] { 0, new List<TimeOnly> { new TimeOnly(12, 0, 0) } });

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("c1c1c1c1-c1c1-c1c1-c1c1-c1c1c1c1c1c1"),
                columns: new[] { "Quantity", "Times" },
                values: new object[] { 0, new List<TimeOnly> { new TimeOnly(7, 0, 0), new TimeOnly(19, 0, 0) } });

            migrationBuilder.UpdateData(
                table: "ProcessedEvents",
                keyColumn: "Id",
                keyValue: new Guid("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"),
                columns: new[] { "CreatedAt", "EventData" },
                values: new object[] { new DateTime(2024, 4, 20, 10, 6, 19, 177, DateTimeKind.Utc).AddTicks(6934), "{\r\n  \"slotNumber\": 1,\r\n  \"pillName\": \"Pill 1\"\r\n}" });

            migrationBuilder.UpdateData(
                table: "Profiles",
                keyColumn: "Id",
                keyValue: 1,
                column: "RfidTag_CreatedAt",
                value: new DateTime(2024, 4, 20, 10, 6, 19, 177, DateTimeKind.Utc).AddTicks(4479));
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Quantity",
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
                values: new object[] { new DateTime(2024, 4, 16, 10, 24, 41, 534, DateTimeKind.Utc).AddTicks(7609), "{\n  \"slotNumber\": 1,\n  \"pillName\": \"Pill 1\"\n}" });

            migrationBuilder.UpdateData(
                table: "Profiles",
                keyColumn: "Id",
                keyValue: 1,
                column: "RfidTag_CreatedAt",
                value: new DateTime(2024, 4, 16, 10, 24, 41, 534, DateTimeKind.Utc).AddTicks(5589));
        }
    }
}
