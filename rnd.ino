#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#define DEBUG
#define DEBUG_BLE
//Personal headers
#include "defines.h"
#include "UUIDs.h"


#define bluetoothLog


#define AVERAGE_SIZE 100

float reverseFloat(float a);





//Global Variables
#pragma region globalVarsSetup
char initState = 0;

typedef union {
  float v;
  char b[4];
  char buffer[4];
} floaty_t;











#ifdef bluetoothLog
/*
BLEFloatCharacteristic xAccelChar(X_ACCEL_CHARACTERISTIC, BLERead | BLENotify);
BLEFloatCharacteristic yAccelChar(Y_ACCEL_CHARACTERISTIC, BLERead | BLENotify);
BLEFloatCharacteristic zAccelChar(Z_ACCEL_CHARACTERISTIC, BLERead | BLENotify);

BLEFloatCharacteristic xGyroChar(X_GYRO_CHARACTERISTIC, BLERead | BLENotify);
BLEFloatCharacteristic yGyroChar(Y_GYRO_CHARACTERISTIC, BLERead | BLENotify);
BLEFloatCharacteristic zGyroChar(Z_GYRO_CHARACTERISTIC, BLERead | BLENotify);
*/

BLECharacteristic AccelChar(X_ACCEL_CHARACTERISTIC, BLERead | BLENotify, 12);
BLECharacteristic GyroChar(X_GYRO_CHARACTERISTIC, BLERead | BLENotify, 12);

BLEFloatCharacteristic frontChar(ENCODER_CHARACTERISTIC_FRONT, BLERead | BLENotify);
BLEFloatCharacteristic backChar(ENCODER_CHARACTERISTIC_BACK, BLERead | BLENotify);
BLEFloatCharacteristic leftChar(ENCODER_CHARACTERISTIC_LEFT, BLERead | BLENotify);
BLEFloatCharacteristic rightChar(ENCODER_CHARACTERISTIC_RIGHT, BLERead | BLENotify);
BLEFloatCharacteristic topChar(ENCODER_CHARACTERISTIC_TOP, BLERead | BLENotify);
BLEFloatCharacteristic bottomChar(ENCODER_CHARACTERISTIC_BOTTOM, BLERead | BLENotify);


// Rotary encoder stuff
int A[6] = { 2, 4, 6, 8, 10, 12 };
int B[6] = { 3, 5, 7, 9, 11, 13 };
int k = 0;
float C[6] = { 0, 0, 0, 0, 0, 0 };
int CSA[6] = { 0, 0, 0, 0, 0, 0 };
int CSB[6] = { 0, 0, 0, 0, 0, 0 };
int LSA[6] = { 0, 0, 0, 0, 0, 0 };
int LSB[6] = { 0, 0, 0, 0, 0, 0 };
char side;


#endif  //bluetoothLog
#pragma endregion




#pragma region timerFunctions


char flag = 0;

extern "C" {

  void TIMER4_IRQHandler_v() {
    if (NRF_TIMER4->EVENTS_COMPARE[0] == 1) {
      flag = 1;
      NRF_TIMER4->EVENTS_COMPARE[0] = 0;
    }
  }
}

#pragma endregion



int i;
int avg[AVERAGE_SIZE];




void setup() {
  Serial.begin(9600);
  initState = initState | SerialSuccess;

  Serial.println("Beginning  initialization");

  //Inits the Inertial Measurement Unit
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  //Sets the bit to succesfully set inertial measurement unit
  initState = initState | IMUSuccess;

  //Initializes the bluetooth
  #ifdef bluetoothLog
  Serial.println("Initializing Bluetooth LE");
  initBLE();
  #endif  //bluetoothLog
  Serial.println("BLE initialization finished");

  for (int i = 0; i < 6; i++) {
    pinMode(A[i], INPUT_PULLUP);
    pinMode(B[i], INPUT_PULLUP);
    LSA[i] = digitalRead(A[i]);
    LSB[i] = digitalRead(B[i]);
  }

  Serial.println("Configuring the timer");
  NRF_TIMER4->BITMODE = TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
  NRF_TIMER4->PRESCALER = 7 << TIMER_PRESCALER_PRESCALER_Pos;
  NRF_TIMER4->CC[0] = 4167;
  NRF_TIMER4->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
  NRF_TIMER4->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;

  NVIC_EnableIRQ(TIMER4_IRQn);
  NRF_TIMER4->TASKS_START = 1;

  i=0;
}


