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

void moveForwardRight() {
  // Nyalakan roda depan kiri dulu
  digitalWrite(motor1, LOW);   // Depan kiri maju
  digitalWrite(motor2, HIGH);  // Belakang kiri mundur (untuk maju)
  
  // Delay sebentar sebelum nyalakan roda kanan
  delay(100); // Delay 100ms

  digitalWrite(motor3, HIGH);  // Depan kanan maju
  digitalWrite(motor4, LOW);   // Belakang kanan mundur (untuk maju)

  Serial.println("Moving Forward Right (dengan delay motor3)");
}

void moveForwardLeft() {
  // Nyalakan sisi kanan dulu
  digitalWrite(motor3, HIGH);  // Depan kanan maju
  digitalWrite(motor4, LOW);   // Belakang kanan bantu maju

  delay(100); // Delay sebelum sisi kiri nyala

  digitalWrite(motor1, LOW);   // Depan kiri maju
  digitalWrite(motor2, HIGH);  // Belakang kiri bantu maju

  Serial.println("Moving Forward Left");
}

void moveBackward() {
  // Inverse of your forward pattern
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, HIGH);
  Serial.println("Moving Backward");
}

void moveBackwardRight() {
  // Nyalakan sisi kiri dulu (mundur)
  digitalWrite(motor1, HIGH);  // Depan kiri mundur
  digitalWrite(motor2, LOW);   // Belakang kiri bantu mundur

  delay(100); // Delay sebelum sisi kanan nyala

  digitalWrite(motor3, LOW);   // Depan kanan mundur
  digitalWrite(motor4, HIGH);  // Belakang kanan bantu mundur

  Serial.println("Moving Backward Right");
}

void moveBackwardLeft() {
  // Nyalakan sisi kanan dulu (mundur)
  digitalWrite(motor3, LOW);   // Depan kanan mundur
  digitalWrite(motor4, HIGH);  // Belakang kanan bantu mundur

  delay(100); // Delay sebelum sisi kiri nyala

  digitalWrite(motor1, HIGH);  // Depan kiri mundur
  digitalWrite(motor2, LOW);   // Belakang kiri bantu mundur

  Serial.println("Moving Backward Left");
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
    if (data.angleX > 15 && data.angleY > 15) {
      moveForwardRight();
    }
    else if (data.angleX > 15 && data.angleY < -15) {
      moveForwardLeft();
    }
    else if (data.angleX < -15 && data.angleY > 15) {
      moveBackwardRight();
    }
    else if (data.angleX < -15 && data.angleY < -15) {
      moveBackwardLeft();
    }
    else if (data.angleX > 15) {
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
      stopMotors();
      Serial.println("Stopped");
    }
  }
}
