#include <Arduino.h>
#include <LittleFS.h>
#include "stm32_flash.h"   // alle Funktionen zum Flashen des STM32
#include "web.h"

void setup() {
  Serial.begin(115200);
  if (!LittleFS.begin()) Serial.println("LittleFS Fehler");   // Dateisystem des ESP32 einbinden
  pinMode(PIN_BOOT0, OUTPUT);
  digitalWrite(PIN_BOOT0, LOW);
  pinMode(PIN_NRST, OUTPUT_OPEN_DRAIN);                        // nie aktiv high treiben
  digitalWrite(PIN_NRST, HIGH);
  Serial2.begin(115200, SERIAL_8N1, PIN_STM_RX, PIN_STM_TX);   // Betriebs-UART zum STM32
  wifiStart();
}

void loop() {
  // alles, was der STM32 schickt, direkt an den PC weitergeben
  while (Serial2.available()) Serial.write(Serial2.read());

  // Taste im Monitor: STM32 neu flashen
  if (Serial.available()) {
    while (Serial.available()) Serial.read();                     // Eingabe verwerfen
    Serial.println(flashFileSTM32() ? "FLASH erfolgreich" : "FLASH fehlgeschlagen");
    Serial2.begin(115200, SERIAL_8N1, PIN_STM_RX, PIN_STM_TX);   // zurück auf Betriebs-UART (Bootloader nutzte 8E1)
  }
}