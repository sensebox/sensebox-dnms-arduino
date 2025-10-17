# senseBox-Config-AP Bibliothek

## Überblick

Die senseBox-Config-AP Bibliothek erstellt einen Access Point für die Konfiguration von senseBox-Geräten. Sie ermöglicht die Einstellung von WLAN-Zugangsdaten und OpenSenseMap-IDs über eine Web-Oberfläche.

## Funktionen

- Startet einen Access Point (Standard: "senseBox-AccessPoint")
- Web-Oberfläche für WLAN-Konfiguration
- Web-Oberfläche für OpenSenseMap-Einstellungen
- Speicherung in Preferences
- Automatischer Neustart nach Konfiguration
- HTTPS-Übertragung an OpenSenseMap

## Installation

1. Lade die Bibliothek als ZIP herunter
2. In Arduino IDE: Sketch → Bibliothek einbinden → .ZIP-Bibliothek hinzufügen
3. Wähle die ZIP-Datei aus

## Abhängigkeiten

- ESP Async WebServer
- ArduinoJson
- HTTPClient

## Verwendung

```cpp
#include "SenseBoxAP.h"

SenseBoxAP configAP;

void setup() {
  // Mit Standard-AP-Einstellungen
  configAP.begin();

  // Oder mit benutzerdefinierten Einstellungen
  // configAP.begin("MeinAP", "meinPasswort");
}

void loop() {
  configAP.handle();
}
```

## Methoden

- `begin()` - Startet AP mit Standard-Einstellungen
- `begin(const char*, const char*)` - Startet AP mit benutzerdefinierten Einstellungen
- `handle()` - Verarbeitet Web-Anfragen

## Lizenz

MIT License
