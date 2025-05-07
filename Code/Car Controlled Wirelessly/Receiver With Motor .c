#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN pins
const byte address[6] = "00001";

// Motor pins (using your exact setup)
const int motor1 = 2;
const int motor2 = 3;
const int motor3 = 5;
const int motor4 = 6;

struct GyroData {
  float angleX; // Left/Right tilt
  float angleY; // Forward/Backward tilt
};

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

  Serial.println("System Ready");
}

void stopMotors() {
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, LOW);
}

void moveForward() {
  // Your exact forward pattern
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, HIGH);
  digitalWrite(motor3, HIGH);
  digitalWrite(motor4, LOW);
  Serial.println("Moving Forward");
}

void moveBackward() {
  // Inverse of your forward pattern
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, HIGH);
  Serial.println("Moving Backward");
}

void turnRight() {
  // Left side forward, right side backward (sharp turn)
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, HIGH);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, HIGH);
  Serial.println("Turning Right");
}

void turnLeft() {
  // Right side forward, left side backward (sharp turn)
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, HIGH);
  digitalWrite(motor4, LOW);
  Serial.println("Turning Left");
}

void loop() {
  if (radio.available()) {
    GyroData data;
    radio.read(&data, sizeof(data));

    // Invert angleY to fix left/right confusion
    data.angleY = -data.angleY;

    stopMotors();

    // angleX = forward/backward tilt
    // angleY = left/right tilt
    if (data.angleX > 15) {
      moveForward();
    } 
    else if (data.angleX < -15) {
      moveBackward();
    } 
    else if (data.angleY > 15) {
      turnRight();
    } 
    else if (data.angleY < -15) {
      turnLeft();
    }
    else {
      Serial.println("Stopped");
    }
  }
}
