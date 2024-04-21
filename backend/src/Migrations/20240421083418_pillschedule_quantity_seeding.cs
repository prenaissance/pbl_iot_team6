using System;
using System.Collections.Generic;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Dispenser.Migrations
{
    /// <inheritdoc />
    public partial class pillschedule_quantity_seeding : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b1b1b1b1-b1b1-b1b1-b1b1-b1b1b1b1b1b1"),
                columns: new[] { "Quantity", "Times" },
                values: new object[] { 1, new List<TimeOnly> { new TimeOnly(8, 0, 0), new TimeOnly(20, 0, 0) } });

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b2b2b2b2-b2b2-b2b2-b2b2-b2b2b2b2b2b2"),
                columns: new[] { "Quantity", "Times" },
                values: new object[] { 2, new List<TimeOnly> { new TimeOnly(12, 0, 0) } });

            migrationBuilder.UpdateData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("c1c1c1c1-c1c1-c1c1-c1c1-c1c1c1c1c1c1"),
                columns: new[] { "Quantity", "Times" },
                values: new object[] { 1, new List<TimeOnly> { new TimeOnly(7, 0, 0), new TimeOnly(19, 0, 0) } });

            migrationBuilder.UpdateData(
                table: "ProcessedEvents",
                keyColumn: "Id",
                keyValue: new Guid("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"),
                column: "CreatedAt",
                value: new DateTime(2024, 4, 21, 8, 34, 17, 361, DateTimeKind.Utc).AddTicks(2532));

            migrationBuilder.UpdateData(
                table: "Profiles",
                keyColumn: "Id",
                keyValue: 1,
                column: "RfidTag_CreatedAt",
                value: new DateTime(2024, 4, 21, 8, 34, 17, 360, DateTimeKind.Utc).AddTicks(8207));
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
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
                column: "CreatedAt",
                value: new DateTime(2024, 4, 20, 10, 6, 19, 177, DateTimeKind.Utc).AddTicks(6934));

            migrationBuilder.UpdateData(
                table: "Profiles",
                keyColumn: "Id",
                keyValue: 1,
                column: "RfidTag_CreatedAt",
                value: new DateTime(2024, 4, 20, 10, 6, 19, 177, DateTimeKind.Utc).AddTicks(4479));
        }
    }
}
