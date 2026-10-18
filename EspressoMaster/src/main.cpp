#include <HX711.h>

#define SCK_PIN 19

#define DT_PIN_1 23
#define DT_PIN_2 18

#define WaterLevel_1 26

HX711 scale1, scale2;

// Kalibrierung
const int KALIBRIERUNG_ANZAHL = 50;
long kalSumme1 = 0, kalSumme2 = 0;
int kalCount = 0;
bool kalibriert = false;
long nullwert1 = 0, nullwert2 = 0;

// Filter
const int FILTER_GROESSE = 6;
long filterPuffer1[FILTER_GROESSE];
long filterPuffer2[FILTER_GROESSE];
int filterIndex1 = 0, filterIndex2 = 0;
long filterSumme1 = 0, filterSumme2 = 0;
bool filterVoll1 = false, filterVoll2 = false;

long gefilterterWert(long neuerWert, long* puffer, int& index, long& summe, bool& voll) {
    summe -= puffer[index];
    puffer[index] = neuerWert;
    summe += neuerWert;
    index = (index + 1) % FILTER_GROESSE;
    if (!voll && index == 0) voll = true;
    int anzahl = voll ? FILTER_GROESSE : index;
    return summe / anzahl;
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    scale1.begin(DT_PIN_1, SCK_PIN);
    scale2.begin(DT_PIN_2, SCK_PIN);
    scale1.set_scale();
    scale2.set_scale();
    Serial.println("Kalibrierung – nichts auflegen!");

      pinMode(WaterLevel_1, INPUT_PULLUP);
}

void loop() {
    if (!kalibriert) {
        if (scale1.is_ready() && scale2.is_ready()) {
            kalSumme1 += scale1.get_value();
            kalSumme2 += scale2.get_value();
            kalCount++;

            if (kalCount % 100 == 0) {
                Serial.print("Kalibrierung: ");
                Serial.print(kalCount);
                Serial.println("/500");
            }

            if (kalCount >= KALIBRIERUNG_ANZAHL) {
                nullwert1 = kalSumme1 / KALIBRIERUNG_ANZAHL;
                nullwert2 = kalSumme2 / KALIBRIERUNG_ANZAHL;
                kalibriert = true;
                Serial.print("Nullwert 1: "); Serial.println(nullwert1);
                Serial.print("Nullwert 2: "); Serial.println(nullwert2);
                Serial.println("Jetzt bekanntes Gewicht auflegen!");
            }
        }
    } else {
        
        if (scale1.is_ready() && scale2.is_ready()) {
            long roh1 = scale1.get_value() - nullwert1;
            long roh2 = scale2.get_value() - nullwert2;

            long gef1 = gefilterterWert(roh1, filterPuffer1, filterIndex1, filterSumme1, filterVoll1);
            long gef2 = gefilterterWert(roh2, filterPuffer2, filterIndex2, filterSumme2, filterVoll2);

            long gesamt = gef1 + gef2;

            Serial.print("Waage1: "); Serial.print(gef1);
            Serial.print(" | Waage2: "); Serial.print(gef2);
            Serial.print(" | Gesamt: "); Serial.println(gesamt);
        }
    }
    if (digitalRead(WaterLevel_1) == LOW) {
        Serial.println("Flüssigkeit erkannt!");
    }
}