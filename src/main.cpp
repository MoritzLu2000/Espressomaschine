#include <Arduino.h>
#include <TM1637Display.h>
#include <Ticker.h>

// Define the connections pins
#define CLK 17
#define DIO 16

int sensorPin = 13;

int TouchbuttonInputLeft = 22;
int TouchbuttonInputRight = 21;


bool endtimer = false;

bool state = false;
float voltage = 0;
int m = 0;
int i = 0;

TM1637Display display(CLK, DIO);  // Create an instance of the TM1637Display
Ticker Displaytimer;
Ticker Readouttimer;
//---------------------Funktionen----------------------------------------------
void endTimer(void);

void onTimer() {
  if(i==0){
    display.showNumberDec(i, true, 1, 2);
  }
  i++;
  uint8_t tens = i / 10;
  uint8_t ones = i % 10;

  uint8_t prevtens = (i-1) / 10;

  if(prevtens<tens){
    display.showNumberDec(tens, true, 1, 2);
    display.showNumberDec(ones, true, 1, 3);
  }else{
    display.showNumberDec(ones, true, 1, 3);
  }
}



void readout(){
  /*int sensorValue = analogRead(sensorPin);
  voltage = sensorValue * (5.0 / 1023.0);
  Serial.println(voltage);
  if (voltage < 1) {
    if(state){
      Displaytimer.attach(1.0, onTimer);
      state = false;
    }else{
      Displaytimer.detach();
      state = true;
    }
  }*/
}

//---------------------Button Interrupts----------------------------------------------
void IRAM_ATTR onLeftTouch()  {
  endtimer = false;
  Serial.println(state);
  if(state){
    //display.clear();
    Displaytimer.detach();
    Readouttimer.detach();
    state = false;
    endtimer = true;
  }else{
    Displaytimer.attach(1.0, onTimer);
    state = true;
  }
}

void IRAM_ATTR onRightTouch() { Serial.println("rechts");}

void setup() {
  Serial.begin(9600);
  display.setBrightness(5);  // Set the display brightness (0-7)
  display.clear();
  //Displaytimer.attach(1.0, onTimer);
  Readouttimer.attach(0.2, readout);

  attachInterrupt(digitalPinToInterrupt(TouchbuttonInputLeft),  onLeftTouch,  RISING);
  attachInterrupt(digitalPinToInterrupt(TouchbuttonInputRight), onRightTouch, RISING);

  pinMode(TouchbuttonInputLeft, INPUT);
  pinMode(TouchbuttonInputRight, INPUT);

}


void loop() {
  if(endtimer){
    for (int u = 0; u < 5; u++) {   
      if(!endtimer){break;}
      display.showNumberDec(i, true, 2, 2);
      delay(500);
      if(!endtimer){break;}
      display.clear();
      delay(500);
    }

    if(endtimer){
      endtimer = false;
      i=0;
    }
  }
}