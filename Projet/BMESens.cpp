#include "BMESens.h"
#include "config.h"

Adafruit_BME280 bme;

extern int16_t temp;
extern int16_t press;
extern int16_t hum;
extern uint16_t lum;
bool checkSensor = true;

void startBME() {
  if(!bme.begin(0x76)){
    if(!bme.begin(0x77)){
      checkSensor=false;
    }
  }
}

void takeValuesSens() {
  temp = config.temp_air ? bme.readTemperature() : temp;
  press = config.pressure? bme.readPressure() / 100.0F :press;
  hum = config.hygr? bme.readHumidity():hum;
  lum = analogRead(A0);
}