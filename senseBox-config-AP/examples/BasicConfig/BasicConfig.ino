/*
 * senseBox Config AP - Beispiel
 * 
 * Dieses Beispiel zeigt, wie die senseBox Config AP Bibliothek verwendet wird.
 * 
 * Funktionen:
 * - Startet einen Access Point mit benutzerdefinierten Einstellungen
 * - Web-Oberfläche zur Konfiguration des Ziel-WLANs
 * - Speicherung der Einstellungen in Preferences
 * - Automatischer Neustart nach Konfiguration
 */

#include "SenseBoxAP.h"

// Erstelle eine Instanz
SenseBoxAP configAP;

void setup() {
  // Initialisiere die Konfiguration mit benutzerdefinierten AP-Einstellungen
  configAP.begin("MeinAP", "meinpasswort123");
}

void loop() {
  // Hauptfunktionen laufen im Hintergrund
  configAP.handle();
}