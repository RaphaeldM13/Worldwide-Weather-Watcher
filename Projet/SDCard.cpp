#include "SDCard.h"
#include "config.h"
#include "RTC.h"

static const char STR_NA[]    PROGMEM = "NA";
static const char STR_OFF[]   PROGMEM = "désactivé";

unsigned long totalSize = 0;
uint16_t nb=1;
extern volatile bool Eco;
extern uint16_t temp;
extern uint16_t press;
extern uint8_t hum;
extern uint16_t lum;
extern int32_t lat;
extern int32_t lng;
extern uint32_t yymmdd;

File BFile;
char baseFile[13];

void InitSD() {
  pinMode(4, OUTPUT);
  SD.begin(chipSelect);
  sprintf(baseFile, "%lu_0.txt", (unsigned long)yymmdd); // %lu car yymmdd est un unsigned long
}


void Copy(){
  char SaveFile[13];
  sprintf(SaveFile, "%lu_%d.txt", (unsigned long)yymmdd, nb);
  File BFile = SD.open(baseFile, FILE_READ);
  File SFile = SD.open(SaveFile, FILE_WRITE);
  if (SFile && BFile) {
    while (BFile.available()) {
      SFile.write(BFile.read());
    }
    SFile.close();
    BFile.close();
  }
}

void Save() {
  BFile = SD.open(baseFile, FILE_WRITE);
  GiveValues(BFile);
  if(BFile.size()>=config.file_max_size){
    BFile.close();
    Copy();
    nb++;
    SD.remove(baseFile);
    BFile = SD.open(baseFile, FILE_WRITE); // recrée un fichier vide
    BFile.close();

  }
  else{
    BFile.close();
  }
}

bool checkSD() {
  File myFile = SD.open("test.txt", FILE_WRITE);
  if (!myFile) {
    return false;
  }
  bool success = myFile.println(F("Test d'écriture"));
  myFile.close();
  SD.remove("test.txt");

  return success;
}



void listFiles(File dir) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    if (entry.isDirectory()) {
      listFiles(entry);
    } else {
      totalSize += entry.size();
    }
    entry.close();
  }
}

bool checkPlaceSD() {
  totalSize = 0;
  File root = SD.open("/");
  if (root) {
    listFiles(root);
    root.close();
  }
  const unsigned long capacity = 2044723200UL;
  if(capacity > totalSize){
    return true;
  }
  else{
    return false;
  }
}

void GiveValues(Print& out){
  out.print(F("T=")); 
  if(config.temp_air){
    if(temp < config.min_temp_air || temp > config.max_temp_air){
      out.print((__FlashStringHelper*)STR_NA);
    }
    else{
      out.print(temp);
    }
  }
  else{
    out.print((__FlashStringHelper*)STR_OFF);
  }
  out.print(F("°C, P="));
  if(config.pressure){
    if(press<=config.pressure_max && press>=config.pressure_min){
      out.print(press);
    }
    else{
      out.print((__FlashStringHelper*)STR_NA);
    }
  }
  else{
    out.print((__FlashStringHelper*)STR_OFF);
  }
  out.print(F("hPa, H="));
  if(config.hygr){
    out.print(hum);
  }
  else{
    out.print((__FlashStringHelper*)STR_OFF);
  }
  out.print(F("%, L="));
  if(config.lumin){
    if(lum<=config.lumin_high && lum>=config.lumin_low){
      out.println(F("Moyenne"));
    }
    else if(lum>config.lumin_high){
      out.println(F("Forte"));
    }
    else if (lum<config.lumin_low){
      out.println(F("Basse"));
    }
  }
  else{
    out.print((__FlashStringHelper*)STR_OFF);
  }
  if(Eco==false|| EnReadGPS==true){
    out.print(F("lat = "));
    out.println(lat);
    out.print(F("lng = "));
    out.println(lng);
  }
  else{
    out.print((__FlashStringHelper*)STR_OFF);
  }
}