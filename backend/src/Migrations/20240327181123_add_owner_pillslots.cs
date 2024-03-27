using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Dispenser.Migrations
{
    /// <inheritdoc />
    public partial class add_owner_pillslots : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<int>(
                name: "OwnerId",
                table: "PillSlots",
                type: "integer",
                nullable: false,
                defaultValue: 0);

            migrationBuilder.AddColumn<int>(
                name: "SlotNumber",
                table: "PillSlots",
                type: "integer",
                nullable: false,
                defaultValue: 0);

            migrationBuilder.CreateIndex(
                name: "IX_PillSlots_OwnerId",
                table: "PillSlots",
                column: "OwnerId");

            migrationBuilder.CreateIndex(
                name: "IX_Owners_DeviceId",
                table: "Owners",
                column: "DeviceId",
                unique: true);

            migrationBuilder.AddForeignKey(
                name: "FK_PillSlots_Owners_OwnerId",
                table: "PillSlots",
                column: "OwnerId",
                principalTable: "Owners",
                principalColumn: "Id",
                onDelete: ReferentialAction.Cascade);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropForeignKey(
                name: "FK_PillSlots_Owners_OwnerId",
                table: "PillSlots");

            migrationBuilder.DropIndex(
                name: "IX_PillSlots_OwnerId",
                table: "PillSlots");

            migrationBuilder.DropIndex(
                name: "IX_Owners_DeviceId",
                table: "Owners");

            migrationBuilder.DropColumn(
                name: "OwnerId",
                table: "PillSlots");

            migrationBuilder.DropColumn(
                name: "SlotNumber",
                table: "PillSlots");
        }
    }
}
