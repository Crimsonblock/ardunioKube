#include "defines.h"

typedef union {
  float v;
  char buffer[4];
} floaty_t;

int prev;


// Setup
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Inits IMU
  if (!IMU.begin()) {
    while (true) Serial.println("Error while init IMU");
  }

  // Inits BLE
  prev = 0;
}


void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - prev >= SAMPLING_PERIOD) {
    prev = millis();
    float aX, aY, aZ, gX, gY, gZ;
    IMU.readGyroscope(gX, gY, gZ);
    IMU.readAcceleration(aX, aY, aZ);

    gX = gX * DEG_TO_RAD;
    gY = gY * DEG_TO_RAD;
    gZ = gZ * DEG_TO_RAD;

    sendSerialImu(aX, aY, aZ, gX, gY, gZ);
  }
}

void sendSerialImu(float aX, float aY, float aZ, float gX, float gY, float gZ) {
  char buffer[100];
  snprintf(buffer, 100, "i%f %f %f %f %f %f", aX, aY, aZ, gX, gY, gZ);
  Serial.println(buffer);
}

