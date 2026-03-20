#include <stdlib.h>
#include <stdio.h>

#include "bus.h"

struct bus
{
    car_t *car;
    int seatCount;
};

static bus_ops_t bus_ops = {
    bus_drive,
    bus_destroy,
};

bus_ops_t *bus_get_ops(void)
{
    return &bus_ops;
}

bus_t *bus_create(const char *licensePlate, enum car_type type, enum car_color color, date_t productionDate, date_t releaseDate, int seatCount)
{
    bus_t *bus = (bus_t *)malloc(sizeof(bus_t));
    if (NULL == bus)
    {
        fprintf(stderr, "Failed to allocate memory for bus\n");
        return NULL;
    }

    bus->car = car_create(licensePlate, type, color, productionDate, releaseDate);
    if (NULL == bus->car)
    {
        fprintf(stderr, "Failed to create car for bus\n");
        free(bus);
        return NULL;
    }

    bus->seatCount = seatCount;
    return bus;
}

void bus_destroy(bus_t *bus)
{
    if (NULL == bus)
    {
        fprintf(stderr, "Bus is NULL\n");
        return;
    }

    car_destroy(bus->car);
    free(bus);
}

void bus_drive(bus_t *bus, date_t date)
{
    if (NULL == bus)
    {
        fprintf(stderr, "Bus is NULL\n");
        return;
    }

    car_drive(bus->car, date);
    printf("Bus is driving with %d seats on %d-%d-%d\n", bus->seatCount, date.year, date.month, date.day);
}