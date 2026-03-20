#include <stdlib.h>
#include <stdio.h>

#include "car.h"

car_t *car_create(const char *licensePlate, enum car_type type, enum car_color color, date_t productionDate, date_t releaseDate)
{
    car_t *car = (car_t *)malloc(sizeof(car_t));
    if (car == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for car\n");
        return NULL;
    }

    car->licensePlate = licensePlate;
    car->type = type;
    car->color = color;
    car->productionDate = productionDate;
    car->releaseDate = releaseDate;

    car->ops.drive = car_drive;
    car->ops.destroy = car_destroy;
    return car;
}

void car_init(car_t *car, const char *licensePlate, enum car_type type, enum car_color color, date_t productionDate, date_t releaseDate)
{
    if (NULL == car)
    {
        return;
    }
    car->licensePlate = licensePlate;
    car->type = type;
    car->color = color;
    car->productionDate = productionDate;
    car->releaseDate = releaseDate;
    car->ops.drive = car_drive;
    car->ops.destroy = car_destroy;
}

void car_drive(car_t *car, date_t date)
{
    if (NULL == car)
    {
        fprintf(stderr, "Car is NULL\n");
        return;
    }

    printf("Car is driving on %d-%d-%d\n", date.year, date.month, date.day);
}

void car_destroy(car_t *car)
{
    if (NULL == car)
    {
        fprintf(stderr, "Car is NULL\n");
        return;
    }
    free(car);
}

car_ops_t *get_car_ops(car_t *car)
{
    if (NULL == car)
    {
        fprintf(stderr, "Car is NULL\n");
        return NULL;
    }
    return &car->ops;
}