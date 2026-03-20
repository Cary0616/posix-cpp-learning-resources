#ifndef COMMON_H
#define COMMON_H

enum car_type
{
    CAR_TYPE_SEDAN,
    CAR_TYPE_SUV,
    CAR_TYPE_MPV,
    CAR_TYPE_COUPE,
    CAR_TYPE_SPORTS_CAR,
    CAR_TYPE_BUS,
    CAR_TYPE_OTHER,
};

enum car_color
{
    CAR_COLOR_RED,
    CAR_COLOR_BLUE,
    CAR_COLOR_GREEN,
    CAR_COLOR_YELLOW,
    CAR_COLOR_BLACK,
    CAR_COLOR_WHITE,
    CAR_COLOR_OTHER,
};

typedef struct date
{
    int year;
    int month;
    int day;
} date_t;

#endif /* COMMON_H */