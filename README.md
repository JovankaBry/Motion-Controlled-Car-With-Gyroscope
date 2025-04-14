# Embedded-System-Projekt
A motion-controlled car project using an MPU6050 gyroscope connected via wire to an Arduino, where hand tilts control the car’s direction.

How It Works:
- An MPU6050 sensor is mounted on a glove or handheld controller.
- The sensor detects the tilt of the hand (pitch and roll).
- These tilt values are sent to the Arduino over a wired connection (I2C).
- Based on the detected angle, the Arduino processes the data and controls the car's direction:
  - Tilt left → car turns left
  - Tilt right → car turns right
  - Tilt forward → car moves forward
  - Tilt backward → car reverses

Components Used:
-Arduino Nano
- MPU6050 Gyroscope & Accelerometer Module
- Motor Driver (e.g., L298N or similar)
- DC motors with wheels
- Battery 9V
- Jumper wires
- Chassis or 3D printed body

Features:
-Real-time motion control using hand gestures
-Wired I2C communication for low latency
-Simple and intuitive control system
-Expandable for wireless control (e.g., using NRF24L01 or Bluetooth)
