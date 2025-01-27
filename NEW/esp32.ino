#include <WiFi.h>
#include <HTTPClient.h>

#define SENSOR_PIN 34
const float ACS712_SENSITIVITY = 0.185;
const int ADC_RESOLUTION = 4096;
const float VREF = 3.3;
const float NO_LOAD_VOLTAGE = VREF / 2;
const int NUM_SAMPLES = 100;

// WiFi credentials
const char* ssid = "Virus Detected";
const char* password = "Sagar@react";

float calibrationOffset = 0;
bool detection_flag = 0; // Manually set this via Serial Monitor

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  // Calibration code remains same as your existing code
  // ... (your existing calibration code)

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
  // Current measurement code remains same
  // ... (your existing measurement code)
  float totalVoltage = 0;

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

  // Send data to backend
  send_to_server(current, averageVoltage, power);
  
  delay(3000); // Send every 3 seconds
}

void send_to_server(float current, float voltage, float power) {
  HTTPClient http;
  http.begin("http://192.168.1.36:5000/api/store-data");
  http.addHeader("Content-Type", "application/json");
  
  // Manually set detection_flag via Serial input
  if (Serial.available()) {
    detection_flag = Serial.parseInt();
  }

  String payload = String("{") +
                   "\"current\":" + String(current) + "," +
                   "\"voltage\":" + String(voltage) + "," +
                   "\"power\":" + String(power) + "," +
                   "\"detection\":" + String(detection_flag) +
                   "}";

  int httpCode = http.POST(payload);
  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Data sent successfully");
  }
  http.end();
}