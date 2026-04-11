#include <SPI.h>

const int SS_PIN = D8;

typedef enum {
    STATE_IDLE           = 0,
    STATE_PUMPING        = 1,
    STATE_HEATING_BOILER = 2,
    STATE_HEATING_STEAM  = 3
} MachineState;

typedef enum {
    MODE_OFF    = 0,
    MODE_LOW    = 1,
    MODE_MEDIUM = 2,
    MODE_HIGH   = 3,
    MODE_TURBO  = 4
} MachineMode;

typedef enum {
    FLAG_NONE     = 0,
    FLAG_WARNING  = 1,
    FLAG_ERROR    = 2,
    FLAG_CRITICAL = 3
} MachineFlag;

// Interrupt flags
volatile bool isr_rising  = false;
volatile bool isr_falling = false;

void IRAM_ATTR onRising()  { isr_rising  = true; }
void IRAM_ATTR onFalling() { isr_falling = true; }

uint8_t encodeAll(MachineState state, MachineMode mode, MachineFlag flag) {
    return ((state & 0x03) << 6)
         | ((mode  & 0x0F) << 2)
         | ((flag  & 0x03) << 0);
}

void sendAll(MachineState state, MachineMode mode, MachineFlag flag) {
    uint8_t cmd = encodeAll(state, mode, flag);

    digitalWrite(SS_PIN, LOW);
    uint8_t response = SPI.transfer(cmd);
    digitalWrite(SS_PIN, HIGH);

    Serial.print("Gesendet: 0x"); Serial.print(cmd, HEX);
    Serial.print("  →  Bestätigt: 0x"); Serial.println(response, HEX);
}

void setup() {
    Serial.begin(115200);
    pinMode(SS_PIN, OUTPUT);
    digitalWrite(SS_PIN, HIGH);

    pinMode(D1, INPUT);
    pinMode(D2, INPUT);
    attachInterrupt(digitalPinToInterrupt(D1), onRising,  RISING);
    attachInterrupt(digitalPinToInterrupt(D2), onFalling, FALLING);

    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setFrequency(10000);
}

const MachineState states[] = {
    STATE_IDLE,
    STATE_PUMPING,
    STATE_HEATING_BOILER,
    STATE_HEATING_STEAM
};

const char* stateNames[] = {
    "IDLE",
    "PUMPING",
    "HEATING_BOILER",
    "HEATING_STEAM"
};

const int stateCount = 4;
int currentIndex = 0;

void loop() {
    if (isr_rising)  { isr_rising  = false; Serial.println("ISR: Rising  D1"); }
    if (isr_falling) { isr_falling = false; Serial.println("ISR: Falling D2"); }

    Serial.print("[");
    Serial.print(stateNames[currentIndex]);
    Serial.print("] ");

    // Testweise: Mode und Flag fest, nur State wechselt
    sendAll(states[currentIndex], MODE_LOW, FLAG_NONE);

    currentIndex = (currentIndex + 1) % stateCount;
    delay(100);
}