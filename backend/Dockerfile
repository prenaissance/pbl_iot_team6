FROM mcr.microsoft.com/dotnet/sdk:8.0 AS build
COPY Dispenser.sln Dispenser.sln
COPY src/Dispenser.csproj src/Dispenser.csproj
RUN dotnet restore
COPY src/ src/
RUN dotnet publish src/Dispenser.csproj -c Release -o /app

FROM mcr.microsoft.com/dotnet/aspnet:8.0 AS runtime
ENV Logging__LogLevel__Default=Information
ENV Logging__LogLevel__Microsoft.AspNetCore=Warning
WORKDIR /app
COPY --from=build /app .
EXPOSE 5050 7070
ENTRYPOINT ["dotnet", "Dispenser.dll"]