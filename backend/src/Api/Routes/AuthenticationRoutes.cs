using Dispenser.Api.Filters;
using Dispenser.DataAccess;
using Dispenser.Dtos.Authentication;
using Dispenser.Models.Profiles;
using Dispenser.Services.Authentication;
using Microsoft.AspNetCore.Http.HttpResults;

namespace Dispenser.Api;

public static class AuthenticationRoutes
{
    public static IEndpointRouteBuilder RegisterAuthenticationRoutes(this IEndpointRouteBuilder routes)
    {
        var group = routes.MapGroup("/authentication");
        group.MapPost("/register", Register).WithValidation<RegistrationDto>();
        group.MapPost("/login", Login);

        return routes;
    }
    private static Ok<JwtResponse> Register(RegistrationDto registrationDto, Db db, IJwtService jwtService)
    {
        var owner = new Owner
        {
            Username = registrationDto.Username,
            DeviceId = registrationDto.DeviceId
        };
        owner.SetPassword(registrationDto.Password);

        db.Owners.Add(owner);
        db.SaveChanges();

        string jwt = jwtService.GenerateJwtToken(owner);
        return TypedResults.Ok(new JwtResponse(jwt));
    }

    private static Results<Ok<JwtResponse>, UnauthorizedHttpResult> Login(LoginDto loginDto, Db db, IJwtService jwtService)
    {
        var owner = db.Owners
            .SingleOrDefault(o => o.Username == loginDto.Username);

        if (owner is null || !owner.DoesPasswordMatch(loginDto.Password))
        {
            return TypedResults.Unauthorized();
        }

        string jwt = jwtService.GenerateJwtToken(owner);
        return TypedResults.Ok(new JwtResponse(jwt));
    }
}