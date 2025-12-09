#include <ChainableLED.h>
#include <SoftwareSerial.h>
#include "config.h"
#include "BMESens.h"
#include "GPSSens.h"
#include "SDCard.h"
#include "RTC.h"
#define NUM_LEDS 1
#define PUSHPINV 2
#define PUSHPINR 3

SoftwareSerial SoftSerial(5, 6);
ChainableLED leds(7,8, NUM_LEDS);
volatile unsigned long tps_app = 0;
volatile unsigned long tps_appf = 0;
volatile bool Eco = false;
volatile bool Main = false;
volatile bool BtV = false;
bool EnReadGPS=true;
bool configR=false;
bool error=false;
volatile uint16_t tps = 0;
int16_t temp;
int16_t press;
int16_t hum;
uint16_t lum;
int32_t lat;
int32_t lng;
uint8_t nberr;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  pinMode(PUSHPINV, INPUT_PULLUP); // Initialisation bouton
  pinMode(PUSHPINR, INPUT_PULLUP); // Initialisation bouton
  resetToDefault();
  while(digitalRead(PUSHPINV)==HIGH && digitalRead(PUSHPINR) == HIGH ){
  }
  if(digitalRead(PUSHPINR)==LOW){
    configR=true;
  }
  TCCR1A=0;
  TCCR1B=0;
  TCNT1=0;
  OCR1A=15624;
  TCCR1B =B00001101;
  TIMSK1 |=B00000010;
  tps=0;
  init_interr();
  startBME();
  StartRTC();
  TakeRTC();
  InitSD();

  sei();
}

void init_interr()
{
  attachInterrupt(digitalPinToInterrupt(PUSHPINV),test,CHANGE);
  attachInterrupt(digitalPinToInterrupt(PUSHPINR),testR,CHANGE);
}

void loop(){
  while(configR){
    leds.setColorRGB(0,255,255,0);
    Config();
    if(tps>=config.timeout){
      configR=false;
    }
  }
  if(error==false){
    LedColor();
  }
  if(tps>=(Eco? config.log_interval*2 : config.log_interval)){
    takeValuesSens();
    if(Eco){
      if(EnReadGPS){
        readGPS();
      }
      EnReadGPS=!EnReadGPS;
    } 
    else{
      readGPS();
    }
    if(Main){
      GiveValues(Serial);
      if (!SD.begin(chipSelect)) {
        InitSD();
      }
    }
    else{
      Save();
    }
    checkError();
    TCNT1=0;
    tps=0;
  }
  if(error){
    LedError(nberr);
  }
}

void testR(){
  int val = digitalRead(PUSHPINR);
  if (val == LOW) {   // bouton pressé (avec INPUT_PULLUP)
    start();
  } else {            // bouton relâché
    end();
  }
}

void test(){
  int val = digitalRead(PUSHPINV);
  configR=false;
  BtV = true;
  if (val == LOW) {   // bouton pressé (avec INPUT_PULLUP)
    start();
  } else {            // bouton relâché
    end();
  }
}

void start(){
  tps_app=millis();
}

void end(){
  tps_appf=millis()-tps_app;
  if(tps_appf>4500){
    if(BtV){
      BtV = false;
      Eco = !Eco;
      if(Eco==false){
        EnReadGPS=true;
      }
    }
    else{
      Main = !Main;
    }
  }
  else if(BtV){
    BtV=false;
  }
}

ISR(TIMER1_COMPA_vect){
  tps++;
}

void LedColor(){
  if(Eco && Main == false){
    leds.setColorRGB(0, 0, 0, 255);
  }
  else if(Main){
    leds.setColorRGB(0,255,127,0);
  }
  else{
    leds.setColorRGB(0,0,255,0);
  }
}

void checkError(){
  error=false;
  if (!checkRTC()) {
    error=true;
    nberr=1;
  }
  //if (!checkGPS()) {
  //  error=true;
    //nberr=2;
  //}
  else if (checkSensor==false) {
    error=true;
    nberr=3;
  }
  else if (sensorDataInvalid()) {
    error=true;
    nberr=4;
  }
  else if(!Main){
    if(!checkPlaceSD()){
      error=true;
      nberr=5;
    }
    else if(!checkSD()){
      error=true;
      nberr=6;
    }
  }
}

void LedError(int mode)
{
  if(error){
    static unsigned long lastMillis = 0;
    static bool state = false;
    unsigned long currentMillis = millis();

    if (currentMillis - lastMillis >= 100 && error)
    {
      state = !state;
      lastMillis = currentMillis;
    }

    switch (mode)
    {
      case 1: // RTC
        if (state) leds.setColorRGB(0, 255, 0, 0);
        else       leds.setColorRGB(0, 0, 0, 255);
        break;

      case 2: // GPS
        if (state) leds.setColorRGB(0, 255, 0, 0);
        else       leds.setColorRGB(0, 255, 255, 0);
        break;

      case 3: // Capteur BME
        if (state) leds.setColorRGB(0, 255, 0, 0);
        else       leds.setColorRGB(0, 0, 255, 0);
        break;

      case 4: { // Données incohérentes
        Serial.print(F("ok"));
        static unsigned long phaseStart = 0;
        static bool greenPhase = false;
        if (currentMillis - phaseStart >= (greenPhase ? 1000 : 500))
        {
          greenPhase = !greenPhase;
          phaseStart = currentMillis;
        }
        if (greenPhase) leds.setColorRGB(0, 0, 255, 0);
        else leds.setColorRGB(0, 255, 0, 0);
        break;
      }
      case 5:
        if (state) leds.setColorRGB(0, 255, 0, 0);
        else       leds.setColorRGB(0, 255, 255, 255);
        break;
      case 6: {
        static unsigned long phaseStart = 0;
        static bool greenPhase = false;
        if (currentMillis - phaseStart >= (greenPhase ? 1000 : 500))
        {
          greenPhase = !greenPhase;
          phaseStart = currentMillis;
        }
        if (greenPhase) leds.setColorRGB(0, 255, 255, 255);
        else leds.setColorRGB(0, 255, 0, 0);
        break;
      }
    }
  }
}

bool sensorDataInvalid()
{
  bool tempInvalid = (temp < config.min_temp_air || temp > config.max_temp_air);
  bool pressInvalid = (press < config.pressure_min || press > config.pressure_max);
  return (tempInvalid || pressInvalid);
}