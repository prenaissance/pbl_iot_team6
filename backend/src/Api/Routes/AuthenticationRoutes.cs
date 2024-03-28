using Dispenser.Api.Extensions;
using Dispenser.Api.Filters;
using Dispenser.DataAccess;
using Dispenser.Dtos.Authentication;
using Dispenser.Models.Profiles;
using Dispenser.Services.Authentication;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.OpenApi.Models;

namespace Dispenser.Api;

public static class AuthenticationRoutes
{
    public static IEndpointRouteBuilder RegisterAuthenticationRoutes(this IEndpointRouteBuilder routes)
    {
        var group = routes.MapGroup("/authentication");
        group.MapPost("/register", Register)
            .WithValidation<RegistrationDto>()
            .WithDescription("Registers a new user and their device and returns a JWT token");
        group.MapPost("/login", Login)
            .WithDescription("Logs in the user and returns a JWT token")
            .WithExample<LoginDto>(new("admin", "Test12345!"));

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