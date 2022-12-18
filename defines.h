#ifndef DEFINES_H_
#define DEFINES_H_
#include "defines.h"
#include <Arduino_LSM9DS1.h>


#define DEVICE_NAME "Kube"

#define SAMPLING_PERIOD 33
#define DEG_TO_RAD (PI/180)


#define IMU_TYPE 0
#define ROT_TYPE 1



int A[6] = { 2, 4, 6, 8, 10, 12 };
int B[6] = { 3, 5, 7, 9, 11, 13 };
int k = 0;
float C[6] = { 0, 0, 0, 0, 0, 0 };
int CSA[6] = { 0, 0, 0, 0, 0, 0 };
int CSB[6] = { 0, 0, 0, 0, 0, 0 };
int LSA[6] = { 0, 0, 0, 0, 0, 0 };
int LSB[6] = { 0, 0, 0, 0, 0, 0 };
char side;







#endif // DEFINES_H_ 