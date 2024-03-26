namespace Dispenser.Api.Filters;

public static class ValidationFilterExtensions
{
    public static RouteHandlerBuilder WithValidation<ArgT>(this RouteHandlerBuilder builder) =>
        builder.AddEndpointFilter<ValidationFilter<ArgT>>();
}