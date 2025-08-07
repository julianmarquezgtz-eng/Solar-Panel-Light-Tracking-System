# Solar Panel Light Tracking System

## Description

This code controls a solar tracking system for a solar panel using two servomotors (horizontal and vertical). The servos adjust the panel’s position based on light intensity readings from four LDR sensors. The system is implemented on an **ESP32** microcontroller and sends data to **ThingSpeak** via Wi-Fi.

## Features

- Dynamic real-time adjustment of the solar panel orientation.
- Servo control based on LDR sensor readings.
- Sends data (temperature, humidity, and LDR readings) to **ThingSpeak**.
- Servo positioning with configurable limits.

## Requirements

- ESP32 or compatible microcontroller.
- Servomotors (Azimuth and Elevation).
- LDR sensors (4 units).
- DHT11 sensor for temperature and humidity measurement.
- Wi-Fi access to send data to ThingSpeak.

## Installation

1. Clone or download this repository to your local machine.
2. Open the project in **Arduino IDE** or **PlatformIO**.
3. Install the following libraries:
   - `ESP32Servo` (for servo control).
   - `DHT` (for the DHT11 sensor).
   - `WiFi` and `HTTPClient` (for Wi-Fi and HTTP communication).
4. Configure your Wi-Fi credentials and **ThingSpeak** API key in the code.
5. Upload the code to the **ESP32** microcontroller.

## Usage

Once the code is uploaded to the ESP32, the system will start adjusting the solar panel position according to light readings and will send temperature, humidity, and LDR sensor data to ThingSpeak every 15 seconds.

## Author

Julián Márquez Gutiérrez
Email: julianmarquezgtz@gmail.com  
GitHub: julianmarquezgtz-eng(https://github.com/julianmarquezgtz-eng)
