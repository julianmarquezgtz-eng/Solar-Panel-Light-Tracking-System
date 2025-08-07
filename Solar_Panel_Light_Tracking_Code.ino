/*
"""
Solar Panel Light Tracking System
Author: Julian Marquez Gutierrez
Email: julianmarquezgtz@gmail.com
GitHub: https://github.com/julianmarquezgtz-eng
Date: 2025-03-30
"""
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <DHT.h>

// -------------------------- WiFi CONFIGURATION --------------------------
const char* ssid = "OPPO"; // WiFi network name
const char* password = "12345678";  // WiFi password

// ------------------------ THINGSPEAK CONFIGURATION ------------------------
const char* server = "http://api.thingspeak.com/update"; // ThingSpeak URL
String apiKey = "0LHTVNWF6GYVIPQX"; // ThingSpeak API key

// ---------------------------- PIN DEFINITIONS ----------------------------
const int servoPinH = 18; // Horizontal servo pin
const int servoPinV = 19; // Vertical servo pin
const int DHTPIN = 4;     // DHT11 temperature and humidity sensor pin

// Light sensor (LDR) pins
const int LDR_TOP_LEFT = 34;
const int LDR_TOP_RIGHT = 35;
const int LDR_BOTTOM_LEFT = 32;
const int LDR_BOTTOM_RIGHT = 33;

// ---------------------------- SERVO CONFIGURATION ----------------------------
Servo servoHorizontal, servoVertical; // Servo objects

int servoH = 45, servoHLimitHigh = 180, servoHLimitLow = 0; // Horizontal servo range
int servoV = 90, servoVLimitHigh = 145, servoVLimitLow = 35; // Vertical servo range

// ---------------------------- DHT SENSOR CONFIGURATION ----------------------------
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---------------------------- SMOOTHING VARIABLES ----------------------------
const int samples = 5; // Number of samples to smooth readings
int lt_hist[samples] = {0}, rt_hist[samples] = {0}, ld_hist[samples] = {0}, rd_hist[samples] = {0};

// ---------------------------- SMOOTHING FUNCTION ----------------------------
int smoothReading(int *history, int newValue) {
  int sum = 0;
  for (int i = samples - 1; i > 0; i--) {
    history[i] = history[i - 1];
    sum += history[i];
  }
  history[0] = newValue;
  sum += history[0];
  return sum / samples;
}

// ---------------------------- WIFI CONNECTION FUNCTION ----------------------------
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ---------------------------- SEND DATA TO THINGSPEAK FUNCTION ----------------------------
void sendToThingSpeak(float temp, float hum, int lt, int rt, int ld, int rd) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(temp) +
                 "&field2=" + String(hum) +
                 "&field3=" + String(lt) +
                 "&field4=" + String(rt) +
                 "&field5=" + String(ld) +
                 "&field6=" + String(rd);

    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      Serial.println("Data sent to ThingSpeak");
    } else {
      Serial.println("Error sending data");
    }
    http.end();
  } else {
    Serial.println("Error: WiFi not connected.");
  }
}

// ---------------------------- INITIAL SETUP ----------------------------
void setup() {
  Serial.begin(115200);
  connectWiFi();

  // Servo setup
  servoHorizontal.attach(servoPinH);
  servoVertical.attach(servoPinV);
  servoH = 45;
  servoV = 90;

  servoHorizontal.write(servoH);
  servoVertical.write(servoV);
  delay(3000);
  dht.begin(); // Start DHT11 sensor
}

// ---------------------------- SERVO ADJUSTMENT FUNCTION ----------------------------
void adjustServo(int &servoPos, int diff, int limitLow, int limitHigh, Servo &servo) {
  int step = map(abs(diff), 0, 1024, 1, 5);  // Adjust servo step size according to light difference
  if (diff > 0) servoPos = max(servoPos - step, limitLow); // Move towards light
  else servoPos = min(servoPos + step, limitHigh);
  servo.write(servoPos);
}

// ---------------------------- MAIN LOOP ----------------------------
void loop() {
  static unsigned long lastDHTRead = 0;
  static unsigned long lastThingSpeakSend = 0;
  const unsigned long DHT_INTERVAL = 2000;  // DHT11 reading interval
  const unsigned long THINGSPEAK_INTERVAL = 15000; // Data sending interval

  // Read and smooth LDR values
  int lt = smoothReading(lt_hist, analogRead(LDR_TOP_LEFT));
  int rt = smoothReading(rt_hist, analogRead(LDR_TOP_RIGHT));
  int ld = smoothReading(ld_hist, analogRead(LDR_BOTTOM_LEFT));
  int rd = smoothReading(rd_hist, analogRead(LDR_BOTTOM_RIGHT));

  // Print LDR values in columns
  Serial.printf("%-15s%-15s%-15s%-15s\n", "LDR_TOP_LEFT", "LDR_TOP_RIGHT", "LDR_BOTTOM_LEFT", "LDR_BOTTOM_RIGHT");
  Serial.printf("%-15d%-15d%-15d%-15d\n", lt, rt, ld, rd);

  // Calculate light differences
  int dvert = ((ld + rd) / 2) - ((lt + rt) / 2); // Vertical difference
  int dhoriz = ((rt + rd) / 2) - ((lt + ld) / 2); // Horizontal difference

  // Adjust servos based on light
  adjustServo(servoV, dvert, servoVLimitLow, servoVLimitHigh, servoVertical);
  adjustServo(servoH, dhoriz, servoHLimitLow, servoHLimitHigh, servoHorizontal);

  // Read temperature and humidity every 2 seconds
  if (millis() - lastDHTRead > DHT_INTERVAL) {
    lastDHTRead = millis();
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    Serial.print("Temp: "); Serial.print(temperature);
    Serial.print("°C  Hum: "); Serial.println(humidity);
  }

  // Send data to ThingSpeak every 15 seconds
  if (millis() - lastThingSpeakSend > THINGSPEAK_INTERVAL) {
    lastThingSpeakSend = millis();
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    sendToThingSpeak(temperature, humidity, lt, rt, ld, rd);
  }
}
