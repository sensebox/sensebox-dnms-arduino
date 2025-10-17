# senseBox-Config-AP: Anleitung zur Verwendung

## Einrichtung des Geräts

### 1. Gerät starten

- Schalten Sie das senseBox-Gerät ein
- Warten Sie einige Sekunden, bis das Gerät vollständig gestartet ist
- Das Gerät erstellt automatisch einen Access Point (WLAN-Hotspot)

### 2. Mit dem Access Point verbinden

- Öffnen Sie die WLAN-Einstellungen Ihres Smartphones oder Computers
- Suchen Sie nach verfügbaren WLAN-Netzwerken
- Wählen Sie das Netzwerk mit dem Namen **"senseBox-Access-Point"** aus
- Geben Sie das Passwort **"12345678"** ein
- Bestätigen Sie die Verbindung

### 3. Konfigurationsseite aufrufen

- Öffnen Sie Ihren Webbrowser
- Scannen Sie den QR-Code unten ein (führt zu http://192.168.4.1)
- Alternativ geben Sie die Adresse **192.168.4.1** manuell in die Adresszeile ein

### 4. WLAN-Konfiguration

- Gehen Sie auf der Webseite zum Abschnitt **"WLAN Verbindung"**
- Geben Sie die **SSID** Ihres Heim-WLANs in das erste Eingabefeld ein
- Geben Sie das **Passwort** Ihres Heim-WLANs in das zweite Eingabefeld ein
- Klicken Sie auf den Button **"Speichern & Neustarten"**

### 5. SenseBox-Konfiguration

- Gehen Sie auf der Webseite zum Abschnitt **"SenseBox Einstellungen"**
- Geben Sie Ihre **SenseBox ID** in das erste Eingabefeld ein
  - Diese finden Sie in Ihrem senseBox-Account unter "Meine senseBoxen"
- Geben Sie Ihre **Sensor ID** in das zweite Eingabefeld ein
  - Diese finden Sie unter "Sensoren" für Ihre senseBox
- Klicken Sie auf den Button **"IDs speichern"**

### 6. Neustart und Verbindung

- Nach dem Speichern startet das Gerät automatisch neu (5 Sekunden Countdown)
- Während des Neustarts sehen Sie die Meldung **"Bereits neu gestartet"**
- Das Gerät versucht nun, sich mit Ihrem konfigurierten WLAN zu verbinden
- Nach erfolgreicher Verbindung beginnt die Datenerfassung und -übertragung

### 7. Statusüberprüfung

- Wenn Sie den Status überprüfen möchten, verbinden Sie sich erneut mit dem Access Point
- Rufen Sie die Konfigurationsseite unter **http://192.168.4.1** auf
- Dort sehen Sie den aktuellen Verbindungsstatus

---

**Wichtig:**

- Das Gerät speichert Ihre Einstellungen dauerhaft
- Bei Änderungen können Sie jederzeit zur Konfigurationsseite zurückkehren
- Das Gerät sendet alle 60 Sekunden einen Messwert an die OpenSenseMap

---

### QR-Code für die Konfigurationsseite
