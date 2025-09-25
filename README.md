# Smart Fire Detection and Suppression System (Arduino + ESP32)

This Arduino sketch runs on an **ESP32** and integrates smoke and flame sensors with both **Firebase Realtime Database** and a **PHP backend** for real-time fire event logging.

## Features
- Detects **smoke** and **fire** using sensors.
- Sends all events (Fire, Smoke, Safe) to **Firebase**.
- Sends only Fire/Smoke alerts to a **PHP backend dashboard**.
- Prevents duplicate logs by only sending updates when the sensor state changes.

## Hardware Requirements
- ESP32
- Smoke Sensor (digital pin connected to GPIO 32)
- Flame Sensor (digital DO pin connected to GPIO 35)
- WiFi access

## Setup
1. Clone this repository
  
