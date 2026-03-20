#ifndef CAR_H
#define CAR_H

#include "common.h"

typedef struct car car_t;

/* 操作表（vtable）：在 header 中完整定义，供调用方通过指针调用方法 */
typedef struct car_ops {
    void (*drive)(car_t *car, date_t date);
    void (*destroy)(car_t *car);
} car_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

car_t *car_create(const char *licensePlate, enum car_type type, enum car_color color, date_t productionDate, date_t releaseDate);

/* 获取 car 的操作表，用于 car->drive/destroy 的替代 */
car_ops_t *car_get_ops(void);

void car_destroy(car_t *car);

void car_drive(car_t *car, date_t date);

#ifdef __cplusplus
}
#endif

#endif /* CAR_H */