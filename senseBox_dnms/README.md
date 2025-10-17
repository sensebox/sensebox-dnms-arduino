# senseBox Lautstärke-Logger

## Überblick

Der senseBox Lautstärke-Logger kombiniert die SoundSensor- und Config-AP-Bibliotheken, um kontinuierlich Lautstärke-Messwerte zu erfassen und an die OpenSenseMap zu senden. Der Sketch ermöglicht eine einfache Konfiguration über eine Web-Oberfläche.

## Funktionen

- Lautstärke-Messung über DNMS-Teensy (I2C)
- Konfiguration über Access Point-Webseite
- Speicherung von WLAN und OpenSenseMap-IDs
- HTTPS-Datenübertragung an OpenSenseMap
- Intervallgesteuertes Senden (alle 60 Sekunden)

## Installation

1. Installiere die Abhängigkeiten:
   - SoundSensor Bibliothek
   - SenseBox-Config-AP Bibliothek
2. Öffne den Sketch in der Arduino IDE
3. Lade auf das ESP32-Board

## Verwendung

1. Gerät starten → Access Point erscheint
2. Mit AP verbinden (Standard: senseBox-AccessPoint / 12345678)
3. Seite öffnen: `192.168.4.1`
4. WLAN-Zugangsdaten eingeben
5. OpenSenseMap-IDs eingeben (Box-ID und 3 Sensor-IDs)
6. "Speichern & Neustarten" klicken
7. Gerät startet neu und sendet Messwerte

## Anpassung

- Messintervall: Ändere `MEASUREMENT_INTERVAL` (in Sekunden)
- Standard-AP-Einstellungen: Passe Parameter in `setup()` an

## Lizenz

MIT License
