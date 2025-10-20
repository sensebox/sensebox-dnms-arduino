#include "SenseBoxAP.h"

SenseBoxAP::SenseBoxAP() : _server(80) {
    _wifiClientSecure.setInsecure();
    _httpClient = new HTTPClient();
    _httpClient->begin(_wifiClientSecure, "ingress.opensensemap.org", 443, "/boxes/...");
    _apSsid = "senseBox-AccessPoint";
    _apPassword = "123456789";
}

void SenseBoxAP::begin() {
    WiFi.softAP(_apSsid.c_str(), _apPassword.c_str());
    _readWiFi();
    _readIds();
    _readMqtt();
    _setupRoutes();
    _server.begin();
    
    if (_ssidString != "N/A" && _passwordString != "N/A") {
        WiFi.begin(_ssidString.c_str(), _passwordString.c_str());
    }
}

void SenseBoxAP::begin(const char* apSsid, const char* apPassword) {
    _apSsid = String(apSsid);
    _apPassword = String(apPassword);
    
    WiFi.softAP(_apSsid.c_str(), _apPassword.c_str());
    _readWiFi();
    _readIds();
    _readMqtt();
    _setupRoutes();
    _server.begin();
    
    if (_ssidString != "N/A" && _passwordString != "N/A") {
        WiFi.begin(_ssidString.c_str(), _passwordString.c_str());
    }
}

void SenseBoxAP::handle() {}

void SenseBoxAP::_readWiFi() {
    _preferences.begin("wifi_storage", true);
    _ssidString = _preferences.getString("ssid", "N/A");
    _passwordString = _preferences.getString("pw", "N/A");
    _preferences.end();
}

void SenseBoxAP::_readIds() {
    _preferences.begin("ids_storage", true);
    _senseboxId = _preferences.getString("sensebox_id", "5f1c265de45a4f001cd2f000");
    _sensorId1 = _preferences.getString("loudness_min", "default_min_id");
    _sensorId2 = _preferences.getString("loudness_max", "default_max_id");
    _sensorId3 = _preferences.getString("loudness_avg", "default_avg_id");
    _preferences.end();
}

void SenseBoxAP::_readMqtt() {
    _preferences.begin("mqtt_storage", true);
    _mqttEnable = _preferences.getBool("mqtt_enable", false);
    _mqttHost = _preferences.getString("mqtt_host", "broker.hivemq.com");
    _mqttPort = _preferences.getString("mqtt_port", "1883");
    _mqttUser = _preferences.getString("mqtt_user", "");
    _mqttPass = _preferences.getString("mqtt_pass", "");
    _mqttTopic = _preferences.getString("mqtt_topic", "sensebox/data");
    _mqttFieldName = _preferences.getString("mqtt_field_name", "loudness");
    _preferences.end();
}

void SenseBoxAP::_updateWiFiConnection(String newSSID, String newPassword) {
    _preferences.begin("wifi_storage", false);
    _preferences.putString("ssid", newSSID);
    _preferences.putString("pw", newPassword);
    _preferences.end();
}

void SenseBoxAP::_updateIds(String senseboxId, String sensorId1, String sensorId2, String sensorId3) {
    _preferences.begin("ids_storage", false);
    _preferences.putString("sensebox_id", senseboxId);
    _preferences.putString("loudness_min", sensorId1);
    _preferences.putString("loudness_max", sensorId2);
    _preferences.putString("loudness_avg", sensorId3);
    _preferences.end();
}

void SenseBoxAP::_updateMqtt(bool mqttEnable, String mqttHost, String mqttPort, String mqttUser, String mqttPass, String mqttTopic, String mqttFieldName) {
    _preferences.begin("mqtt_storage", false);
    _preferences.putBool("mqtt_enable", mqttEnable);
    _preferences.putString("mqtt_host", mqttHost);
    _preferences.putString("mqtt_port", mqttPort);
    _preferences.putString("mqtt_user", mqttUser);
    _preferences.putString("mqtt_pass", mqttPass);
    _preferences.putString("mqtt_topic", mqttTopic);
    _preferences.putString("mqtt_field_name", mqttFieldName);
    _preferences.end();
}

