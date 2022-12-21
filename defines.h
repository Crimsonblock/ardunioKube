#ifndef DEFINES_H_
#define DEFINES_H_
#include "defines.h"
#include <Arduino_LSM9DS1.h>


#define DEVICE_NAME "Kube"

#define SAMPLING_PERIOD 33
#define DEG_TO_RAD (PI/180)


#define IMU_TYPE 0
#define ROT_TYPE 1

#define ROT_FRONT 0
#define ROT_BACK 1
#define ROT_LEFT 2
#define ROT_RIGHT 3
#define ROT_TOP 4
#define ROT_BOTTOM 5




void RotaryEncoders();
void sendEncoders(int i);




#endif // DEFINES_H_ 