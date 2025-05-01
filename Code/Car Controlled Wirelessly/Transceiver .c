#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "Wire.h"
#include <MPU6050_light.h>

// NRF24 Setup
RF24 radio(7, 8); // CE, CSN on Uno
const byte address[6] = "00001";

// MPU6050 Setup
MPU6050 mpu(Wire);
unsigned long timer = 0;

// Data structure to send
struct GyroData {
  float angleX;
  float angleY;
  float angleZ;
};

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Initialize MPU6050
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status != 0) { } // Stop if connection fails
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // Calibrate gyro and accelerometer
  Serial.println("Done!\n");

  // Initialize radio
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  mpu.update();
  
  if((millis() - timer) > 20) { // Send data every 20ms
    GyroData data;
    data.angleX = mpu.getAngleX();
    data.angleY = mpu.getAngleY();
    data.angleZ = mpu.getAngleZ();
    
    // Send data
    bool success = radio.write(&data, sizeof(data));
    
    // Debug output
    Serial.print("X: ");
    Serial.print(data.angleX);
    Serial.print(" Y: ");
    Serial.print(data.angleY);
    Serial.print(" Z: ");
    Serial.print(data.angleZ);
    Serial.print(" Transmission: ");
    Serial.println(success ? "OK" : "Failed");
    
    timer = millis();
  }
}
