#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";
const char* trainingUrl = "http://<backend_ip>:8000/collect-data/";
const char* detectionUrl = "http://<backend_ip>:8000/detect-theft/";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Example sensor data
    String payload = R"(
      {
        "pole_id": "P1",
        "location": "Area 1",
        "current": 10.0,
        "voltage": 220.0,
        "power": 2200.0
      }
    )";

    // Send to the detection endpoint
    http.begin(detectionUrl);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  delay(10000); // Wait 10 seconds before sending the next request
}
