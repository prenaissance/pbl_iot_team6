using System.Reflection;
using System.Text;
using Dispenser.Api;
using Dispenser.DataAccess;
using Dispenser.Services.Authentication;
using FluentValidation;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.EntityFrameworkCore;
using Microsoft.IdentityModel.Tokens;

var builder = WebApplication.CreateBuilder(args);
var configuration = builder.Configuration;

// Add services to the container.
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();
builder.Services.AddValidatorsFromAssembly(Assembly.GetExecutingAssembly(), ServiceLifetime.Transient);
builder.Services.AddDbContext<Db>(options =>
{
    options.UseNpgsql(configuration.GetValue<string>("PostgresConnection"));
});
builder.Services.AddSingleton<IJwtService, JwtService>();
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
            IssuerSigningKey = new SymmetricSecurityKey(Encoding.ASCII.GetBytes(jwtSecret))
        };
    });

var app = builder.Build();

using var scope = app.Services.CreateScope();
Db db = scope.ServiceProvider.GetRequiredService<Db>();
db.Database.Migrate(); // Apply any pending migrations, dev mode

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.UseHttpsRedirection();

var routes = app.MapGroup("/api");

routes.RegisterAuthenticationRoutes();

app.Run();