String SenseBoxAP::_buildHTMLString() {
    String html = "<!DOCTYPE html><html lang='de'><head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>senseBox Konfiguration</title>";
    html += "<style>";
    html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 0; background-color: #f8f9fa; color: #333; }";
    html += ".container { max-width: 800px; margin: 20px auto; padding: 20px; background: white; box-shadow: 0 0 10px rgba(0,0,0,0.1); border-radius: 8px; }";
    html += "header { text-align: center; padding: 20px 0; border-bottom: 2px solid #4CAF50; margin-bottom: 20px; }";
    html += ".logo { display: inline-block; }";
    html += ".logo img { height: 60px; }";
    html += "h1 { color: #4CAF50; font-size: 28px; margin-bottom: 5px; }";
    html += "h2 { color: #2196F3; font-size: 22px; margin-top: 30px; border-bottom: 1px solid #eee; padding-bottom: 5px; }";
    html += "form { margin: 20px 0; }";
    html += "label { display: block; margin: 10px 0 5px; font-weight: bold; color: #555; }";
    html += "input[type='text'], input[type='password'] { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 4px; font-size: 16px; }";
    html += "input[type='submit'] { background-color: #4CAF50; color: white; padding: 12px 20px; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; margin-top: 10px; transition: background-color 0.3s; }";
    html += "input[type='submit']:hover { background-color: #45a049; }";
    html += ".status { margin-top: 30px; padding: 15px; background-color: #f1f8e9; border-left: 4px solid #4CAF50; border-radius: 4px; }";
    html += ".status-item { margin: 8px 0; }";
    html += ".status-label { font-weight: bold; color: #333; }";
    html += ".status-value { color: #555; }";
    html += ".connected { color: #4CAF50; font-weight: bold; }";
    html += ".disconnected { color: #f44336; font-weight: bold; }";
    html += "@media (max-width: 600px) { .container { margin: 10px; padding: 15px; } }";
    html += "</style>";
    html += "</head><body>";
    
    html += "<div class='container'>";
    html += "<header>";
    html += "<div class='logo'>";
    html += "<img src='data:image/svg+xml;base64,PHN2ZyBpZD0iRWJlbmVfMSIgZGF0YS1uYW1lPSJFYmVuZSAxIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNDIuMzQgOTEuNSI+PGRlZnM+PHN0eWxlPi5jbHMtMXtmaWxsOiM1MGFlNDg7fS5jbHMtMntmaWxsOiM2MzYzNjI7fS5jbHMtMiwuY2xzLTMsLmNscy00e3N0cm9rZTojZmZmO3N0cm9rZS1taXRlcmxpbWl0OjEwO3N0cm9rZS13aWR0aDo0LjA5cHg7fS5jbHMtMywuY2xzLTV7ZmlsbDojMDBiNGU2O30uY2xzLTQsLmNscy02e2ZpbGw6I2ZmZWMwMDt9PC9zdHlsZT48L2RlZnM+PHRpdGxlPkZseWVyX0hhY2thdGhvbiBTdGVwNjwvdGl0bGU+PHBhdGggY2xhc3M9ImNscy0xIiBkPSJNMjIuNDgsNDMuMTRoLTNsLS41My0yLjgyYTcuODQsNy44NCwwLDAsMC0yLjktLjQ5Yy0yLDAtMi45LDEuMTUtMi45LDIuMzdzLjQ5LDEuOCwyLDIuMjlsMy4xMS45NGMzLjIzLDEsNC45NSwyLjYyLDQuOTUsNS40OFMyMSw1Ni40NiwxNS42NSw1Ni40NmExNy42MiwxNy42MiwwLDAsMS02Ljc5LTEuMzFMOC41LDQ5Ljc2aDNMMTIuMzgsNTNhNy40Nyw3LjQ3LDAsMCwwLDMuMjcuNjVjMS44OCwwLDMuMTktLjgyLDMuMTktMi4zMywwLTEuMTgtLjc0LTEuODgtMi4xNy0yLjM3bC0zLjI3LTFjLTIuODYtLjktNC41NC0yLjU4LTQuNTQtNS4zMlMxMC42MywzNywxNiwzN2ExOS4yNiwxOS4yNiwwLDAsMSw2LjUsMS4xNFoiLz48cGF0aCBjbGFzcz0iY2xzLTEiIGQ9Ik00MC42Myw1MS4yN2wxLjI3LDNhMTMuMDYsMTMuMDYsMCwwLDEtNy4wNywyLjE3Yy02LjI5LDAtOC44My00LjA5LTguODMtOS43N0MyNiw0MS4zNCwyOC43NCwzNywzNC40MiwzN2M1LjQ0LDAsNy42NCwzLjkyLDcuNjQsNy45M0ExNC4xMSwxNC4xMSwwLDAsMSw0MS43Myw0OEgzMC4wOGMwLDMuMTUsMS41MSw1LjA3LDQuNzgsNS4wN0ExMC42MiwxMC42MiwwLDAsMCw0MC42Myw1MS4yN1pNMzguMjYsNDVhNCw0LDAsMCwwLDAtLjY1YzAtMi42Mi0xLjU5LTQuMTMtMy45Mi00LjEzLTMsMC00LjA5LDIuMTMtNC4yNSw0Ljc4WiIvPjxwYXRoIGNsYXNzPSJjbHMtMSIgZD0iTTQ0LjkyLDUyLjY2SDQ3LjVWNDAuNzJINDVWMzcuMzdoNS44OVYzOS41QTguMzUsOC4zNSwwLDAsMSw1Ny4zMSwzN2M0LjQ2LDAsNi4yNSwyLjQ5LDYuMjUsNy4yM3Y4LjQ2SDY2LjFWNTZINTYuODZWNTIuNjZoMi45VjQ0LjU3YzAtMi40OS0uNjEtNC0zLjc2LTRhOC4yNiw4LjI2LDAsMCwwLTQuNzQsMS42OFY1Mi42NmgyLjlWNTZINDQuOTJaIi8+PHBhdGggY2xhc3M9ImNscy0xIiBkPSJNODIuMTYsNDMuMTRoLTNsLS41My0yLjgyYTcuODQsNy44NCwwLDAsMC0yLjktLjQ5Yy0yLDAtMi45LDEuMTUtMi45LDIuMzdzLjQ5LDEuOCwyLDIuMjlsMy4xMS45NGMzLjIzLDEsNC45NSwyLjYyLDQuOTUsNS40OHMtMi4xMyw1LjU2LTcuNDgsNS41NmExNy42MiwxNy42MiwwLDAsMS02Ljc5LTEuMzFsLS4zNy01LjM5aDNMNzIuMDcsNTNhNy40OCw3LjQ4LDAsMCwwLDMuMjcuNjVjMS44OCwwLDMuMTktLjgyLDMuMTktMi4zMywwLTEuMTgtLjc0LTEuODgtMi4xNy0yLjM3bC0zLjI3LTFjLTIuODYtLjktNC41NC0yLjU4LTQuNTQtNS4zMlM3MC4zMSwzNyw3NS42NiwzN2ExOS4yNiwxOS4yNiwwLDAsMSw2LjUsMS4xNFoiLz48cGF0aCBjbGFzcz0iY2xzLTEiIGQ9Ik0xMDAuMzEsNTEuMjdsMS4yNywzYTEzLjA2LDEzLjA2LDAsMCwxLTcuMDcsMi4xN2MtNi4yOSwwLTguODMtNC4wOS04LjgzLTkuNzcsMC01LjM1LDIuNzQtOS43Myw4LjQyLTkuNzMsNS40NCwwLDcuNjQsMy45Miw3LjY0LDcuOTNhMTQuMTEsMTQuMTEsMCwwLDEtLjMzLDMuMDdIODkuNzdjMCwzLjE1LDEuNTEsNS4wNyw0Ljc4LDUuMDdBMTAuNjIsMTAuNjIsMCwwLDAsMTAwLjMxLDUxLjI3Wk05Ny45NCw0NWE0LDQsMCwwLDAsMC0uNjVjMC0yLjYyLTEuNTktNC4xMy0zLjkyLTQuMTMtMywwLTQuMDksMi4xMy00LjI1LDQuNzhaIi8+PHBhdGggY2xhc3M9ImNscy0xIiBkPSJNMTA0Ljg1LDUyLjYyaDIuOTRWMzEuNzdoLTIuOTRWMjguMzhoMTEuOWM1LjY0LDAsOC4xMywyLjgyLDguMTMsNy4yLDAsMy4wNy0xLjcyLDUuMzUtNCw1Ljg5YTYuODQsNi44NCwwLDAsMSw1LjI3LDYuNzhjMCw1Ljg5LTQuMTcsNy43Ny05LjQ0LDcuNzdoLTExLjlaTTExNiw0MC4wN2MzLjIzLDAsNC43NC0xLjU1LDQuNzQtNC4yOSwwLTIuNDEtMS4xOS00LTQuNjItNEgxMTEuOHY4LjNabS40OSwxMi41NWM0LjA5LDAsNS42LTEuNzIsNS42LTQuNTQsMC0yLjU4LTEuMzEtNC42Ni01LjY0LTQuNjZIMTExLjh2OS4yWiIvPjxwYXRoIGNsYXNzPSJjbHMtMSIgZD0iTTE0Ni4xNCw0Ni42NWMwLDUuNzYtMi4zMyw5LjgxLTguNzksOS44MS02LjE3LDAtOC41OC00LTguNTgtOS43M1MxMzEuMDksMzcsMTM3LjUxLDM3LDE0Ni4xNCw0MSwxNDYuMTQsNDYuNjVabS00LC4wOGMwLTQuMTMtMS4xLTYuMjEtNC42Ni02LjIxcy00LjY2LDIuMDgtNC42Niw2LjEzLDEuMTgsNi4yNSw0LjcsNi4yNVMxNDIuMDksNTAuNzQsMTQyLjA5LDQ2LjczWiIvPjxwYXRoIGNsYXNzPSJjbHMtMSIgZD0iTTE1NC42NCw1Mi42NkgxNThWNTZoLTkuMzJWNTIuNjZIMTUxbDUtNi4yNS00LjM4LTUuNjhoLTIuOVYzNy4zN2g3LjczdjMuMTlsMy4yNyw0LjI5LDMuMTEtNC4xM2gtMy4zMVYzNy4zN2g5LjI4djMuMzVoLTIuNTNsLTQuNjYsNS45Myw0Ljk1LDZoMi42NlY1NmgtNy42NFY1Mi44MmwtMy42LTQuN1oiLz48cGF0aCBjbGFzcz0iY2xzLTIiIGQ9Ik0yMTguMzksNjEuMjFhMTYuODQsMTYuODQsMCwwLDAtMjMuMjYsMGwxMS42MSwxMS42MVoiLz48cGF0aCBjbGFzcz0iY2xzLTMiIGQ9Ik0yMDUuNjQsMzUuNjVhMS45MiwxLjkyLDAsMSwxLTIuMi0xLjU4LDEuOTEsMS45MSwwLDAsMSwyLjIsMS41OCIvPjxwYXRoIGNsYXNzPSJjbHMtMyIgZD0iTTIwNS42NCw0My43M2ExLjkyLDEuOTIsMCwxLDEtMi4yLTEuNTksMS45MSwxLjkxLDAsMCwxLDIuMiwxLjU5Ii8+PHBhdGggY2xhc3M9ImNscy0zIiBkPSJNMjExLjg4LDUwLjU3YTEuOTIsMS45MiwwLDEsMS0yLjItMS41OSwxLjkyLDEuOTIsMCwwLDEsMi4yLDEuNTkiLz48cGF0aCBjbGFzcz0iY2xzLTMiIGQ9Ik0yMTMuOCwzOS45YTEuOTIsMS45MiwwLDEsMS0yLjItMS41OSwxLjkyLDEuOTIsMCwwLDEsMi4yLDEuNTkiLz48cGF0aCBjbGFzcz0iY2xzLTQiIGQ9Ik0xOTcuMDYsMjguNDJsOS42Ny05LjY3LDkuNzYsOS43NmExNi40NSwxNi40NSwwLDAsMS0xOS40My0uMDhaIi8+PHBvbHlnb24gY2xhc3M9ImNscy0zIiBwb2ludHM9IjIyNC4xOCA1NS40NiAyMjAuNjcgNTEuOTUgMjI0LjMgNDguMzIgMjE3LjA0IDQ4LjMyIDIxNC41MyA0NS44MSAyMTYuOTcgNDMuMzYgMjI0LjM3IDQzLjM2IDIyMC42NyAzOS42NiAyMjQuMTggMzYuMTUgMjMzLjc5IDQ1Ljc2IDIyNC4xOCA1NS40NiIvPjxwYXRoIGNsYXNzPSJjbHMtMSIgZD0iTTIxOC4zOSw2MS4yMWExNi44NCwxNi44NCwwLDAsMC0yMy4yNiwwbDExLjYxLDExLjYxWiIvPjxwYXRoIGNsYXNzPSJjbHMtNSIgZD0iTTIwNS42NCwzNS42NWExLjkyLDEuOTIsMCwxLDEtMi4yLTEuNTgsMS45MSwxLjkxLDAsMCwxLDIuMiwxLjU4Ii8+PHBhdGggY2xhc3M9ImNscy01IiBkPSJNMjA1LjY0LDQzLjczYTEuOTIsMS45MiwwLDEsMS0yLjItMS41OSwxLjkxLDEuOTEsMCwwLDEsMi4yLDEuNTkiLz48cGF0aCBjbGFzcz0iY2xzLTUiIGQ9Ik0yMTEuODgsNTAuNTdhMS45MiwxLjkyLDAsMSwxLTIuMi0xLjU5LDEuOTIsMS45MiwwLDAsMSwyLjIsMS41OSIvPjxwYXRoIGNsYXNzPSJjbHMtNSIgZD0iTTIxMy44LDM5LjlhMS45MiwxLjkyLDAsMSwxLTIuMi0xLjU5LDEuOTIsMS45MiwwLDAsMSwyLjIsMS41OSIvPjxwYXRoIGNsYXNzPSJjbHMtNiIgZD0iTTE5Ny4wNiwyOC40Mmw5LjY3LTkuNjcsOS43Niw5Ljc2YTE2LjQ1LDE2LjQ1LDAsMCwxLTE5LjQzLS4wOFoiLz48cG9seWdvbiBjbGFzcz0iY2xzLTUiIHBvaW50cz0iMjI0LjE4IDU1LjQ2IDIyMC42NyA1MS45NSAyMjQuMyA0OC4zMiAyMTcuMDQgNDguMzIgMjE0LjUzIDQ1LjgxIDIxNi45NyA0My4zNiAyMjQuMzcgNDMuMzYgMjIwLjY3IDM5LjY2IDIyNC4xOCAzNi4xNSAyMzMuNzkgNDUuNzYgMjI0LjE4IDU1LjQ2Ii8+PHBhdGggY2xhc3M9ImNscy01IiBkPSJNMTgyLjcyLDQyLjc2aDBhNC4zMSw0LjMxLDAsMCwxLDAsNi4wOWwyLjQsMi40YTcuNyw3LjcsMCwwLDAsMC0xMC44OGgwWiIvPjxwYXRoIGNsYXNzPSJjbHMtNSIgZD0iTTE4NywzOC41MmExMC4zMiwxMC4zMiwwLDAsMSwwLDE0LjU4bDIuNCwyLjRhMTMuNywxMy43LDAsMCwwLDAtMTkuMzdaIi8+PHBhdGggY2xhc3M9ImNscy01IiBkPSJNMTkxLjIxLDM0LjI3aDBhMTYuMywxNi4zLDAsMCwxLDAsMjMuMDdsMi40LDIuNGExOS42OCwxOS42OCwwLDAsMCwwLTI3Ljg2aDBaIi8+PHBhdGggY2xhc3M9ImNscy01IiBkPSJNMTc5LjY3LDQ1LjgxbDEuNTIsMS41MmEyLjEsMi4xLDAsMCwwLDAtM1oiLz48L3N2Zz4=' alt='senseBox Logo' />";
    html += "</div>";
    html += "</header>";
    
    html += "<h1>senseBox Konfiguration</h1>";
    
    html += "<form action='/save' method='post'>";
    html += "<h2>WLAN Verbindung</h2>";
    html += "<label for='ssid'>SSID:</label>";
    html += "<input type='text' id='ssid' name='ssid' value='" + _ssidString + "' required>";
    html += "<label for='password'>Passwort:</label>";
    html += "<input type='password' id='password' name='password' value='" + _passwordString + "' required>";
    
    html += "<h2>openSenseMap Einstellungen</h2>";
    html += "<label for='sensebox_id'>senseBox ID:</label>";
    html += "<input type='text' id='sensebox_id' name='sensebox_id' value='" + _senseboxId + "' required>";
    html += "<label for='sensor_id_1'>Lautstärke (Min):</label>";
    html += "<input type='text' id='sensor_id_1' name='sensor_id_1' value='" + _sensorId1 + "' required>";
    html += "<label for='sensor_id_2'>Lautstärke (Max):</label>";
    html += "<input type='text' id='sensor_id_2' name='sensor_id_2' value='" + _sensorId2 + "' required>";
    html += "<label for='sensor_id_3'>Lautstärke (Avg):</label>";
    html += "<input type='text' id='sensor_id_3' name='sensor_id_3' value='" + _sensorId3 + "' required>";
    
    html += "<h2>MQTT Einstellungen</h2>";

    html += "<label for='mqtt_enable'>MQTT Aktivieren:</label>";
    html += "<input type='checkbox' id='mqtt_enable' name='mqtt_enable' " + String(_mqttEnable ? "checked" : "") + ">";
    html += "<label for='mqtt_host'>MQTT Host Name:</label>";
    html += "<input type='text' id='mqtt_host' name='mqtt_host' value='" + _mqttHost + "'>";
    html += "<label for='mqtt_port'>MQTT Port:</label>";
    html += "<input type='text' id='mqtt_port' name='mqtt_port' value='" + _mqttPort + "'>";
    html += "<label for='mqtt_user'>MQTT Benutzername:</label>";
    html += "<input type='text' id='mqtt_user' name='mqtt_user' value='" + _mqttUser + "'>";
    html += "<label for='mqtt_pass'>MQTT Passwort:</label>";
    html += "<input type='password' id='mqtt_pass' name='mqtt_pass' value='" + _mqttPass + "'>";
    html += "<label for='mqtt_topic'>MQTT Topic:</label>";
    html += "<input type='text' id='mqtt_topic' name='mqtt_topic' value='" + _mqttTopic + "'>";
    html += "<label for='mqtt_field_name'>MQTT Feldname für Messwert:</label>";
    html += "<input type='text' id='mqtt_field_name' name='mqtt_field_name' value='" + _mqttFieldName + "'>";
    
    html += "<input type='submit' value='Speichern & Neustarten'>";
    html += "</form>";
    
    html += "<div class='status'>";
    html += "<h2>Status</h2>";
    html += "<div class='status-item'><span class='status-label'>WLAN Status:</span> <span class='status-value ";
    if (WiFi.status() == WL_CONNECTED) {
        html += "connected'>Verbunden</span></div>";
    } else {
        html += "disconnected'>Nicht verbunden</span></div>";
    }
    html += "<div class='status-item'><span class='status-label'>Aktuelle SSID:</span> <span class='status-value'>" + WiFi.SSID() + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>Signalstärke:</span> <span class='status-value'>" + String(WiFi.RSSI()) + " dBm</span></div>";
    html += "<div class='status-item'><span class='status-label'>AP SSID:</span> <span class='status-value'>" + _apSsid + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>senseBox ID:</span> <span class='status-value'>" + _senseboxId + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>Sensor ID 1 (Min):</span> <span class='status-value'>" + _sensorId1 + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>Sensor ID 2 (Max):</span> <span class='status-value'>" + _sensorId2 + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>Sensor ID 3 (Avg):</span> <span class='status-value'>" + _sensorId3 + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>MQTT Aktiviert:</span> <span class='status-value'>" + String(_mqttEnable ? "Ja" : "Nein") + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>MQTT Host:</span> <span class='status-value'>" + _mqttHost + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>MQTT Port:</span> <span class='status-value'>" + _mqttPort + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>MQTT Benutzername:</span> <span class='status-value'>" + _mqttUser + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>MQTT Passwort:</span> <span class='status-value'>" + _mqttPass + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>MQTT Topic</span> <span class='status-value'>" + _mqttTopic + "</span></div>";
    html += "<div class='status-item'><span class='status-label'>MQTT Feldname:</span> <span class='status-value'>" + _mqttFieldName + "</span></div>";
    html += "</div>";
    
    html += "</div>";
    html += "</body></html>";
    
    return html;
}

