#include "stm32_flash.h"
#include <LittleFS.h>




// STM32 in den ROM-Bootloader starten und synchronisieren.
// Rückgabe: Antwort-Byte (0x79 = ACK) oder -1 bei Timeout.
int bootSTM32() {
  Serial2.begin(115200, SERIAL_8E1, PIN_STM_RX, PIN_STM_TX); // starte serielle comm mit STM32

  digitalWrite(PIN_BOOT0, HIGH);   // Boot pin wird auf high gesetzt damit bei nächstem neustart in bootmodus gegangen wird
  delay(2);

  digitalWrite(PIN_NRST, LOW);     // STM32 reset wird erzwungen
  delay(5);
  digitalWrite(PIN_NRST, HIGH);    
  delay(100);                      

  // Boot wird erzwungen

  while (Serial2.available()) Serial2.read();   // Müll verwerfen

  Serial2.write(0x7F);             // Baudrate wird auf dem stm32 festgelegt

  const uint32_t t0 = millis();
  while (!Serial2.available()) {
    if (millis() - t0 > 500) return -1;
  }
  return Serial2.read();
}

// Wartet auf ein Byte vom STM32. Rückgabe: Byte oder -1 bei Timeout.
int rdByte(uint32_t timeoutMs) {
    const uint32_t t0 = millis();
  while (!Serial2.available()) if (millis() - t0 > timeoutMs) return -1;
  return Serial2.read();
}




// ----- Löschen des bestehenden Flashs auf dem STM32 -----
// Der Programmcode, welcher das Verhalten des STM32 definiert, liegt im Flash. 
// Flash-Bits können beim Schreiben aber nur von 1 auf 0 gesetzt werden, deshalb muss immer ein ganzer Bereich gelöscht werden um ihn neu zu beschreiben das heiß hier 0xFF.
// Der Flash ist in Pages unterteilt und ist die kleinste Einheit, die auf einmal gelöscht werden kann.
// STM32G474RE: 512 KB Flash = 256 Pages zu je 2 KB (Dual-Bank, Werkseinstellung).
// Gelöscht werden nur die Pages, die die neue Firmware belegt: Anzahl = Dateigröße / 2048, aufgerundet.
// gibt eine 1 zurück wenn die Page gelöscht wurde.
bool erasePageSTM32(uint16_t page) {
  while (Serial2.available()) Serial2.read();   // Empfangspuffer leeren, alte Bytes verwerfen
  
  Serial2.write(0x44);                 // Extended Erase als Befehl geschickt
  Serial2.write(0xBB);                 // Prüfungsnachricht das Übertragungsfehler aufgedeckt werden
  if (rdByte() != ACK) return false;
  const uint8_t d[4] = {0x00, 0x00, uint8_t(page >> 8), uint8_t(page)}; // konstantes Array d wird erstellt mit vier Stellen, die je ein Byte enthalten und die Parameter für den Bootloader sind, damit dieser weiß, wie viele und welche Pages er löschen muss
  // das heißt der Bootloader hört: lösche (0x44) x Seiten (x = d[0...1] + 1), diese Seiten sind y (y = d[2...3]), danach schicke ich dir die Prüfsumme, um zu prüfen, ob die Übertragung korrekt war
  // in diesem Fall wird also nur eine Seite gelöscht
  // nun tritt das Problem auf, dass jede Page-Nummer 16 Bit hat, aber per UART nur einzelne Bytes übertragen werden können, deshalb wird die Page-Nummer in zwei Bytes aufgeteilt
  // es werden also die letzten 8 Bit in d[3] gespeichert, indem einfach die vorderen 8 wegfallen, und die vorderen 8 Bit werden um 8 Stellen nach rechts verschoben und in d[2] gespeichert
  
  Serial2.write(d, 4); // die vier Bytes aus d werden nun übertragen und der bootloader weiß jetzt was er lösche muss
  Serial2.write(d[0] ^ d[1] ^ d[2] ^ d[3]);          // Prüfsumme
  return rdByte(5000) == ACK;                        // Löschen dauert länger
}

