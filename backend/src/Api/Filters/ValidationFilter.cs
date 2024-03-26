using System.Net;
using FluentValidation;

namespace Dispenser.Api.Filters;

public class ValidationFilter<T> : IEndpointFilter
{
    public async ValueTask<object?> InvokeAsync(EndpointFilterInvocationContext context, EndpointFilterDelegate next)
    {
        T? argToValidate = context.GetArgument<T>(0);
        IValidator<T>? validator = context.HttpContext.RequestServices.GetService<IValidator<T>>();

        if (validator is not null)
        {
            var validationResult = await validator.ValidateAsync(argToValidate!);
            if (!validationResult.IsValid)
            {
                var formattedErrors = validationResult.Errors.GroupBy(e => e.PropertyName)
                    .ToDictionary(
                        group => $"{group.Key[0..1].ToLower()}{group.Key[1..]}",
                        group => group.Select(e => e.ErrorMessage).ToArray()
                    );


                return Results.ValidationProblem(formattedErrors,
                    statusCode: (int)HttpStatusCode.UnprocessableEntity);
            }
        }

        // Otherwise invoke the next filter in the pipeline
        return await next.Invoke(context);
    }
}