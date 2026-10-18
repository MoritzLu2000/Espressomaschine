#pragma once                     // Datei nur einmal einbinden, auch wenn mehrfach #include
#include <Arduino.h>

// ------ Pins ESP32 <-> STM32 ------
constexpr int PIN_STM_RX = 17;   // ESP empfängt <- STM PA9
constexpr int PIN_STM_TX = 16;   // ESP sendet   -> STM PA10
constexpr int PIN_BOOT0  = 25;   // -> PB8-BOOT0
constexpr int PIN_NRST   = 26;   // -> NRST

// ------ Antworten des Bootloaders ------
constexpr uint8_t ACK  = 0x79;
constexpr uint8_t NACK = 0x1F;

// ------ Prototypen ------
int  bootSTM32();
int  rdByte(uint32_t timeoutMs = 500);
bool erasePageSTM32(uint16_t page);
bool writeMemSTM32(uint32_t addr, const uint8_t* data, uint16_t n);
bool readMemSTM32(uint32_t addr, uint8_t* buf, uint16_t n);
void exitBootSTM32();
bool flashFileSTM32();