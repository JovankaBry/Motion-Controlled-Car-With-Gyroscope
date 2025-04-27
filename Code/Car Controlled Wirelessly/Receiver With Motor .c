#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";

// CONFIRMED CORRECT PINS AFTER TESTING
const int leftFront = 2;
const int leftBack = 3;
const int rightFront = 5;
const int rightBack = 6;

struct GyroData {
  float angleX;
  float angleY;
};

void setup() {
  Serial.begin(9600);
  pinMode(leftFront, OUTPUT);
  pinMode(leftBack, OUTPUT);
  pinMode(rightFront, OUTPUT);
  pinMode(rightBack, OUTPUT);
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  Serial.println("Receiver Ready!");
}

void loop() {
  if (radio.available()) {
    GyroData data;
    radio.read(&data, sizeof(data));
    
    // Reset motors
    digitalWrite(leftFront, LOW);
    digitalWrite(leftBack, LOW);
    digitalWrite(rightFront, LOW);
    digitalWrite(rightBack, LOW);
    
    // Movement logic
    if (data.angleY > 15) { // Forward
      digitalWrite(leftFront, HIGH);
      digitalWrite(rightFront, HIGH);
    } 
    else if (data.angleY < -15) { // Backward
      digitalWrite(leftBack, HIGH);
      digitalWrite(rightBack, HIGH);
    } 
    else if (data.angleX > 15) { // Right
      digitalWrite(leftFront, HIGH);
      digitalWrite(rightBack, HIGH);
    } 
    else if (data.angleX < -15) { // Left
      digitalWrite(leftBack, HIGH);
      digitalWrite(rightFront, HIGH);
    }
  }
}
