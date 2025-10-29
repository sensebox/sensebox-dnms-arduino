#include "SenseBoxAP.h"
#include "DNMSI2C.h"

DNMSI2C sensor;
const int MEASUREMENT_INTERVAL = 60;
SenseBoxAP configAP;
unsigned long lastMeasurement = 0;

void setup() {
  Serial.begin(115200);
  configAP.begin("senseBox-Access-Point", "12345678");
  delay(1000);
  sensor.begin();
  lastMeasurement = millis() - MEASUREMENT_INTERVAL * 1000;
}

void loop() {
  configAP.handle();
  
  if (millis() - lastMeasurement >= MEASUREMENT_INTERVAL * 1000) {
    lastMeasurement = millis();
    sensor.update();
  
    int avg = sensor.average();
    int min = sensor.min();
    int max = sensor.max();
      
    sendToOpenSenseMap(min, max, avg);
  }


}

void sendToOpenSenseMap(float value1, float value2, float value3) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClientSecure client;
    client.setInsecure();
    
    Preferences prefs;
    prefs.begin("ids_storage", true);
    String senseboxId = prefs.getString("sensebox_id", "5f1c265de45a4f001cd2f000");
    String sensorId1 = prefs.getString("loudness_min", "default_min_id");
    String sensorId2 = prefs.getString("loudness_max", "default_max_id");
    String sensorId3 = prefs.getString("loudness_avg", "default_avg_id");
    prefs.end();
    
    String url = "https://ingress.opensensemap.org/boxes/" + senseboxId + "/data";
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    
    String json = "[";
    json += "{\"sensor\": \"" + sensorId1 + "\", \"value\": " + String(value1) + "},";
    json += "{\"sensor\": \"" + sensorId2 + "\", \"value\": " + String(value2) + "},";
    json += "{\"sensor\": \"" + sensorId3 + "\", \"value\": " + String(value3) + "}";
    json += "]";
    
    Serial.println("URL: " + url);
    Serial.println("JSON: " + json);
    
    int httpResponseCode = http.POST(json);
    Serial.println("HTTP Antwortcode: " + String(httpResponseCode));
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Antwort: " + response);
      Serial.println("Daten erfolgreich gesendet");
    } else {
      Serial.println("Fehler beim Senden: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("Keine WLAN-Verbindung");
  }
}