void loop() {
  if (flag) {
    // int g = millis();
    accelUpdate();
    gyroUpdate();
    flag = 0;

    // avg[i] = millis()-g;
    // int f = 0;
    // for(int g=0; g<AVERAGE_SIZE; g++){
    //   f+=avg[g];
    // }
    // Serial.print(f/AVERAGE_SIZE);
    // Serial.print(" - ");
    // Serial.println(avg[i]);
    // i = i>= AVERAGE_SIZE -1 ? 0 : i+1;  
  }
  RotaryEncoder();
}


#pragma region sensors
#ifdef Accel
void accelUpdate() {
  if ((initState & IMUSuccess)) {
    float x, y, z;
    IMU.readAcceleration(x, y, z);

    sendData(ACCEL_DATA, x, y, z);
  }
}
#endif  //Accel

#ifdef Gyro
void gyroUpdate() {
  if ((initState & IMUSuccess)) {
    float x, y, z;

    IMU.readGyroscope(x, y, z);

    int g = millis();
    sendData(GYRO_DATA, x, y, z);
  }
}
#endif  //Gyro


//Bluetooth-specific functions
#pragma region bt

#ifdef bluetoothLog
void initBLE() {
  if (!BLE.begin()) {
    Serial.println("Unable to initialize BLE module");
    while (1)
      ;
  }
  initState = initState | BLESuccess;
  BLE.setDeviceName(DEVICE_NAME);
  BLE.setLocalName(DEVICE_NAME);

  BLEService kubeService(KUBE_SERVICE);

  kubeService.addCharacteristic(AccelChar);
  kubeService.addCharacteristic(GyroChar);
  
  kubeService.addCharacteristic(frontChar);
  kubeService.addCharacteristic(backChar);
  kubeService.addCharacteristic(leftChar);
  kubeService.addCharacteristic(rightChar);
  kubeService.addCharacteristic(topChar);
  kubeService.addCharacteristic(bottomChar);

  BLE.addService(kubeService);

  BLE.setConnectable(true);
  if (!BLE.advertise()) {
    Serial.println("Unable to advertise");
    while (1)
      ;
  }
}

void bluetoothSend(char type, float x, float y, float z) {
  BLEDevice central = BLE.central();
  if (central) {
    floaty_t a;
    switch (type) {
      case ACCEL_DATA:
      /*
        xAccelChar.writeValue(reverseFloat(x));
        yAccelChar.writeValue(reverseFloat(y));
        zAccelChar.writeValue(reverseFloat(z));*/

        byte toTransmit[12];
        a.v = x;
        toTransmit[0] = a.buffer[0];
        toTransmit[1] = a.buffer[1];
        toTransmit[2] = a.buffer[2];
        toTransmit[3] = a.buffer[3];

        a.v = y;
        toTransmit[4] = a.buffer[0];
        toTransmit[5] = a.buffer[1];
        toTransmit[6] = a.buffer[2];
        toTransmit[7] = a.buffer[3];

        a.v = z;
        toTransmit[8] = a.buffer[0];
        toTransmit[9] = a.buffer[1];
        toTransmit[10] = a.buffer[2];
        toTransmit[11] = a.buffer[3];

        AccelChar.writeValue(toTransmit, 12);

        break;
      case GYRO_DATA:/*
        xGyroChar.writeValue(reverseFloat(x));
        yGyroChar.writeValue(reverseFloat(y));
        zGyroChar.writeValue(reverseFloat(z));*/
        a.v = x;
        toTransmit[0] = a.buffer[0];
        toTransmit[1] = a.buffer[1];
        toTransmit[2] = a.buffer[2];
        toTransmit[3] = a.buffer[3];

        a.v = y;
        toTransmit[4] = a.buffer[0];
        toTransmit[5] = a.buffer[1];
        toTransmit[6] = a.buffer[2];
        toTransmit[7] = a.buffer[3];

        a.v = z;
        toTransmit[8] = a.buffer[0];
        toTransmit[9] = a.buffer[1];
        toTransmit[10] = a.buffer[2];
        toTransmit[11] = a.buffer[3];
        GyroChar.writeValue(toTransmit, 12);
        
        break;
      case FRONT:
        frontChar.writeValue(reverseFloat(x));
        break;
      case BACK:
        backChar.writeValue(reverseFloat(x));
        break;
      case LEFT:
        leftChar.writeValue(reverseFloat(x));
        break;
      case RIGHT:
        rightChar.writeValue(reverseFloat(x));
        break;
      case TOP:
        topChar.writeValue(reverseFloat(x));
        break;
      case BOTTOM:
        bottomChar.writeValue(reverseFloat(x));
        break;
    }
  }
}
#endif  //bluetoothLog