String SenseBoxAP::_buildSaveString() {
    String html = "<!DOCTYPE html><html lang='de'><head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Erfolg</title>";
    html += "<style>";
    html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 0; background-color: #f8f9fa; color: #333; }";
    html += ".container { max-width: 600px; margin: 50px auto; padding: 30px; background: white; box-shadow: 0 0 10px rgba(0,0,0,0.1); border-radius: 8px; text-align: center; }";
    html += ".logo { display: block; margin: 0 auto 20px; }";
    html += ".logo img { height: 60px; }";
    html += "h1 { color: #4CAF50; font-size: 28px; margin-bottom: 10px; }";
    html += "p { font-size: 18px; margin: 15px 0; }";
    html += ".countdown { font-size: 24px; font-weight: bold; color: #2196F3; margin: 20px 0; }";
    html += "button { background-color: #4CAF50; color: white; padding: 12px 20px; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; margin: 10px; transition: background-color 0.3s; }";
    html += "button:hover { background-color: #45a049; }";
    html += "</style>";
    html += "</head><body>";
    html += "<div class='container'>";
    html += "<div class='logo'>";
    html += "<img src='data:image/svg+xml;base64,PHN2ZyBpZD0iRWJlbmVfMSIgZGF0YS1uYW1lPSJFYmVuZSAxIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNDIuMzQgOTEuNSI+PGRlZnM+PHN0eWxlPi5jbHMtMXtmaWxsOiM1MGFlNDg7fS5jbHMtMntmaWxsOiM2MzYzNjI7fS5jbHMtMiwuY2xzLTMsLmNscy00e3N0cm9rZTojZmZmO3N0cm9rZS1taXRlcmxpbWl0OjEwO3N0cm9rZS13aWR0aDo0LjA5cHg7fS5jbHMtMywuY2xzLTV7ZmlsbDojMDBiNGU2O30uY2xzLTQsLmNscy02e2ZpbGw6I2ZmZWMwMDt9PC9zdHlsZT48L2RlZnM+PHRpdGxlPkZseWVyX0hhY2thdGhvbiBTdGVwNjwvdGl0bGU+PHBhdGggY2xhc3M9ImNscy0xIiBkPSJNMjIuNDgsNDMuMTRoLTNsLS41My0yLjgyYTcuODQsNy44NCwwLDAsMC0yLjktLjQ5Yy0yLDAtMi45LDEuMTUtMi45LDIuMzdzLjQ5LDEuOCwyLDIuMjlsMy4xMS45NGMzLjIzLDEsNC45NSwyLjYyLDQuOTUsNS40OFMyMSw1Ni40NiwxNS42NSw1Ni40NmExNy42MiwxNy42MiwwLDAsMS02Ljc5LTEuMzFMOC41LDQ5Ljc2aDNMMTIuMzgsNTNhNy40Nyw3LjQ3LDAsMCwwLDMuMjcuNjVjMS44OCwwLDMuMTktLjgyLDMuMTktMi4zMywwLTEuMTgtLjc0LTEuODgtMi4xNy0yLjM3bC0zLjI3LTFjLTIuODYtLjktNC41NC0yLjU4LTQuNTQtNS4zMlMxMC42MywzNywxNiwzN2ExOS4yNiwxOS4yNiwwLDAsMSw2LjUsMS4xNFoiLz48cGF0aCBjbGFzcz0iY2xzLTEiIGQ9Ik00MC42Myw1MS4yN2wxLjI3LDNhMTMuMDYsMTMuMDYsMCwwLDEtNy4wNywyLjE3Yy02LjI5LDAtOC44My00LjA5LTguODMtOS43N0MyNiw0MS4zNCwyOC43NCwzNywzNC40MiwzN2M1LjQ0LDAsNy42NCwzLjkyLDcuNjQsNy45M0ExNC4xMSwxNC4xMSwwLDAsMSw0MS43Myw0OEgzMC4wOGMwLDMuMTUsMS41MSw1LjA3LDQuNzgsNS4wN0ExMC42MiwxMC42MiwwLDAsMCw0MC42Myw1MS4yN1pNMzguMjYsNDVhNCw0LDAsMCwwLDAtLjY1YzAtMi42Mi0xLjU5LTQuMTMtMy45Mi00LjEzLTMsMC00LjA5LDIuMTMtNC4yNSw0Ljc4WiIvPjxwYXRoIGNsYXNzPSJjbHMtMSIgZD0iTTQ0LjkyLDUyLjY2SDQ3LjVWNDAuNzJINDVWMzcuMzdoNS44OVYzOS41QTguMzUsOC4zNSwwLDAsMSw1Ny4zMSwzN2M0LjQ2LDAsNi4yNSwyLjQ5LDYuMjUsNy4yM3Y4LjQ2SDY2LjFWNTZINTYuODZWNTIuNjZoMi45VjQ0LjU3YzAtMi40OS0uNjEtNC0zLjc2LTRhOC4yNiw4LjI2LDAsMCwwLTQuNzQsMS42OFY1Mi42NmgyLjlWNTZINDQuOTJaIi8+PHBhdGggY2xhc3M9ImNscy0xIiBkPSJNODIuMTYsNDMuMTRoLTNsLS41My0yLjgyYTcuODQsNy44NCwwLDAsMC0yLjktLjQ5Yy0yLDAtMi45LDEuMTUtMi45LDIuMzdzLjQ5LDEuOCwyLDIuMjlsMy4xMS45NGMzLjIzLDEsNC45NSwyLjYyLDQuOTUsNS40OHMtMi4xMyw1LjU2LTcuNDgsNS41NmExNy42MiwxNy42MiwwLDAsMS02Ljc5LTEuMzFsLS4zNy01LjM5aDNMNzIuMDcsNTNhNy40OCw3LjQ4LDAsMCwwLDMuMjcuNjVjMS44OCwwLDMuMTktLjgyLDMuMTktMi4zMywwLTEuMTgtLjc0LTEuODgtMi4xNy0yLjM3bC0zLjI3LTFjLTIuODYtLjktNC41NC0yLjU4LTQuNTQtNS4zMlM3MC4zMSwzNyw3NS42NiwzN2ExOS4yNiwxOS4yNiwwLDAsMSw2LjUsMS4xNFoiLz48cGF0aCBjbGFzcz0iY2xzLTEiIGQ9Ik0xMDAuMzEsNTEuMjdsMS4yNywzYTEzLjA2LDEzLjA2LDAsMCwxLTcuMDcsMi4xN2MtNi4yOSwwLTguODMtNC4wOS04LjgzLTkuNzcsMC01LjM1LDIuNzQtOS43Myw4LjQyLTkuNzMsNS40NCwwLDcuNjQsMy45Miw3LjY0LDcuOTNhMTQuMTEsMTQuMTEsMCwwLDEtLjMzLDMuMDdIODkuNzdjMCwzLjE1LDEuNTEsNS4wNyw0Ljc4LDUuMDdBMTAuNjIsMTAuNjIsMCwwLDAsMTAwLjMxLDUxLjI3Wk05Ny45NCw0NWE0LDQsMCwwLDAsMC0uNjVjMC0yLjYyLTEuNTktNC4xMy0zLjkyLTQuMTMtMywwLTQuMDksMi4xMy00LjI1LDQuNzhaIi8+PHBhdGggY2xhc3M9ImNscy0xIiBkPSJNMTA0Ljg1LDUyLjYyaDIuOTRWMzEuNzdoLTIuOTRWMjguMzhoMTEuOWM1LjY0LDAsOC4xMywyLjgyLDguMTMsNy4yLDAsMy4wNy0xLjcyLDUuMzUtNCw1Ljg5YTYuODQsNi44NCwwLDAsMSw1LjI3LDYuNzhjMCw1Ljg5LTQuMTcsNy43Ny05LjQ0LDcuNzdoLTExLjlaTTExNiw0MC4wN2MzLjIzLDAsNC43NC0xLjU1LDQuNzQtNC4yOSwwLTIuNDEtMS4xOS00LTQuNjItNEgxMTEuOHY4LjNabS40OSwxMi41NWM0LjA5LDAsNS42LTEuNzIsNS42LTQuNTQsMC0yLjU4LTEuMzEtNC42Ni01LjY0LTQuNjZIMTExLjh2OS4yWiIvPjxwYXRoIGNsYXNzPSJjbHMtMSIgZD0iTTE0Ni4xNCw0Ni42NWMwLDUuNzYtMi4zMyw5LjgxLTguNzksOS44MS02LjE3LDAtOC41OC00LTguNTgtOS43M1MxMzEuMDksMzcsMTM3LjUxLDM3LDE0Ni4xNCw0MSwxNDYuMTQsNDYuNjVabS00LC4wOGMwLTQuMTMtMS4xLTYuMjEtNC42Ni02LjIxcy00LjY2LDIuMDgtNC42Niw2LjEzLDEuMTgsNi4yNSw0LjcsNi4yNVMxNDIuMDksNTAuNzQsMTQyLjA5LDQ2LjczWiIvPjxwYXRoIGNsYXNzPSJjbHMtMSIgZD0iTTE1NC42NCw1Mi42NkgxNThWNTZoLTkuMzJWNTIuNjZIMTUxbDUtNi4yNS00LjM4LTUuNjhoLTIuOVYzNy4zN2g3LjczdjMuMTlsMy4yNyw0LjI5LDMuMTEtNC4xM2gtMy4zMVYzNy4zN2g5LjI4djMuMzVoLTIuNTNsLTQuNjYsNS45Myw0Ljk1LDZoMi42NlY1NmgtNy42NFY1Mi44MmwtMy42LTQuN1oiLz48cGF0aCBjbGFzcz0iY2xzLTIiIGQ9Ik0yMTguMzksNjEuMjFhMTYuODQsMTYuODQsMCwwLDAtMjMuMjYsMGwxMS42MSwxMS42MVoiLz48cGF0aCBjbGFzcz0iY2xzLTMiIGQ9Ik0yMDUuNjQsMzUuNjVhMS45MiwxLjkyLDAsMSwxLTIuMi0xLjU4LDEuOTEsMS45MSwwLDAsMSwyLjIsMS41OCIvPjxwYXRoIGNsYXNzPSJjbHMtMyIgZD0iTTIwNS42NCw0My43M2ExLjkyLDEuOTIsMCwxLDEtMi4yLTEuNTksMS45MSwxLjkxLDAsMCwxLDIuMiwxLjU5Ii8+PHBhdGggY2xhc3M9ImNscy0zIiBkPSJNMjExLjg4LDUwLjU3YTEuOTIsMS45MiwwLDEsMS0yLjItMS41OSwxLjkyLDEuOTIsMCwwLDEsMi4yLDEuNTkiLz48cGF0aCBjbGFzcz0iY2xzLTMiIGQ9Ik0yMTMuOCwzOS45YTEuOTIsMS45MiwwLDEsMS0yLjItMS41OSwxLjkyLDEuOTIsMCwwLDEsMi4yLDEuNTkiLz48cGF0aCBjbGFzcz0iY2xzLTQiIGQ9Ik0xOTcuMDYsMjguNDJsOS42Ny05LjY3LDkuNzYsOS43NmExNi40NSwxNi40NSwwLDAsMS0xOS40My0uMDhaIi8+PHBvbHlnb24gY2xhc3M9ImNscy0zIiBwb2ludHM9IjIyNC4xOCA1NS40NiAyMjAuNjcgNTEuOTUgMjI0LjMgNDguMzIgMjE3LjA0IDQ4LjMyIDIxNC41MyA0NS44MSAyMTYuOTcgNDMuMzYgMjI0LjM3IDQzLjM2IDIyMC42NyAzOS42NiAyMjQuMTggMzYuMTUgMjMzLjc5IDQ1Ljc2IDIyNC4xOCA1NS40NiIvPjxwYXRoIGNsYXNzPSJjbHMtMSIgZD0iTTIxOC4zOSw2MS4yMWExNi44NCwxNi44NCwwLDAsMC0yMy4yNiwwbDExLjYxLDExLjYxWiIvPjxwYXRoIGNsYXNzPSJjbHMtNSIgZD0iTTIwNS42NCwzNS42NWExLjkyLDEuOTIsMCwxLDEtMi4yLTEuNTgsMS45MSwxLjkxLDAsMCwxLDIuMiwxLjU4Ii8+PHBhdGggY2xhc3M9ImNscy01IiBkPSJNMjA1LjY0LDQzLjczYTEuOTIsMS45MiwwLDEsMS0yLjItMS41OSwxLjkxLDEuOTEsMCwwLDEsMi4yLDEuNTkiLz48cGF0aCBjbGFzcz0iY2xzLTUiIGQ9Ik0yMTEuODgsNTAuNTdhMS45MiwxLjkyLDAsMSwxLTIuMi0xLjU5LDEuOTIsMS45MiwwLDAsMSwyLjIsMS41OSIvPjxwYXRoIGNsYXNzPSJjbHMtNSIgZD0iTTIxMy44LDM5LjlhMS45MiwxLjkyLDAsMSwxLTIuMi0xLjU5LDEuOTIsMS45MiwwLDAsMSwyLjIsMS41OSIvPjxwYXRoIGNsYXNzPSJjbHMtNiIgZD0iTTE5Ny4wNiwyOC40Mmw5LjY3LTkuNjcsOS43Niw5Ljc2YTE2LjQ1LDE2LjQ1LDAsMCwxLTE5LjQzLS4wOFoiLz48cG9seWdvbiBjbGFzcz0iY2xzLTUiIHBvaW50cz0iMjI0LjE4IDU1LjQ2IDIyMC42NyA1MS45NSAyMjQuMyA0OC4zMiAyMTcuMDQgNDguMzIgMjE0LjUzIDQ1LjgxIDIxNi45NyA0My4zNiAyMjQuMzcgNDMuMzYgMjIwLjY3IDM5LjY2IDIyNC4xOCAzNi4xNSAyMzMuNzkgNDUuNzYgMjI0LjE4IDU1LjQ2Ii8+PHBhdGggY2xhc3M9ImNscy01IiBkPSJNMTgyLjcyLDQyLjc2aDBhNC4zMSw0LjMxLDAsMCwxLDAsNi4wOWwyLjQsMi40YTcuNyw3LjcsMCwwLDAsMC0xMC44OGgwWiIvPjxwYXRoIGNsYXNzPSJjbHMtNSIgZD0iTTE4NywzOC41MmExMC4zMiwxMC4zMiwwLDAsMSwwLDE0LjU4bDIuNCwyLjRhMTMuNywxMy43LDAsMCwwLDAtMTkuMzdaIi8+PHBhdGggY2xhc3M9ImNscy01IiBkPSJNMTkxLjIxLDM0LjI3aDBhMTYuMywxNi4zLDAsMCwxLDAsMjMuMDdsMi40LDIuNGExOS42OCwxOS42OCwwLDAsMCwwLTI3Ljg2aDBaIi8+PHBhdGggY2xhc3M9ImNscy01IiBkPSJNMTc5LjY3LDQ1LjgxbDEuNTIsMS41MmEyLjEsMi4xLDAsMCwwLDAtM1oiLz48L3N2Zz4=' alt='senseBox Logo' />";
    html += "</div>";
    html += "<div class='container'>";
    html += "<h1>✅ Konfiguration gespeichert!</h1>";
    html += "<p>Neue WLAN-Daten:</p>";
    html += "<p><strong>SSID:</strong> " + _ssidString + "</p>";
    html += "<p><strong>Passwort:</strong> " + _passwordString + "</p>";
    html += "<p>Das Gerät startet jetzt neu und versucht sich mit dem konfigurierten WLAN zu verbinden.</p>";
    html += "<p>Wenn Sie den Status prüfen möchten, verbinden Sie sich mit dem WLAN und rufen Sie die ";
    html += "<a href='http://192.168.4.1'>Konfigurationsseite</a> auf.</p>";
    html += "<p>Automatischer Neustart in <span class='countdown'>5</span> </p>";
    html += "</div>";
    html += "<script>";
    html += "let countdown = 5;";
    html += "const countdownInterval = setInterval(() => {";
    html += "  if (countdown > 0) {";
    html += "    document.querySelector('.countdown').textContent = countdown--;";  
    html += "  } else {";
    html += "    clearInterval(countdownInterval);";
    html += "    document.querySelector('.countdown').textContent = 'Bereits neu gestartet';";
    html += "    fetch('/restart');";
    html += "  }";
    html += "}, 1000);";
    html += "</script>";
    html += "</body></html>";
    
    return html;
}

