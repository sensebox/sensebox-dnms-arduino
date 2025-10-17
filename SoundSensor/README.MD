# SoundSensor Bibliothek

## Überblick

Die SoundSensor Bibliothek ermöglicht das Auslesen von Lautstärke-Werten (dBA) von einem DNMS-Teensy über I2C. Sie liefert Mittelwert, Minimum und Maximum in einem konfigurierbaren Intervall.

## Funktionen

- Auslesen von LAeq (durchschnittliche Lautstärke)
- Auslesen von LAmin (minimaler Pegel)
- Auslesen von LAmax (maximaler Pegel)
- Intervall-basierte Messung (Standard: 1 Sekunde)

## Installation

1. Lade die Bibliothek als ZIP herunter
2. In Arduino IDE: Sketch → Bibliothek einbinden → .ZIP-Bibliothek hinzufügen
3. Wähle die ZIP-Datei aus

## Verwendung

```cpp
#include "SoundSensor.h"

SoundSensor sensor;

void setup() {
  sensor.begin();
}

void loop() {
  sensor.update();  // Aktualisiert Messwerte

  int avg = sensor.average();  // Mittelwert
  int min = sensor.min();      // Minimum
  int max = sensor.max();      // Maximum

  delay(1000);
}
```

## Methoden

- `begin()` - Initialisiert die I2C-Kommunikation
- `update()` - Aktualisiert die Messwerte
- `average()` - Gibt den Durchschnittswert zurück
- `min()` - Gibt den Minimalwert zurück
- `max()` - Gibt den Maximalwert zurück

## Lizenz

MIT License
