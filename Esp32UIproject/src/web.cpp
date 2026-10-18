#include "web.h"
#include <WiFi.h>
#include "secrets.h"

// Verbindet den ESP32 mit dem WLAN. Wartet maximal 15 s, danach läuft das Programm trotzdem weiter.
// Bricht die Verbindung später ab, verbindet sich der ESP32 automatisch neu.
void wifiStart() {
  WiFi.mode(WIFI_STA);                 // Station-Modus: ESP32 meldet sich als Teilnehmer im Router an
  WiFi.setAutoReconnect(true);         // bei Verbindungsabbruch automatisch neu verbinden
  WiFi.begin(WIFI_SSID, WIFI_PASS);    // Verbindung starten

  Serial.print("WLAN verbinden");
  const uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(500);
    Serial.print(".");                 // Fortschritt anzeigen
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(" verbunden, IP: ");
    Serial.println(WiFi.localIP());    // diese Adresse brauchst du später im Browser
  } else {
    Serial.println(" fehlgeschlagen");
  }
}