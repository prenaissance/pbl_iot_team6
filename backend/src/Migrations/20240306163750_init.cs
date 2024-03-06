using System;
using System.Collections.Generic;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Dispenser.Migrations
{
    /// <inheritdoc />
    public partial class init : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "PillSlots",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    PillName = table.Column<string>(type: "text", nullable: false),
                    PillCount = table.Column<long>(type: "bigint", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_PillSlots", x => x.Id);
                });

            migrationBuilder.CreateTable(
                name: "PillSchedules",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    Times = table.Column<List<TimeOnly>>(type: "time without time zone[]", nullable: false),
                    PillSlotId = table.Column<Guid>(type: "uuid", nullable: false),
                    ProfileId = table.Column<Guid>(type: "uuid", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_PillSchedules", x => x.Id);
                    table.ForeignKey(
                        name: "FK_PillSchedules_PillSlots_PillSlotId",
                        column: x => x.PillSlotId,
                        principalTable: "PillSlots",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateIndex(
                name: "IX_PillSchedules_PillSlotId",
                table: "PillSchedules",
                column: "PillSlotId");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "PillSchedules");

            migrationBuilder.DropTable(
                name: "PillSlots");
        }
    }
}
