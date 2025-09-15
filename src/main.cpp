#include <Arduino.h>
#include <TM1637Display.h>

// Define the connections pins
#define CLK 17
#define DIO 16

int sensorValue = 0;
int SensorPin = 21;

float voltage = 0;
int m = 0;
TM1637Display display(CLK, DIO);  // Create an instance of the TM1637Display


void setup() {
  Serial.begin(115200);

  display.setBrightness(5);  // Set the display brightness (0-7)
}


void loop() {
  // put your main code here, to run repeatedly:
  sensorValue = analogRead(13);
  voltage = sensorValue * (5.0 / 1023.0);
  int u = 0;

  if (voltage > 1) {
    //counterStart();
    Serial.println(voltage);

    for (int i = 0; i < 10; i++) {
      display.clear();                       // Clear the display
      display.showNumberDec(i, true, 2, 2);  // __01
      delay(1000);
      sensorValue = analogRead(13);
      voltage = sensorValue * (5.0 / 1023.0);
      Serial.println(voltage);
      if (voltage < 1) {
        u++;
      }
      if (u == 2) {
        i = 10;
      }
    }

  } else {
    Serial.print(voltage);
  }
}



void counterStart() {
  bool boolean = true;
  int seconds = 0;
  int v = 0;
  while (boolean) {
    display.clear();                             // Clear the display
    display.showNumberDec(seconds, true, 2, 2);  // __01
    for (int u = 0; u < 10; u++) {
      sensorValue = analogRead(13);
      voltage = sensorValue * (5.0 / 1023.0);
      Serial.print(voltage);

      if (voltage != 0) {
        v++;
      }
    }
    if (v >= 8) {
      boolean = false;
    }
  }
}