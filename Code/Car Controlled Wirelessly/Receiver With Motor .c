#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// NRF24 Setup
RF24 radio(7, 8); // CE, CSN on Nano
const byte address[6] = "00001";

// Motor Pins
const int motor1 = 2;
const int motor2 = 3;
const int motor3 = 5;
const int motor4 = 6;

// Received data structure
struct GyroData {
  float angleX;
  float angleY;
  float angleZ;
};
GyroData receivedData;

// Motor control parameters
const float deadZone = 5.0; // Degrees where motors won't respond
const int maxSpeed = 255;   // Maximum PWM value

void setup() {
  Serial.begin(9600);
  
  // Initialize motor pins
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(motor3, OUTPUT);
  pinMode(motor4, OUTPUT);
  
  // Initialize radio
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  Serial.println("Receiver ready");
}

void loop() {
  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));
    
    // Debug output
    Serial.print("Received - X: ");
    Serial.print(receivedData.angleX);
    Serial.print(" Y: ");
    Serial.print(receivedData.angleY);
    Serial.print(" Z: ");
    Serial.println(receivedData.angleZ);
    
    // Control motors based on tilt angles
    controlMotors(receivedData.angleX, receivedData.angleY);
  }
}

void controlMotors(float xAngle, float yAngle) {
  // Calculate motor speeds with dead zone
  int leftSpeed = 0, rightSpeed = 0;
  int frontSpeed = 0, backSpeed = 0;
  
  // Y-axis controls left/right motors (for forward/backward movement)
  if (abs(yAngle) > deadZone) {
    int ySpeed = map(abs(yAngle), deadZone, 90, 0, maxSpeed);
    ySpeed = constrain(ySpeed, 0, maxSpeed);
    
    if (yAngle > 0) {
      // Tilt forward
      leftSpeed = ySpeed;
      rightSpeed = ySpeed;
    } else {
      // Tilt backward
      leftSpeed = -ySpeed;
      rightSpeed = -ySpeed;
    }
  }
  
  // X-axis controls front/back motors (for turning)
  if (abs(xAngle) > deadZone) {
    int xSpeed = map(abs(xAngle), deadZone, 90, 0, maxSpeed);
    xSpeed = constrain(xSpeed, 0, maxSpeed);
    
    if (xAngle > 0) {
      // Tilt right
      frontSpeed = xSpeed;
      backSpeed = -xSpeed;
    } else {
      // Tilt left
      frontSpeed = -xSpeed;
      backSpeed = xSpeed;
    }
  }
  
  // Apply motor speeds (adjust based on your motor driver)
  analogWrite(motor1, abs(leftSpeed + frontSpeed));
  analogWrite(motor2, abs(rightSpeed + frontSpeed));
  analogWrite(motor3, abs(leftSpeed + backSpeed));
  analogWrite(motor4, abs(rightSpeed + backSpeed));
  
  // For bidirectional control, you would need additional direction pins
}
