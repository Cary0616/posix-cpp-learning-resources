#include <stdio.h>
#include <stdlib.h>

#include "car.h"
#include "bus.h"

int main()
{
    car_t *car = (car_t *)bus_create("1234567890", CAR_TYPE_BUS, CAR_COLOR_RED, (date_t){2020, 1, 1}, (date_t){2020, 1, 2}, 10);
    if (NULL == car)
    {
        fprintf(stderr, "Failed to create bus\n");
        return 1;
    }
    car_ops_t *ops = get_car_ops(car);
    if (NULL == ops)
    {
        fprintf(stderr, "Failed to get car ops\n");
        return 1;
    }
    ops->drive(car, (date_t){2020, 1, 2});
    ops->destroy(car);
    return 0;
}