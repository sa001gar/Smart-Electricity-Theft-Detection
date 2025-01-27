#include <WiFi.h>
#include <HTTPClient.h>

#define SENSOR_PIN 34
const float ACS712_SENSITIVITY = 0.185;
const int ADC_RESOLUTION = 4096;
const float VREF = 3.3;
const float NO_LOAD_VOLTAGE = VREF / 2;
const int NUM_SAMPLES = 100;

const char* ssid = "Virus Detected";
const char* password = "Sagar@react";

// Device metadata
const String POLE_ID = "POLE_001";
const String AREA_NAME = "Netaji Subhas Pally , Durgapur";
const String LOCATION = "23.484098878057395, 87.32089780205827";  // Latitude,Longitude

float calibrationOffset = 0;

void setup() {
  Serial.begin(250000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  // Calibration code (same as before)
  // ...
  Serial.println("Calibrating sensor...");
  float totalVoltage = 0;
  for (int i = 0; i < 1000; i++) {
    totalVoltage += analogRead(SENSOR_PIN) * (VREF / ADC_RESOLUTION);
    delay(1);
  }
  calibrationOffset = (totalVoltage / 1000.0) - NO_LOAD_VOLTAGE;
  Serial.println("Calibration complete!");
}

void loop() {
  float totalVoltage = 0;
  
  // Read sensor data (same as before)
  // ...
  

  // Sample sensor data
  for (int i = 0; i < NUM_SAMPLES; i++) {
    totalVoltage += analogRead(SENSOR_PIN) * (VREF / ADC_RESOLUTION);
    delay(10);
  }
  float averageVoltage = totalVoltage / NUM_SAMPLES;

  // Calculate current
  float current = (averageVoltage - NO_LOAD_VOLTAGE - calibrationOffset) / ACS712_SENSITIVITY;

  // Set current to 0 if it is less than 0.020
  if (current < 0.020) {
    current = 0.000;
  }
  // Calculate power
  float power = current * averageVoltage;
  

  send_to_server(current, averageVoltage, power);
  delay(3000);
}

void send_to_server(float current, float voltage, float power) {
  HTTPClient http;
  http.begin("http://192.168.1.36:5000/api/analyse");
  http.addHeader("Content-Type", "application/json");

  String payload = String("{") +
    "\"pole_id\":\"" + POLE_ID + "\"," +
    "\"area_name\":\"" + AREA_NAME + "\"," +
    "\"location\":\"" + LOCATION + "\"," +
    "\"current\":" + String(current) + "," +
    "\"voltage\":" + String(voltage) + "," +
    "\"power\":" + String(power) + "}";

  int httpCode = http.POST(payload);
  
  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println("Server Response: " + response);
  } else {
    Serial.println("HTTP Error: " + String(httpCode));
  }
  
  http.end();
}