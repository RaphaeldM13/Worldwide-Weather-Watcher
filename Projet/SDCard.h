#ifndef SDMODULE_H
#define SDMODULE_H

#include <SPI.h>
#include <SD.h>

const uint8_t chipSelect = 4;

extern bool EnReadGPS;
extern volatile bool Eco;
extern File BFile;   // on utilisera File au lieu de SdFile

void InitSD();
void GiveValues(Print& out);
void Save();
bool checkPlaceSD();
bool checkSD();

#endif