String SenseBoxAP::_buildIdsSaveString() {
    String html = "<!DOCTYPE html><html><body>IDs gespeichert! <a href='/'>Zurück</a></body></html>";
    return html;
}

void SenseBoxAP::_setupRoutes() {
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        String html = _buildHTMLString();
        request->send(200, "text/html", html);
    });
    _server.on("/save", HTTP_POST, [this](AsyncWebServerRequest *request) {
        _updateWiFiConnection(request->arg("ssid"), request->arg("password"));
        // IDs ebenfalls aktualisieren
        _updateIds(
            request->arg("sensebox_id"),
            request->arg("sensor_id_1"),
            request->arg("sensor_id_2"),
            request->arg("sensor_id_3")
        );
        _updateMqtt(
            request->hasArg("mqtt_enable") ? true : false,
            request->arg("mqtt_host"),
            request->arg("mqtt_port"),
            request->arg("mqtt_user"),
            request->arg("mqtt_pass"),
            request->arg("mqtt_topic"),
            request->arg("mqtt_field_name")
        );
        String saveHtml = _buildSaveString();
        request->send(200, "text/html", saveHtml);
    });

    _server.on("/saveids", HTTP_POST, [this](AsyncWebServerRequest *request) {
        _updateIds(
            request->arg("sensebox_id"),
            request->arg("sensor_id_1"),
            request->arg("sensor_id_2"),
            request->arg("sensor_id_3")
        );
        _updateMqtt(
            request->hasArg("mqtt_enable") ? true : false,
            request->arg("mqtt_host"),
            request->arg("mqtt_port"),
            request->arg("mqtt_user"),
            request->arg("mqtt_pass"),
            request->arg("mqtt_topic"),
            request->arg("mqtt_field_name")
        );
        String html = _buildIdsSaveString();
        request->send(200, "text/html", html);
    });

    _server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Restarting...");
        delay(1000);
        esp_restart();
    });
}