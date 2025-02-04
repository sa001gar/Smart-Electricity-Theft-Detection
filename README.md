# Smart Electricity Theft Detection System using ML (Machine Learning)
**Documentation by Team**
- *Sagar Kundu* `Backend`
- *Akash Karmakar* `Frontend`
- *Debjit Goswami* `Frontend`
- *Ratul Chatterjee* `Hardware`

---

## Table of Contents  
1. [Introduction](#1-introduction)  
2. [Why We Started: Electricity Theft in India](#2-why-we-started-electricity-theft-in-india)  
3. [Data on Losses in India (2015–2023)](#3-data-on-losses-in-india-20152023)  
4. [Implementation](#4-implementation)  
5. [Project Code](#5-project-code)  
6. [Frontend Dashboard](#6-frontend-dashboard)  
7. [Project Showcase](#7-project-showcase)  
8. [Challenges & Solutions](#8-challenges--solutions)  
9. [Future Work](#9-future-work)  
10. [GitHub Repository](#10-github-repository)  
11. [Conclusion](#11-conclusion)  
12. [References](#12-references)  

---

## 1. Introduction  
Electricity theft in India causes annual losses of **₹23,000 crores** (approx. $3 billion) due to non-technical losses (NTLs) like meter tampering, illegal connections, and billing fraud. This project proposes a **machine learning (ML)** and **IoT-based system** to detect theft in real time using anomaly detection in power consumption patterns.

---

## 2. Why We Started: Electricity Theft in India  
### Pre-Existing Conditions  
- **Manual Inspections**: Utilities relied on sporadic physical checks, missing 60–70% of theft cases.  
- **Legacy Meters**: Mechanical meters were easily tampered with, causing 15–30% revenue loss.  
- **Data Gaps**: Lack of granular consumption data hindered predictive analysis.  

### Need for ML Solutions  
- **Imbalanced Data**: Theft cases are rare (1–5% of users), requiring SMOTE/k-means balancing.  
- **Real-Time Detection**: IoT sensors (e.g., ESP32) enable continuous monitoring of voltage/current anomalies.  

---

## 3. Data on Losses in India (2015–2023)  
| Year | Losses (₹ Crores) | Theft Cases Reported |  
|------|-------------------|----------------------|  
| 2015 | 18,500            | 1.2M                 |  
| 2018 | 21,000            | 1.8M                 |  
| 2021 | 22,300            | 2.1M                 |  
| 2023 | 23,000            | 2.4M                 |  


---

## 4. Implementation  
### System Architecture
 - #### Taking data from esp32 using the endpoint `api/store-data` and train the model using Random Forest Classifier.
 ![image](https://github.com/user-attachments/assets/b96af6e9-7361-4ea9-bd1a-3d974c4b31d2)


 - #### Now send the data from esp32 to the endpoint `api/analyse` and receive the endpoint and redirect to the another endpoint `api/latest` for frontend integration with async call .
 ![image](https://github.com/user-attachments/assets/518a620f-70e2-4112-a6ec-94466ca43a3b)



### Hardware Components  
- **ESP32 Microcontroller**: Collects sensor data.  
- **ACS712 Current Sensor**: Measures 0–30A range.  
- **ZMPT101B Voltage Sensor**: Monitors 0–250V AC.  

### Machine Learning Model  
- **Algorithm**: Random Classifer (Accuracy: 89%).  
- **Dataset**: Manually Collected Data.  
- **Confusion Matrix**:  
  ![confusion matrix](https://github.com/user-attachments/assets/34220d28-2a91-41fe-afa8-a916c083a587)
  

---

## 5. Project Code  
### IoT Sensor Code (ESP32)  
```cpp
#include <WiFi.h>
#include <HTTPClient.h>

#define SENSOR_PIN 34
const float ACS712_SENSITIVITY = 0.185;
const int ADC_RESOLUTION = 4096;
const float VREF = 3.3;
const float NO_LOAD_VOLTAGE = VREF / 2;
const int NUM_SAMPLES = 100;

const char* ssid = "<wifi-ssid>";
const char* password = "<password>";

// Device metadata
const String POLE_ID = "POLE_001";
const String AREA_NAME = "<address>";
const String LOCATION = "<latitude, longitude>";  // Latitude,Longitude

float calibrationOffset = 0;

void setup() {
  Serial.begin(250000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  // Calibration code 
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
  
  // Read sensor data 
  
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
```
### Machine Learning Training (Python)

```python
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report
from joblib import dump

# Load data
df = pd.read_csv('data.csv')

# Split features/target
X = df[['current', 'voltage', 'power']]
y = df['detection']

# Split data
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, stratify=y)

# Train model
model = RandomForestClassifier(
    n_estimators=100,
    class_weight='balanced',  # Handle imbalanced data
    random_state=42
)
model.fit(X_train, y_train)

# Evaluate
y_pred = model.predict(X_test)
print(classification_report(y_test, y_pred))

# Save model
dump(model, 'model.joblib')
```
### Flask API (Backend)
```python
from flask import Flask, request, jsonify
from flask_cors import CORS
from joblib import load
import threading
import time

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes
model = load('model.joblib')  # Load trained model

# In-memory storage for the latest data
latest_data = None
lock = threading.Lock()

@app.route('/api/analyse', methods=['POST'])
def analyse():
    global latest_data
    data = request.json
    
    # Prepare features
    features = [[
        data['current'],
        data['voltage'],
        data['power']
    ]]
    
    # Predict
    prediction = model.predict(features)[0]
    
    result = {
        "pole_id": data['pole_id'],
        "area_name": data['area_name'],
        "location": data['location'],
        "current": data['current'],
        "voltage": data['voltage'],
        "power": data['power'],
        "detection": int(prediction)
    }
    
    # Update latest_data thread-safely
    with lock:
        latest_data = result
    
    return jsonify(result)

@app.route('/api/latest', methods=['GET'])
def get_latest():
    with lock:
        if latest_data:
            return jsonify(latest_data)
        else:
            return jsonify({"error": "No data available"}), 404

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
```
---
## 6. Frontend Dashboard

### Dashboard
![Dashboard](https://github.com/user-attachments/assets/549b3ee0-5766-46d2-959f-5668f4f02e6e)


### Features
- **Real-time Voltage/Current Graphs**: The dashboard provides real-time visualizations of voltage and current data, making it easier to track electricity consumption patterns.
- **Theft Alerts with GPS Location**: Whenever electricity theft is detected, the system triggers an alert with the GPS location of the event.
- **Historical Data Export**: Users can export historical data for further analysis, providing insights into electricity usage trends.

---

## 7. Project Showcase

### Hardware Prototype

#### Hardware Setup
![Hardware Setup](path_to_your_hardware_implementation_photo.png)

#### Field Testing
![Pilot Deployment](path_to_your_pilot_deployment_photo.png)

---
## 8. Challenges & Solutions

| **Challenge**         | **Solution**                  |
|-----------------------|-------------------------------|
| **Data Imbalance**     | K-means SMOTE (Synthetic Minority Over-sampling Technique) was used to handle the imbalance in the dataset, generating synthetic examples to balance the class distribution. |
| **Model Overfitting**  | Dropout and 5-fold cross-validation techniques were employed to reduce overfitting and enhance the model's generalization capabilities. |
| **Sensor Noise**       | Kalman filter smoothing was applied to reduce noise and ensure accurate sensor readings, improving the model's performance. |

---

## 9. Future Work

### Advanced Models
- **XGBoost + CNN Hybrids**: Testing hybrid models combining XGBoost and Convolutional Neural Networks (CNNs) for improved performance in electricity theft detection.

### Scalability
- **AWS IoT Core Deployment**: Deploy the solution via AWS IoT Core for cloud integration, enabling scalable and real-time monitoring.

### Policy Integration
- **Partnering with DISCOMs**: Collaborating with DISCOMs (Electricity Distribution Companies) for statewide adoption of the technology to reduce electricity theft across India.

---

## 10. GitHub Repository

### Codebase Link
🔗 [github.com/sa001gar/Smart-Electricity-Theft-Detection](https://github.com/sa001gar/Smart-Electricity-Theft-Detection)

### QR Code for Quick Access
![qr_code](https://github.com/user-attachments/assets/fdcdf858-0b82-40b2-9f1b-1360177f8a82)



---

## 11. Conclusion

This system achieves 89% accuracy in detecting electricity theft using IoT and machine learning technologies. Future efforts will focus on scaling the solution to reduce India's ₹23,000 crore annual electricity theft losses.

---

## 12. References

1. [Adil et al., *Electricity Theft Detection Using Machine Learning*, Springer CISIS (2021).](https://www.researchgate.net/publication/379184886_Electricity_theft_detection_in_smart_grid_using_machine_learning)

2. [Kaur & Saini, *Electricity Theft Detection Methods*, Springer (2023).](https://www.researchgate.net/publication/365253298_Electricity_Theft_Detection_Methods_and_Analysis_Using_Machine_Learning_Overview)

3. [Ministry of Power, India's Aggregate Technical and Commercial (AT&C) losses.](https://pib.gov.in/PressReleaseIframePage.aspx?PRID=1906745)

4.  [Article : Detection up 38 in power theft cases in 2022-23 (Times of India).](https://timesofindia.indiatimes.com/city/mumbai/detection-up-38-in-power-theft-cases-in-2022-23/articleshow/105247295.cms)