#pragma endregion

#pragma region dataCom

void sendData(char type, float x, float y, float z) {

#ifdef serialLog
  serialData(type, x, y, z);
#endif  //serialLog

#ifdef bluetoothLog
  bluetoothSend(type, x, y, z);
#endif  //bluetoothLog
}

// If the serialLog is set
#ifdef serialLog

void serialData(char type, float x, float y, float z) {
  if (initState & SerialSuccess) {
    switch (type) {
      case ACCEL_DATA:
        Serial.println("Accelerometer Data: ");
        break;

      case GYRO_DATA:
        Serial.println("Gyro data: ");
        break;

      case MAGNETIC_DATA:
        Serial.println("Magnetic data: ");
        break;

      default:
        Serial.println("Unknown data: ");
        break;
    }
    Serial.println(x, 2);
    Serial.println("\t");
    Serial.println(y, 2);
    Serial.println("\t");
    Serial.println(z, 2);
  }
}

#endif  //serialLog
#pragma endregion


#pragma region utility

float reverseFloat(float a) {
  floaty_t b;
  b.v = a;
  char tempB;
  for (int i = 0; i < 2; i++) {
    tempB = b.b[i];
    b.b[i] = b.b[3 - i];
    b.b[3 - i] = tempB;
  }
  return b.v;
}

#pragma endregion  //utility

#pragma region RotaryEncoder

void RotaryEncoder() {
  for (int i = 0; i < 6; i++) {
    CSA[i] = digitalRead(A[i]);
    CSB[i] = digitalRead(B[i]);
    if (CSA[i] != LSA[i]) {
      if (CSA[i] != CSB[i]) {
        C[i]++;
      } else {
        C[i]--;
      }
      sendEncoders(i);
    }
    // if (CSB[i] != LSB[i]) {
    //   if (CSA[i] == CSB[i]) {
    //     C[i]++;
    //   } else {
    //     C[i]--;
    //   }
    //   sendEncoders(i);
    //}
    LSA[i] = CSA[i];
    LSB[i] = CSB[i];
  }
}

void sendEncoders(int i) {
  if (i == 0) {
    bluetoothSend(FRONT, C[i], 0, 0);
  }
  if (i == 1) {
    bluetoothSend(BACK, C[i], 0, 0);
  }
  if (i == 2) {
    bluetoothSend(LEFT, C[i], 0, 0);
  }
  if (i == 3) {
    bluetoothSend(RIGHT, C[i], 0, 0);
  }
  if (i == 4) {
    bluetoothSend(TOP, C[i], 0, 0);
  }
  if (i == 5) {
    bluetoothSend(BOTTOM, C[i], 0, 0);
  }
  C[i] = 0;
}
#pragma endregion  //RotaryEncoder