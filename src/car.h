#ifndef CAR_H
#define CAR_H

#include "common.h"

typedef struct car car_t;
typedef struct car_ops car_ops_t;

struct car_ops
{
    void (*drive)(car_t *car, date_t date);
    void (*destroy)(car_t *car);
};

struct car
{
    const char *licensePlate;
    enum car_type type;
    enum car_color color;
    date_t productionDate;
    date_t releaseDate;
    car_ops_t ops;
};

#ifdef __cplusplus
extern "C"
{
#endif

    car_t *car_create(const char *licensePlate, enum car_type type, enum car_color color, date_t productionDate, date_t releaseDate);

    void car_init(car_t *car, const char *licensePlate, enum car_type type, enum car_color color, date_t productionDate, date_t releaseDate);

    void car_destroy(car_t *car);

    void car_drive(car_t *car, date_t date);

    car_ops_t *get_car_ops(car_t *car);
#ifdef __cplusplus
}
#endif

#endif /* CAR_H */