// ----- Beschreiben des  Flashs auf dem STM32 -----
// Nachdem eine Page gelöscht wurde (alle Bytes 0xFF), kann sie neu beschrieben werden.
// Pro Befehl können maximal 256 Bytes geschrieben werden.
// Der STM32G474 schreibt immer 8 Bytes (64 Bit) auf einmal, deshalb muss n ein Vielfaches von 8 sein.
// Diese Funktion schreibt n Bytes ab der Adresse addr und gibt true zurück, wenn das Schreiben erfolgreich war.
bool writeMemSTM32(uint32_t addr, const uint8_t* data, uint16_t n) {
  while (Serial2.available()) Serial2.read();   // Empfangspuffer leeren, alte Bytes verwerfen

  Serial2.write(0x31);                 // Write Memory als Befehl geschickt
  Serial2.write(0xCE);                 // Komplement von 0x31, damit Übertragungsfehler aufgedeckt werden
  if (rdByte() != ACK) return false;   // Bootloader bestätigt den Befehl

  const uint8_t a[4] = {uint8_t(addr >> 24), uint8_t(addr >> 16), uint8_t(addr >> 8), uint8_t(addr)}; // konstantes Array a wird erstellt mit vier Stellen, die je ein Byte enthalten und zusammen die Startadresse ergeben, damit der Bootloader weiß, wohin er schreiben soll
  // die Adresse hat 32 Bit, per UART können aber nur einzelne Bytes übertragen werden, deshalb wird sie in vier Bytes aufgeteilt
  // a[0] = vorderste 8 Bit (um 24 Stellen nach rechts verschoben), a[1] = um 16 verschoben, a[2] = um 8 verschoben, a[3] = letzte 8 Bit (die vorderen 24 fallen weg)
  // Beispiel 0x0803F800 -> a = {0x08, 0x03, 0xF8, 0x00}

  Serial2.write(a, 4);                               // die vier Bytes der Adresse werden übertragen
  Serial2.write(a[0] ^ a[1] ^ a[2] ^ a[3]);          // Prüfsumme der Adresse: XOR über alle vier Bytes
  if (rdByte() != ACK) return false;                 // Bootloader bestätigt die Adresse

  // jetzt hört der Bootloader: es kommen x Bytes (x = gesendeter Wert + 1), dann die Daten, danach die Prüfsumme
  uint8_t pruefsumme = uint8_t(n - 1);               // die Prüfsumme beginnt mit der Anzahl - 1
  Serial2.write(uint8_t(n - 1));                     // Anzahl der Bytes - 1 wird übertragen
  for (uint16_t i = 0; i < n; i++) {
    Serial2.write(data[i]);                          // jedes Datenbyte wird einzeln übertragen
    pruefsumme = pruefsumme ^ data[i];               // und dabei in die Prüfsumme eingerechnet
  }
  Serial2.write(pruefsumme);                         // Prüfsumme über Anzahl - 1 und alle Daten
  return rdByte(1000) == ACK;                        // ACK kommt erst nach dem Schreiben
}


// Mit READ kann der Inhalt des Flashs zurückgelesen werden, ohne etwas zu verändern.
// So wird geprüft, ob Erase (alle Bytes 0xFF) und Write (gewünschte Werte) wirklich funktioniert haben.
// Pro Befehl können maximal 256 Bytes gelesen werden.
// Diese Funktion liest n Bytes ab der Adresse addr in buf und gibt true zurück, wenn das Lesen erfolgreich war.
bool readMemSTM32(uint32_t addr, uint8_t* buf, uint16_t n) {
  while (Serial2.available()) Serial2.read();   // Empfangspuffer leeren, alte Bytes verwerfen

  Serial2.write(0x11);                 // Read Memory als Befehl geschickt
  Serial2.write(0xEE);                 // Komplement von 0x11, damit Übertragungsfehler aufgedeckt werden
  if (rdByte() != ACK) return false;   // Bootloader bestätigt den Befehl

  const uint8_t a[4] = {uint8_t(addr >> 24), uint8_t(addr >> 16), uint8_t(addr >> 8), uint8_t(addr)}; // konstantes Array a wird erstellt mit vier Stellen, die je ein Byte enthalten und zusammen die Startadresse ergeben, damit der Bootloader weiß, wo er lesen soll
  // die Adresse hat 32 Bit, per UART können aber nur einzelne Bytes übertragen werden, deshalb wird sie in vier Bytes aufgeteilt
  // a[0] = vorderste 8 Bit (um 24 Stellen nach rechts verschoben), a[1] = um 16 verschoben, a[2] = um 8 verschoben, a[3] = letzte 8 Bit (die vorderen 24 fallen weg)
  // Beispiel 0x0803F800 -> a = {0x08, 0x03, 0xF8, 0x00}

  Serial2.write(a, 4);                               // die vier Bytes der Adresse werden übertragen
  Serial2.write(a[0] ^ a[1] ^ a[2] ^ a[3]);          // Prüfsumme der Adresse: XOR über alle vier Bytes
  if (rdByte() != ACK) return false;                 // Bootloader bestätigt die Adresse

  // jetzt hört der Bootloader: schicke mir x Bytes (x = gesendeter Wert + 1)
  Serial2.write(uint8_t(n - 1));                     // Anzahl der Bytes - 1 wird übertragen
  Serial2.write(uint8_t(~(n - 1)));                  // Komplement der Anzahl, damit Übertragungsfehler aufgedeckt werden
  if (rdByte() != ACK) return false;                 // Bootloader bestätigt die Anzahl

  for (uint16_t i = 0; i < n; i++) {                 // jetzt schickt der Bootloader die n Bytes
    const int c = rdByte();                          // ein Byte empfangen
    if (c < 0) return false;                         // Timeout: es kam kein Byte
    buf[i] = uint8_t(c);                             // Byte im Puffer speichern
  }
  return true;                                       // alle Bytes empfangen
}


