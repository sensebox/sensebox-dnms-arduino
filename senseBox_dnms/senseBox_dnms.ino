#include "SenseBoxAP.h"
//#include "SoundSensor.h"

#include <math.h>
#include <WiFi.h>
#include <PubSubClient.h>

//SoundSensor sensor;
const int OSM_MEASUREMENT_INTERVAL = 60000; // in milliseconds
SenseBoxAP configAP;
unsigned long lastOsmMeasurement = 0;

const int MQTT_MEASUREMENT_INTERVAL = 1000; // in milliseconds
const int MQTT_RECONNECT_INTERVAL = 60000; // in milliseconds
WiFiClient mqttWifiClient;
PubSubClient mqttClient(mqttWifiClient);
bool mqttEnabled = false;
String mqttTopic = "sensebox/loudness";
String mqttFieldName = "loudness";
unsigned long lastMqttReconnectAttempt = 0;
unsigned long lastMqttMeasurement = 0;

void setup() {
  Serial.begin(115200);
  configAP.begin("senseBox-Access-Point", "12345678");
  delay(1000);
  mqttAssureConnected();
  //sensor.begin();
}

void loop() {
  configAP.handle();
  float log_min = 0.0;
  float log_max = 0.0;
  // Linear average of measurements in linear scale
  float lin_avg = 0.0;
  float lin_val = 0.0;
  unsigned long n_measurement = 0;

  if (millis() - lastMqttMeasurement >= MQTT_MEASUREMENT_INTERVAL) {
    lastMqttMeasurement = millis();
    //sensor.update();
  
    //float val_avg = sensor.average();
    //float val_min = sensor.min();
    //float val_max = sensor.max();
      
    //sendToOpenSenseMap(val_min, val_max, val_avg);
    float val_avg = random(30, 90); // Placeholder for actual sensor reading
    float val_min = val_avg - random(5, 15);
    float val_max = val_avg + random(5, 15);
    sendToMQTT(val_avg);

    log_min = (n_measurement == 0) ? val_min : min(log_min, val_min);
    log_max = (n_measurement == 0) ? val_max : max(log_max, val_max);
    lin_val = pow(10, val_avg / 10);
    lin_avg = (n_measurement == 0) ? lin_val : lin_avg + lin_val;
    n_measurement++;
  }

  if (millis() - lastOsmMeasurement >= OSM_MEASUREMENT_INTERVAL) {
    lastOsmMeasurement = millis();
    if (n_measurement > 0) {
      float log_avg = 10 * log10(lin_avg / n_measurement);
      sendToOpenSenseMap(log_min, log_max, log_avg);

      n_measurement = 0;
    }
  }
}

static float roundToTwoDecimals(float v) {
  return roundf(v * 100.0f) / 100.0f;
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
    json += "{\"sensor\": \"" + sensorId1 + "\", \"value\": " + String(roundToTwoDecimals(value1)) + "},";
    json += "{\"sensor\": \"" + sensorId2 + "\", \"value\": " + String(roundToTwoDecimals(value2)) + "},";
    json += "{\"sensor\": \"" + sensorId3 + "\", \"value\": " + String(roundToTwoDecimals(value3)) + "}";
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

static String makeClientId() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  return "senseBox-" + mac;
}

void mqttAssureConnected() {
  unsigned long now = millis();
  if (now - lastMqttReconnectAttempt < MQTT_RECONNECT_INTERVAL && lastMqttReconnectAttempt > 0) {
    return;
  }
  lastMqttReconnectAttempt = now;
  Serial.println("Versuche MQTT Verbindung...");

  Preferences prefs;
  prefs.begin("mqtt_storage", true);
  // Retrieve the MQTT enable flag, ensuring the default value is used only if the preference is unset
  bool enabled = prefs.isKey("mqtt_enable") ? prefs.getBool("mqtt_enable") : false;
  String host = prefs.getString("mqtt_host", "");
  String portStr = prefs.getString("mqtt_port", "1883");
  uint16_t port = (uint16_t)portStr.toInt();
  if (port == 0) port = 1883;
  String user = prefs.getString("mqtt_user", "");
  String pass = prefs.getString("mqtt_pass", "");
  String topic = prefs.getString("mqtt_topic", mqttTopic);
  String field = prefs.getString("mqtt_field_name", mqttFieldName);
  prefs.end();

  mqttEnabled = enabled;
  mqttTopic = topic;
  mqttFieldName = field;

  if (!mqttEnabled) {
    Serial.println("MQTT ist deaktiviert");
    return;
  }
  Serial.println("MQTT ist aktiviert");
  Serial.println("MQTT Host: " + host + ":" + String(port));
  Serial.println("MQTT Topic: " + mqttTopic);
  Serial.println("MQTT Field Name: " + mqttFieldName);

  mqttClient.setServer(host.c_str(), port);

  String clientId = makeClientId();
  bool ok;
  if (user.length() == 0) {
    ok = mqttClient.connect(clientId.c_str());
  } else {
    ok = mqttClient.connect(clientId.c_str(), user.c_str(), pass.c_str());
  }
  Serial.println(ok ? "MQTT verbunden" : "MQTT Verbindungsfehler");
}

void sendToMQTT(float value) {
  if (!mqttEnabled) return;
  if (WiFi.status() != WL_CONNECTED) return;
  mqttAssureConnected();
  if (!mqttClient.connected()) return;

  String payload = "{\"" + mqttFieldName + "\":" + String(roundToTwoDecimals(value)) + "}";

  Serial.println("Publishing MQTT: " + mqttTopic + " -> " + payload);
  bool sent = mqttClient.publish(mqttTopic.c_str(), payload.c_str());
  if (!sent) {
    Serial.println("Failed to send MQTT message.");
  }
  mqttClient.loop();
}
