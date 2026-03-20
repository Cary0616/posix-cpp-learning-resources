#ifndef BUS_H
#define BUS_H

#include "car.h"

typedef struct bus bus_t;

/* 操作表（vtable）：在 header 中完整定义，供调用方通过指针调用方法 */
typedef struct bus_ops {
    void (*drive)(bus_t *bus, date_t date);
    void (*destroy)(bus_t *bus);
} bus_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

bus_t *bus_create(const char *licensePlate, enum car_type type, enum car_color color, date_t productionDate, date_t releaseDate, int seatCount);

/* 获取 bus 的操作表 */
bus_ops_t *bus_get_ops(void);

void bus_destroy(bus_t *bus);

void bus_drive(bus_t *bus, date_t date);

#ifdef __cplusplus
}
#endif

#endif /* BUS_H */