#include "defines.h"

typedef union {
  float v;
  char buffer[4];
} floaty_t;

int prev;


int A[6] = { 2, 4, 6, 8, 10, 12 };
int B[6] = { 3, 5, 7, 9, 11, 13 };
int k = 0;
int C[6] = { 0, 0, 0, 0, 0, 0 };
int CSA[6] = { 0, 0, 0, 0, 0, 0 };
int CSB[6] = { 0, 0, 0, 0, 0, 0 };
int LSA[6] = { 0, 0, 0, 0, 0, 0 };
int LSB[6] = { 0, 0, 0, 0, 0, 0 };
char side;

// Setup
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Inits IMU
  if (!IMU.begin()) {
    while (true) Serial.println("Error while init IMU");
  }

  for (int i = 0; i < 6; i++) {
    pinMode(A[i], INPUT_PULLUP);
    pinMode(B[i], INPUT_PULLUP);
    LSA[i] = digitalRead(A[i]);
    LSB[i] = digitalRead(B[i]);
  }

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
  RotaryEncoders();
}

void sendSerialImu(float aX, float aY, float aZ, float gX, float gY, float gZ) {
  char buffer[100];
  snprintf(buffer, 100, "i%f %f %f %f %f %f", aX, aY, aZ, gX, gY, gZ);
  Serial.println(buffer);
}

void sendSerialEncoders(int vals[]){
  Serial.println((String) "r" + vals[0] + " " + vals[1] + " " + vals[2] + " " + vals[3] + " " + vals[4] + " " + vals[5]);
}

void sendEncoders(int i){
  int vals[6] = {0, 0, 0, 0, 0, 0};
  vals[i] = C[i];
  sendSerialEncoders(vals);
}


void RotaryEncoders() {
  for (int i = 0; i < 6; i++) {
    CSA[i] = digitalRead(A[i]);
    CSB[i] = digitalRead(B[i]);
    if (CSA[i] != LSA[i]) {
      if (CSA[i] != CSB[i]) {
        C[i] = -1;
      } else {
        C[i] = 1;
      }
      sendEncoders(i);
    }

    LSA[i] = CSA[i];
    LSB[i] = CSB[i];
  }
}
