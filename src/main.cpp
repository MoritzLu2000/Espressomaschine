#include <Arduino.h>
#include <TM1637Display.h>
#include <Ticker.h>

// Define the connections pins
#define CLK 17
#define DIO 16

int sensorValue = 0;
int SensorPin = 21;

int TouchbuttonInputLeft = 22;
int TouchbuttonInputRight = 21;



float voltage = 0;
int m = 0;
int i =0 ;
TM1637Display display(CLK, DIO);  // Create an instance of the TM1637Display
Ticker Displaytimer;
Ticker Readouttimer;


void onTimer() {
  uint8_t tens = i / 10;
  uint8_t ones = i % 10;

  uint8_t prevtens = (i-1) / 10;

  if(prevtens<tens){
    display.showNumberDec(tens, true, 1, 2);
    display.showNumberDec(ones, true, 1, 3);
  }else{
    display.showNumberDec(ones, true, 1, 3);
  }
  i++;
}

void readout(){}

void IRAM_ATTR onLeftTouch()  { Serial.println("links");
}
void IRAM_ATTR onRightTouch() { Serial.println("rechts");
}


void setup() {
  Serial.begin(9600);
  display.setBrightness(5);  // Set the display brightness (0-7)

  Displaytimer.attach(1.0, onTimer);
  Readouttimer.attach(0.2, readout);

  attachInterrupt(digitalPinToInterrupt(TouchbuttonInputLeft),  onLeftTouch,  CHANGE);
  attachInterrupt(digitalPinToInterrupt(TouchbuttonInputRight), onRightTouch, CHANGE);

  pinMode(TouchbuttonInputLeft, INPUT);
  pinMode(TouchbuttonInputRight, INPUT);

}


void loop() {

}