#ifndef BUS_H
#define BUS_H

#include "car.h"

typedef struct bus bus_t;

#ifdef __cplusplus
extern "C"
{
#endif

    bus_t *bus_create(const char *licensePlate, enum car_type type, enum car_color color, date_t productionDate, date_t releaseDate, int seatCount);

    void bus_destroy(car_t *car);

    void bus_drive(car_t *car, date_t date);

#ifdef __cplusplus
}
#endif

#endif /* BUS_H */