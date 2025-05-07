#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <U8g2lib.h>

// NRF24 Setup
RF24 radio(7, 8); // CE, CSN for Uno/Nano
const byte address[6] = "00001";

// MPU6050 Setup
MPU6050 mpu(Wire);
unsigned long timer = 0;

// OLED Setup
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Button Setup
const int buttonPin = 3; // Button to GND with internal pull-up
unsigned long buttonPressTime = 0;
bool resetTriggered = false;

// Data structure
struct GyroData {
  float angleX;
  float angleY;
  float angleZ;
};

void softReset() {
  asm volatile ("jmp 0"); // Software reset
}

void checkReset() {
  static bool buttonActive = false;
  
  if (digitalRead(buttonPin) == LOW) { // Button pressed
    if (!buttonActive) {
      buttonActive = true;
      buttonPressTime = millis();
    }
    
    // Check if held for 5 seconds
    if (millis() - buttonPressTime > 5000 && !resetTriggered) {
      u8g2.firstPage();
      do {
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(20, 30, "Resetting...");
      } while (u8g2.nextPage());
      delay(500);
      resetTriggered = true;
      softReset();
    }
  } else {
    buttonActive = false;
  }
}

void Introscreen() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 15, "Embedded System");
    u8g2.drawStr(40, 30, "Projekt");
    u8g2.drawStr(35, 45, "Sose 2025");
  } while (u8g2.nextPage());
  delay(2000);
}

void showWelcomeScreen() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 15, "Herzlich Wilkommen");
    u8g2.drawStr(23, 30, "Bitte Druecken");
    u8g2.drawStr(30, 45, "zum Starten");
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize OLED
  u8g2.begin();

  // Initialize MPU6050
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status != 0) { 
    // Show error if MPU fails to initialize
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(15, 30, "MPU6050 Error!");
      u8g2.drawStr(10, 45, "Check Connection");
    } while (u8g2.nextPage());
    delay(1000);
  }

  Introscreen();

  // Wait for button press to continue
  bool waitingForButton = true;
  while(waitingForButton) {
    checkReset(); // Check for reset during wait
    showWelcomeScreen();
    
    if(digitalRead(buttonPin) == LOW) {
      delay(50); // Debounce
      if(digitalRead(buttonPin) == LOW) {
        waitingForButton = false;
      }
    }
    mpu.update(); // Keep MPU updated
  }

  // Calibration
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 30, "Calculating MPU6050");
    u8g2.drawStr(20, 45, "DO NOT MOVE!!!");
  } while (u8g2.nextPage());
  
  delay(1000);
  mpu.calcOffsets();
  Serial.println("Done!\n");

  // Ready message
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(20, 30, "Device Is Ready");
  } while (u8g2.nextPage());
  delay(2000);

  // Radio initialization
  if (!radio.begin()) {
    Serial.println("Radio initialization failed!");
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(15, 30, "Radio Error!");
      u8g2.drawStr(10, 45, "Check NRF24");
    } while (u8g2.nextPage());
    while(1); // Halt on radio failure
  }
  Serial.println("Radio initialized successfully");
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  checkReset(); // Continuous reset check
  
  mpu.update();

  if ((millis() - timer) > 20) { // 50Hz update rate
    GyroData data = {mpu.getAngleX(), mpu.getAngleY(), mpu.getAngleZ()};
    bool success = radio.write(&data, sizeof(data));

    // OLED Display Update
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.setCursor(0, 12); 
      u8g2.print("X: "); u8g2.print(data.angleX, 1);
      u8g2.setCursor(0, 24); 
      u8g2.print("Y: "); u8g2.print(data.angleY, 1);
      u8g2.setCursor(0, 36); 
      u8g2.print("Z: "); u8g2.print(data.angleZ, 1);
      u8g2.setCursor(0, 48); 
      u8g2.print("TX: "); u8g2.print(success ? "OK" : "Fail");
    } while (u8g2.nextPage());

    // Serial Monitor Output
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
