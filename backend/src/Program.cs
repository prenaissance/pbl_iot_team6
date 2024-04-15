using System.Reflection;
using System.Text;
using System.Text.Json;
using Dispenser.Api;
using Dispenser.Api.Routes;
using Dispenser.DataAccess;
using Dispenser.Services.Authentication;
using Dispenser.Services.Identity;
using FluentValidation;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.AspNetCore.HttpLogging;
using Microsoft.EntityFrameworkCore;
using Microsoft.IdentityModel.Tokens;
using Microsoft.OpenApi.Models;

var builder = WebApplication.CreateBuilder(args);
var configuration = builder.Configuration;

string reactNativeOrigins = "react-native-origins";

// Add services to the container.
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen(options =>
{
    options.AddSecurityDefinition("Bearer",
        new OpenApiSecurityScheme
        {
            Name = "Authorization",
            In = ParameterLocation.Header,
            Type = SecuritySchemeType.Http,
            Scheme = "Bearer",
            BearerFormat = "JWT",
            Description = "JWT Authorization header using the Bearer scheme."
        });
});
builder.Services.AddCors(options =>
{
    options.AddPolicy(name: reactNativeOrigins,
                      policy =>
                      {
                          policy.WithOrigins("http://localhost:5037",
                                              "https://localhost:5037");
                      });
});
builder.Services.AddValidatorsFromAssembly(Assembly.GetExecutingAssembly(), ServiceLifetime.Transient);
builder.Services.AddDbContext<Db>(options =>
{
    options.UseNpgsql(configuration.GetValue<string>("PostgresConnection"));
});
builder.Services.AddHttpContextAccessor();
builder.Services.AddSingleton<IJwtService, JwtService>();
builder.Services.AddTransient<ICallerService, CallerService>();
string jwtSecret = configuration.GetValue<string>("JwtSecret") ?? "supersecretkey";
builder.Services.AddAuthentication(JwtBearerDefaults.AuthenticationScheme)
    .AddJwtBearer(options =>
    {
        options.TokenValidationParameters = new TokenValidationParameters
        {
            ValidateIssuer = false,
            ValidateAudience = false,
            ValidateLifetime = true,
            ValidateIssuerSigningKey = true,
            IssuerSigningKey = new SymmetricSecurityKey(Encoding.ASCII.GetBytes(jwtSecret.PadRight(64, ' ')))
        };
    });
builder.Services.AddAuthorization();
builder.Services.AddHttpLogging(logging =>
{
    logging.LoggingFields = 0
        | HttpLoggingFields.RequestMethod
        | HttpLoggingFields.RequestPath
        | HttpLoggingFields.RequestQuery
        | HttpLoggingFields.ResponseStatusCode
        | HttpLoggingFields.Duration;
});

var app = builder.Build();

using var scope = app.Services.CreateScope();
Db db = scope.ServiceProvider.GetRequiredService<Db>();
db.Database.Migrate(); // Apply any pending migrations, dev mode

// Configure the HTTP request pipeline.
app.UseCors(reactNativeOrigins);
app.UseHttpLogging();
app.UseSwagger();
app.UseSwaggerUI();

// app.UseHttpsRedirection();
app.UseAuthentication();
app.UseAuthorization();

var routes = app.MapGroup("/api");

routes
    .RegisterAuthenticationRoutes()
    .RegisterProfileRoutes()
    .RegisterPillRoutes()
    .RegisterDeviceRoutes();

app.Run();
