#include <stdlib.h>
#include <stdio.h>

#include "bus.h"

struct bus
{
    car_t car;
    int seatCount;
};

bus_t *bus_create(const char *licensePlate, enum car_type type, enum car_color color, date_t productionDate, date_t releaseDate, int seatCount)
{
    bus_t *bus = (bus_t *)malloc(sizeof(bus_t));
    if (NULL == bus)
    {
        fprintf(stderr, "Failed to allocate memory for bus\n");
        return NULL;
    }

    car_init(&bus->car, licensePlate, type, color, productionDate, releaseDate);
    bus->seatCount = seatCount;
    car_ops_t *ops = get_car_ops(&bus->car);
    ops->drive = bus_drive;
    ops->destroy = bus_destroy;
    return bus;
}

void bus_destroy(car_t *car)
{
    if (NULL == car)
    {
        fprintf(stderr, "Bus is NULL\n");
        return;
    }

    bus_t *bus = (bus_t *)car;
    free(bus);
}

void bus_drive(car_t *car, date_t date)
{
    if (NULL == car)
    {
        fprintf(stderr, "Bus is NULL\n");
        return;
    }

    bus_t *bus = (bus_t *)car;
    car_drive(&bus->car, date);
    printf("Bus is driving with %d seats on %d-%d-%d\n", bus->seatCount, date.year, date.month, date.day);
}