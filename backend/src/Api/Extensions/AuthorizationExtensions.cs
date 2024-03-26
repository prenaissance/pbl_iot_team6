using Microsoft.OpenApi.Models;

namespace Dispenser.Api.Extensions;

public static class AuthorizationExtensions
{
    public static RouteHandlerBuilder WithAuthorization(this RouteHandlerBuilder builder)
    {
        builder
            .RequireAuthorization()
            .WithOpenApi(options =>
            {
                options.Security.Add(new OpenApiSecurityRequirement
                {
                    {
                        new OpenApiSecurityScheme
                        {
                            Reference = new OpenApiReference
                            {
                                Id = "Bearer",
                                Type = ReferenceType.SecurityScheme
                            }
                        },
                        []
                    }
                });
                return options;
            });

        return builder;
    }
}