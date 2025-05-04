#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// Use software I2C (bit-banging) with default SCL and SDA pins
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  u8g2.begin();  // Initialize the display
}

void loop() {
  u8g2.clearBuffer();          // Clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a nice font
  u8g2.drawStr(0, 20, "Hello, world!"); // Draw text at x=0, y=20
  u8g2.sendBuffer();           // Transfer internal memory to the display
  delay(2000);                 // Wait for 2 seconds
}
