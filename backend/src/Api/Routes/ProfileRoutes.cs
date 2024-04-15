using Dispenser.Api.Extensions;
using Dispenser.DataAccess;
using Dispenser.Dtos.Profiles;
using Dispenser.Models.Profiles;
using Dispenser.Services.Authentication;
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
        group.MapGet("/{id}", GetProfile).WithAuthorization().WithName(GET_PROFILE_NAME);
        group.MapPost("", AddProfile).WithAuthorization();
        group.MapDelete("/{id}", DeleteProfile).WithAuthorization();

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

    private static async Task<Ok<Profile[]>> GetOwnProfiles(
        Db db,
        ICallerService callerService)
    {
        var callerData = callerService.GetCallerData();
        var profiles = await db.Profiles
            .Where(p => p.CreatedById == callerData.Id)
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
}