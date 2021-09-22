#ifndef __imu_types_h__
#define __imu_types_h__

typedef struct imu_raw_data_struct
{
    int g_x;
    int g_y;
    int g_z;
    int r_x;
    int r_y;
    int r_z;
} imu_raw_data_t;

imu_raw_data_t screen_imu;

#endif