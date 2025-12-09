#ifndef BMESENS_H
#define BMESENS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

extern Adafruit_BME280 bme;

extern int16_t temp;
extern int16_t press;
extern int16_t hum;
extern uint16_t lum;
extern bool checkSensor;

void startBME();
void takeValuesSens();

#endif
