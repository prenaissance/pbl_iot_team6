using Dispenser.Api.Extensions;
using Dispenser.DataAccess;
using Dispenser.Dtos.Profiles;
using Dispenser.Models.Profiles;
using Dispenser.Services.Authentication;

namespace Dispenser.Api.Routes;

public static class ProfileRoutes
{
    public static IEndpointRouteBuilder RegisterProfileRoutes(this IEndpointRouteBuilder routes)
    {
        var group = routes.MapGroup("/profiles");

        group.MapGet("/api/profiles/{id}", GetProfile).WithAuthorization();
        group.MapPost("/api/profiles", AddProfile).WithAuthorization();

        return routes;
    }

    private static async Task<IResult> GetProfile(
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

    private static async Task<IResult> AddProfile(
        AddProfileRequest profileRequest,
        ICallerService callerService,
        Db db)
    {
        var callerData = callerService.GetCallerData();
        var profile = new Profile
        {
            Username = profileRequest.Username,
            CreatedById = callerData.Id
        };
        db.Profiles.Add(profile);
        await db.SaveChangesAsync();

        return TypedResults.Created($"/api/profiles/{profile.Id}", profile);
    }
}