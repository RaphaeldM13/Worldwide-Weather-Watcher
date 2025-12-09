#ifndef GPSSENS_H
#define GPSSENS_H

#include <TinyGPS.h>
#include <SoftwareSerial.h>
extern SoftwareSerial SoftSerial;
extern int32_t lat;
extern int32_t lng;

void readGPS();
bool checkGPS();
#endif