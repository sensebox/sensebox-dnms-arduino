#ifndef SENSEBOXAP_H
#define SENSEBOXAP_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

class SenseBoxAP {
public:
    SenseBoxAP();
    void begin();
    void begin(const char* apSsid, const char* apPassword);
    void handle();
    
private:
    AsyncWebServer _server;
    Preferences _preferences;
    WiFiClientSecure _wifiClientSecure;
    HTTPClient* _httpClient;
    
    String _ssidString;
    String _passwordString;
    String _apSsid;
    String _apPassword;
    String _senseboxId;
    String _sensorId1;
    String _sensorId2;
    String _sensorId3;
    
    void _readWiFi();
    void _readIds();
    void _updateWiFiConnection(String newSSID, String newPassword);
    void _updateIds(String senseboxId, String sensorId1, String sensorId2, String sensorId3);
    String _buildHTMLString();
    String _buildSaveString();
    String _buildIdsSaveString();
    void _setupRoutes();
};

#endif