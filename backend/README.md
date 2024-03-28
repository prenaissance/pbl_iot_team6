# Dispenser Backend

This webapi is a backend, which is responsile for the management application fo the dispenser, giving the dispenser the configuration and processing events from dispensers.

## Documentation

The documentation is in the form of a swagger page, which after running the project can be accessed at http://localhost:5050/swagger/index.html

## Running the project

To run the project, you need to have docker installed. After that, you run the following command:

```bash
sudo docker compose -f=docker-compose.yml -f=docker-compose.prod.yml up --build
```

The api can be accessed on port 5050.
