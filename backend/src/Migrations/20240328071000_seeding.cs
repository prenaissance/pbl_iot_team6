using System;
using System.Collections.Generic;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

#pragma warning disable CA1814 // Prefer jagged arrays over multidimensional

namespace Dispenser.Migrations
{
    /// <inheritdoc />
    public partial class seeding : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<Guid>(
                name: "DeviceId",
                table: "ProcessedEvents",
                type: "uuid",
                nullable: true);

            migrationBuilder.InsertData(
                table: "Owners",
                columns: new[] { "Id", "DeviceId", "PasswordHash", "PasswordSalt", "Username" },
                values: new object[] { 1, new Guid("11111111-1111-1111-1111-111111111111"), new byte[] { 133, 134, 151, 154, 100, 198, 174, 189, 72, 159, 68, 169, 31, 132, 245, 46, 225, 233, 141, 94, 96, 215, 61, 142, 130, 152, 85, 190, 200, 133, 255, 21, 17, 244, 124, 130, 151, 164, 42, 143, 91, 205, 237, 243, 220, 243, 49, 70, 232, 135, 54, 123, 185, 211, 114, 132, 119, 214, 44, 186, 221, 2, 184, 43 }, new byte[] { 165, 175, 148, 227, 243, 204, 158, 75, 32, 80, 159, 241, 212, 93, 40, 79, 98, 142, 22, 133, 71, 252, 74, 72, 26, 157, 216, 154, 68, 28, 193, 217, 44, 0, 130, 193, 86, 254, 100, 172, 94, 33, 146, 181, 161, 31, 42, 113, 88, 25, 190, 215, 43, 13, 175, 195, 224, 68, 120, 25, 97, 89, 10, 165, 227, 225, 251, 169, 143, 145, 248, 227, 103, 81, 166, 4, 77, 0, 79, 80, 197, 172, 65, 59, 240, 48, 92, 33, 243, 200, 10, 157, 80, 198, 195, 131, 130, 117, 108, 238, 118, 151, 64, 92, 62, 149, 19, 83, 89, 57, 127, 158, 14, 12, 122, 183, 161, 222, 194, 98, 139, 234, 44, 9, 61, 84, 195, 216 }, "admin" });

            migrationBuilder.InsertData(
                table: "ProcessedEvents",
                columns: new[] { "Id", "CreatedAt", "DeviceId", "EventData", "EventType", "ProfileId" },
                values: new object[] { new Guid("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"), new DateTime(2024, 3, 28, 7, 10, 0, 644, DateTimeKind.Utc).AddTicks(6931), new Guid("11111111-1111-1111-1111-111111111111"), "{\n  \"slotNumber\": 1,\n  \"pillName\": \"Pill 1\"\n}", "PillDispensed", null });

            migrationBuilder.InsertData(
                table: "PillSlots",
                columns: new[] { "Id", "OwnerId", "PillCount", "PillName", "SlotNumber" },
                values: new object[,]
                {
                    { new Guid("aca05c8c-b4f9-4716-ab98-27fa259bc114"), 1, 0L, "Pill 1", 1 },
                    { new Guid("c7bb0dce-793d-48be-8ede-8f01ffd5615a"), 1, 0L, "Pill 2", 2 }
                });

            migrationBuilder.InsertData(
                table: "Profiles",
                columns: new[] { "Id", "CreatedById", "Username" },
                values: new object[,]
                {
                    { 1, 1, "Little Brother" },
                    { 2, 1, "Grandma" }
                });

            migrationBuilder.InsertData(
                table: "PillSchedules",
                columns: new[] { "Id", "PillSlotId", "ProfileId", "Times" },
                values: new object[,]
                {
                    { new Guid("b1b1b1b1-b1b1-b1b1-b1b1-b1b1b1b1b1b1"), new Guid("aca05c8c-b4f9-4716-ab98-27fa259bc114"), 1, new List<TimeOnly> { new TimeOnly(8, 0, 0), new TimeOnly(20, 0, 0) } },
                    { new Guid("b2b2b2b2-b2b2-b2b2-b2b2-b2b2b2b2b2b2"), new Guid("c7bb0dce-793d-48be-8ede-8f01ffd5615a"), 1, new List<TimeOnly> { new TimeOnly(12, 0, 0) } },
                    { new Guid("c1c1c1c1-c1c1-c1c1-c1c1-c1c1c1c1c1c1"), new Guid("aca05c8c-b4f9-4716-ab98-27fa259bc114"), 2, new List<TimeOnly> { new TimeOnly(7, 0, 0), new TimeOnly(19, 0, 0) } }
                });
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DeleteData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b1b1b1b1-b1b1-b1b1-b1b1-b1b1b1b1b1b1"));

            migrationBuilder.DeleteData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("b2b2b2b2-b2b2-b2b2-b2b2-b2b2b2b2b2b2"));

            migrationBuilder.DeleteData(
                table: "PillSchedules",
                keyColumn: "Id",
                keyValue: new Guid("c1c1c1c1-c1c1-c1c1-c1c1-c1c1c1c1c1c1"));

            migrationBuilder.DeleteData(
                table: "ProcessedEvents",
                keyColumn: "Id",
                keyValue: new Guid("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"));

            migrationBuilder.DeleteData(
                table: "PillSlots",
                keyColumn: "Id",
                keyValue: new Guid("aca05c8c-b4f9-4716-ab98-27fa259bc114"));

            migrationBuilder.DeleteData(
                table: "PillSlots",
                keyColumn: "Id",
                keyValue: new Guid("c7bb0dce-793d-48be-8ede-8f01ffd5615a"));

            migrationBuilder.DeleteData(
                table: "Profiles",
                keyColumn: "Id",
                keyValue: 1);

            migrationBuilder.DeleteData(
                table: "Profiles",
                keyColumn: "Id",
                keyValue: 2);

            migrationBuilder.DeleteData(
                table: "Owners",
                keyColumn: "Id",
                keyValue: 1);

            migrationBuilder.DropColumn(
                name: "DeviceId",
                table: "ProcessedEvents");
        }
    }
}
