// sensors.c

#include "sensors.h"

// ─── Low-Level SPI Hilfsfunktionen ───────────────────────────────────────────

static void MAX31865_Select(void) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); // CS low
}

static void MAX31865_Deselect(void) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);   // CS high
}

static void MAX31865_WriteReg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value) {
    uint8_t buf[2] = { reg | 0x80, value }; // Bit7 = 1 → Schreiben
    MAX31865_Select();
    HAL_SPI_Transmit(hspi, buf, 2, HAL_MAX_DELAY);
    MAX31865_Deselect();
}

static uint8_t MAX31865_ReadReg(SPI_HandleTypeDef *hspi, uint8_t reg) {
    uint8_t tx = reg & 0x7F; // Bit7 = 0 → Lesen
    uint8_t rx = 0;
    MAX31865_Select();
    HAL_SPI_Transmit(hspi, &tx, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(hspi, &rx, 1, HAL_MAX_DELAY);
    MAX31865_Deselect();
    return rx;
}

// ─── Initialisierung ─────────────────────────────────────────────────────────

void MAX31865_Init(SPI_HandleTypeDef *hspi) {
    // BIAS an, Auto-Conversion, 50Hz Filter, 2/4-Draht
    MAX31865_WriteReg(hspi, 0x00, 0xC1);
    //                            ││└─ 50Hz Filter
    //                            │└── Auto-Conversion
    //                            └─── BIAS ein
    HAL_Delay(100); // Einschwingzeit abwarten
}

// ─── Rohdaten lesen ──────────────────────────────────────────────────────────

static uint16_t MAX31865_ReadRawADC(SPI_HandleTypeDef *hspi) {
    uint8_t msb = MAX31865_ReadReg(hspi, 0x01);
    uint8_t lsb = MAX31865_ReadReg(hspi, 0x02);

    uint16_t raw = ((uint16_t)msb << 8) | lsb;

    if (raw & 0x01) {
        // Fault-Bit gesetzt → Fehlerbehandlung
        // MAX31865_ReadFault(hspi) aufrufen
        return 0;
    }

    return raw >> 1; // Bit0 ist das Fault-Flag, kein Messwert
}

// ─── Widerstand berechnen ────────────────────────────────────────────────────

static float MAX31865_RawToResistance(uint16_t raw) {
    // R_RTD = (ADC_Code * R_REF) / 2^15
    return ((float)raw * 400.0f) / 32768.0f; // 400Ω für PT100
}

// ─── Temperatur berechnen (Callendar-Van Dusen) ──────────────────────────────

static float MAX31865_ResistanceToTemperature(float resistance) {
    // Vereinfachte Näherung für -100°C bis +850°C
    #define R0      100.0f      // PT100 Nennwiderstand bei 0°C
    #define ALPHA   0.00385f    // IEC 751 Koeffizient

    return (resistance - R0) / (R0 * ALPHA);
}

// ─── Fault-Register auslesen ─────────────────────────────────────────────────

uint8_t MAX31865_ReadFault(SPI_HandleTypeDef *hspi) {
    return MAX31865_ReadReg(hspi, 0x07);
}

// ─── Hauptfunktion: Temperatur lesen ─────────────────────────────────────────

float MAX31865_ReadTemperature(SPI_HandleTypeDef *hspi) {
    uint16_t raw        = MAX31865_ReadRawADC(hspi);
    float    resistance = MAX31865_RawToResistance(raw);
    return MAX31865_ResistanceToTemperature(resistance);
}
