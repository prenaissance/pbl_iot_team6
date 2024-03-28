using System.Text.Json;
using Microsoft.OpenApi.Any;
using Microsoft.OpenApi.Models;

namespace Dispenser.Api.Extensions;

public static class OpenApiExtensions
{
    private static readonly JsonSerializerOptions jsonSerializerOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase
    };
    public static RouteHandlerBuilder WithExample<T>(this RouteHandlerBuilder builder, T example)
    {
        builder.WithOpenApi(
            config =>
            {
                config.RequestBody = new OpenApiRequestBody
                {
                    Content = new Dictionary<string, OpenApiMediaType>
                    {
                        ["application/json"] = new OpenApiMediaType
                        {
                            Schema = new OpenApiSchema
                            {
                                Reference = new OpenApiReference
                                {
                                    Id = typeof(T).Name,
                                    Type = ReferenceType.Schema
                                }
                            },
                            Example = new OpenApiString(JsonSerializer.Serialize(example, jsonSerializerOptions))
                        }
                    }
                };
                return config;
            }
        );
        return builder;
    }
}