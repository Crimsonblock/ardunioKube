#include "defines.h"

typedef union {
  float v;
  char buffer[4];
} floaty_t;

void sendBleImu(float aX, float aY, float aZ, float gX, float gY, float gZ);

int prev;


// Setup
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Inits IMU
  if(!IMU.begin()){
    while(true) Serial.println("Error while init IMU");
  }

  // Inits BLE
  initBLE();
  prev = 0;
}


void loop() {
  // put your main code here, to run repeatedly:
  if(millis()-prev>=SAMPLING_PERIOD){
    prev = millis();

    BLEDevice central = BLE.central();
    if(central){
      float aX, aY, aZ, gX, gY, gZ;
      IMU.readGyroscope(gX, gY, gZ);
      IMU.readAcceleration(aX, aY, aZ);

      gX = gX * DEG_TO_RAD;
      gY = gY * DEG_TO_RAD;
      gZ = gZ * DEG_TO_RAD;

      sendBleImu(aX, aY, aZ, gX, gY, gZ);

      Serial.println((String) "Communication time: " + (millis() - prev) );
    }
  }
}


void sendBleImu( float aX, float aY, float aZ, float gX, float gY, float gZ ){
  floaty_t a;
  byte buffer[24];
  a.v = aX;

  // Acceleromeer
  for(int i=0; i<4 ; i++){
    buffer[i] = a.buffer[3-i];
  }
  a.v = aY;
  for(int i=0; i<4;i++){
    buffer[4+i] = a.buffer[3-i];
  }
  a.v = aZ;
  for(int i=0; i<4; i++){
    buffer[8+i] = a.buffer[3-i];
  }

  //Gyroscope
  a.v = gX;
  for(int i=0; i<4; i++){
    buffer[12+i] = a.buffer[3-i];
  }
  a.v = gY;
  for(int i=0; i<4; i++){
    buffer[16+i] = a.buffer[3-i];
  }
  a.v = gZ;
  for(int i=0; i<4; i++){
    buffer[20+i] = a.buffer[3-i];
  }

  IMUChar.writeValue(buffer, 24);

}



void initBLE(){
  if (!BLE.begin()) {
    Serial.println("Unable to initialize BLE module");
    while (1)
      ;
  }


  BLE.setDeviceName(DEVICE_NAME);
  BLE.setLocalName(DEVICE_NAME);

  BLEService kubeService(KUBE_SERVICE);

  kubeService.addCharacteristic(IMUChar);

  BLE.addService(kubeService);

  BLE.setConnectable(true);
  BLE.advertise();
}
