#define SENSOR_PIN 34  // ADC pin connected to ACS712 OUT

const float ACS712_SENSITIVITY = 0.185; // Sensitivity for ACS712-05B (185 mV/A)
const int ADC_RESOLUTION = 4096;       // ESP32 ADC resolution
const float VREF = 3.3;                // Reference voltage for ADC
const float NO_LOAD_VOLTAGE = VREF / 2; // Voltage at 0A (2.5V for ACS712)
const int NUM_SAMPLES = 100;           // Number of samples for averaging

float calibrationOffset = 0; // Calibration offset

void setup() {
  Serial.begin(115200); // Start serial communication
  delay(1000);

  Serial.println("Calibrating sensor...");
  float totalVoltage = 0;
  
  // Perform calibration
  for (int i = 0; i < 1000; i++) {
    totalVoltage += analogRead(SENSOR_PIN) * (VREF / ADC_RESOLUTION);
    delay(1);
  }
  calibrationOffset = (totalVoltage / 1000.0) - NO_LOAD_VOLTAGE;
  Serial.println("Calibration complete!");
}

void loop() {
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

  // Display results
  Serial.print("Voltage: ");
  Serial.print(averageVoltage, 3);
  Serial.print(" V | Current: ");
  Serial.print(current, 3);
  Serial.println(" A");

  delay(300); // Update every second
}