// Flasht die Datei /stm32.bin aus dem Dateisystem des ESP32 in den STM32.
// Ablauf: Bootloader starten, belegte Pages löschen, Datei in 256-Byte-Blöcken schreiben,
// alles zurücklesen und vergleichen, danach startet der STM32 die neue Firmware.
// Gibt true zurück, wenn alles erfolgreich war.
bool flashFileSTM32() {
  File f = LittleFS.open("/stm32.bin", "r");         // Datei zum Lesen öffnen
  if (!f) { Serial.println("Datei /stm32.bin nicht gefunden"); return false; }

  const uint32_t groesse = f.size();                 // Dateigröße in Bytes
  const uint16_t pages = (groesse + 2047) / 2048;    // Anzahl belegter Pages, aufgerundet (+2047 sorgt fürs Aufrunden)
  Serial.printf("Datei: %u Bytes = %u Pages\n", groesse, pages);

  if (bootSTM32() != ACK) { Serial.println("Boot fehlgeschlagen"); f.close(); return false; }

  // 1. Löschen: jede belegte Page einzeln, beginnend bei Page 0 (Adresse 0x08000000)
  for (uint16_t p = 0; p < pages; p++) {
    if (!erasePageSTM32(p)) { Serial.printf("ERASE Page %u fehlgeschlagen\n", p); f.close(); exitBootSTM32(); return false; }
  }
  Serial.println("ERASE ok");

  // 2. Schreiben: die Datei wird in Blöcken zu 256 Bytes gelesen und an die passende Adresse geschrieben
  uint8_t block[256];
  for (uint32_t offset = 0; offset < groesse; offset += 256) {
    uint16_t n = f.read(block, 256);                 // bis zu 256 Bytes aus der Datei lesen, der letzte Block ist meist kürzer
    while (n % 8 != 0) block[n++] = 0xFF;            // auf ein Vielfaches von 8 auffüllen, 0xFF = gelöschter Zustand
    if (!writeMemSTM32(0x08000000 + offset, block, n)) {
      Serial.printf("WRITE bei 0x%08X fehlgeschlagen\n", 0x08000000 + offset); f.close(); exitBootSTM32(); return false;
    }
  }
  Serial.println("WRITE ok");

  // 3. Prüfen: alles zurücklesen und Byte für Byte mit der Datei vergleichen
  f.seek(0);                                         // zurück an den Anfang der Datei
  uint8_t soll[256], ist[256];
  for (uint32_t offset = 0; offset < groesse; offset += 256) {
    uint16_t n = f.read(soll, 256);
    if (!readMemSTM32(0x08000000 + offset, ist, n)) {
      Serial.printf("READ bei 0x%08X fehlgeschlagen\n", 0x08000000 + offset); f.close(); exitBootSTM32(); return false;
    }
    if (memcmp(soll, ist, n) != 0) {
      Serial.printf("VERIFY Fehler im Block ab 0x%08X\n", 0x08000000 + offset); f.close(); exitBootSTM32(); return false;
    }
  }
  Serial.println("VERIFY ok");

  f.close();
  exitBootSTM32();                                   // BOOT0 low + Reset: der STM32 startet die neue Firmware
  return true;
}



// Zurück in die normale Firmware
void exitBootSTM32() {
  digitalWrite(PIN_BOOT0, LOW);
  delay(2);
  digitalWrite(PIN_NRST, LOW);
  delay(5);
  digitalWrite(PIN_NRST, HIGH);
}
