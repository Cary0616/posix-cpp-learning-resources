#include "car.h"
#include "bus.h"

int main()
{
    car_t *car = bus_create("1234567890", CAR_TYPE_SEDAN, CAR_COLOR_RED, (date_t){2020, 1, 1}, (date_t){2020, 1, 2}, 10);
    bus_ops_t *ops = bus_get_ops();
    ops->drive(car, (date_t){2020, 1, 2});
    ops->destroy(car);

    return 0;
}