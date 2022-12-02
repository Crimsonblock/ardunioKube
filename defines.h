#ifndef DEFINES_H_
#define DEFINES_H_

#define SerialSuccess 0x01
#define IMUSuccess 0x02
#define BLESuccess 0x04


#define ACCEL_DATA 0
#define GYRO_DATA 1
#define FRONT 2
#define BACK 3
#define LEFT 4
#define RIGHT 5
#define TOP 6
#define BOTTOM 7


#define DEVICE_NAME "Kube"
#define Accel
#define Gyro

#ifdef DEBUG
  #define PRINT(str) Serial.println(str)
#else
  #define PRINT(str)
#endif

#ifdef DEBUG_BLE
  #define BleDebug(str) Serial.println(str)
#else
  #define BleDebug(str)
#endif //DEBUG_BLE

#endif // DEFINES_H_ 