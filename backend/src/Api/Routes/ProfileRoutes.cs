using Dispenser.Api.Extensions;
using Dispenser.Api.Filters;
using Dispenser.DataAccess;
using Dispenser.Dtos.PillSchedules;
using Dispenser.Dtos.Profiles;
using Dispenser.Models.Profiles;
using Dispenser.Services.Identity;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.EntityFrameworkCore;

namespace Dispenser.Api.Routes;

public static class ProfileRoutes
{
    public const string GET_PROFILE_NAME = "GetProfile";
    public static IEndpointRouteBuilder RegisterProfileRoutes(this IEndpointRouteBuilder routes)
    {
        var group = routes.MapGroup("/profiles");

        group.MapGet("", GetOwnProfiles).WithAuthorization();
        group.MapGet("/{id}", GetProfile)
            .WithAuthorization()
            .WithName(GET_PROFILE_NAME);
        group.MapPost("", AddProfile).WithAuthorization();
        group.MapDelete("/{id}", DeleteProfile).WithAuthorization();
        group.MapPatch("/rfid", SetRfid)
            .WithAuthorization()
            .WithValidation<SetRfidRequest>()
            .WithDescription("Set the RFID for a profile. Rfid encoded in base64.");

        return routes;
    }

    private static async Task<Results<Ok<Profile>, NotFound>> GetProfile(
        int id,
        Db db)
    {
        var profile = await db.Profiles.FindAsync(id);
        if (profile is null)
        {
            return TypedResults.NotFound();
        }

        return TypedResults.Ok(profile);
    }

    private static async Task<Ok<ProfileWithSchedulesResponse[]>> GetOwnProfiles(
        Db db,
        ICallerService callerService)
    {
        var callerData = callerService.GetCallerData();
        var profiles = await db.Profiles
            .AsNoTracking()
            .Where(p => p.CreatedById == callerData.Id)
            .Include(p => p.PillSchedules)
            .Select(p => p.ToProfileWithSchedulesResponse())
            .ToArrayAsync();

        return TypedResults.Ok(profiles);
    }


    private static async Task<Created<Profile>> AddProfile(
        AddProfileRequest profileRequest,
        ICallerService callerService,
        Db db,
        LinkGenerator linkGenerator)
    {
        var callerData = callerService.GetCallerData();
        var profile = new Profile
        {
            Username = profileRequest.Username,
            CreatedById = callerData.Id
        };
        db.Profiles.Add(profile);
        await db.SaveChangesAsync();

        return TypedResults.Created(
            linkGenerator.GetPathByName(GET_PROFILE_NAME, new { id = profile.Id }),
            profile);
    }

    private static async Task<Results<NoContent, NotFound, BadRequest>> DeleteProfile(
        int id,
        Db db,
        ICallerService callerService)
    {
        var profile = await db.Profiles.FindAsync(id);
        if (profile is null)
        {
            return TypedResults.NotFound();
        }
        var callerData = callerService.GetCallerData();
        if (profile.CreatedById != callerData.Id)
        {
            return TypedResults.BadRequest();
        }

        db.Profiles.Remove(profile);
        await db.SaveChangesAsync();

        return TypedResults.NoContent();
    }

    private static async Task<Results<NoContent, NotFound>> SetRfid(
        SetRfidRequest request,
        Db db,
        ICallerService callerService
    )
    {
        var callerData = callerService.GetCallerData();
        var ownerId = callerData.Id;
        var profile = await db.Profiles
            .Where(p => p.CreatedById == ownerId)
            .FirstOrDefaultAsync(p => p.Id == request.ProfileId);

        if (profile is null)
        {
            return TypedResults.NotFound();
        }

        profile.RfidTag = new RfidTag
        {
            Rfid = request.Rfid
        };

        await db.SaveChangesAsync();

        return TypedResults.NoContent();
    }
}