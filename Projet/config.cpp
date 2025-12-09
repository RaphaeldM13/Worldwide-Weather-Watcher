#include <Arduino.h>
#include <avr/pgmspace.h>
#include "config.h"

extern volatile uint16_t tps;

const Configuration DEFAULT_CONFIG PROGMEM = {
  true, 255, 768,
  true, -10, 60,
  true, 0, 50,
  true, 850, 1080,
  600, 4096, 1800
};

Configuration config;

void setLumin(const char* v)        { config.lumin = (strcmp(v,"TRUE")==0); }
void setTempAir(const char* v)      { config.temp_air = (strcmp(v,"TRUE")==0); }
void setHygr(const char* v)         { config.hygr = (strcmp(v,"TRUE")==0); }
void setPressure(const char* v)     { config.pressure = (strcmp(v,"TRUE")==0); }
void setLuminLow(const char* v)     { config.lumin_low = atoi(v); }
void setLuminHigh(const char* v)    { config.lumin_high = atoi(v); }
void setMinTempAir(const char* v)   { config.min_temp_air = atoi(v); }
void setMaxTempAir(const char* v)   { config.max_temp_air = atoi(v); }
void setHygrMint(const char* v)     { config.hygr_mint = atoi(v); }
void setHygrMaxt(const char* v)     { config.hygr_maxt = atoi(v); }
void setPressureMin(const char* v)  { config.pressure_min = atoi(v); }
void setPressureMax(const char* v)  { config.pressure_max = atoi(v); }
void setLogInterval(const char* v)  { config.log_interval = atoi(v); }
void setTimeout(const char* v)      { config.timeout = atoi(v); }
void setFileMaxSize(const char* v)      { config.file_max_size = atoi(v); }

const char CMD_LUMIN[] PROGMEM = "LUMIN=";
const char CMD_TEMP_AIR[] PROGMEM = "TEMP_AIR=";
const char CMD_HYGR[] PROGMEM = "HYGR=";
const char CMD_PRESSURE[] PROGMEM = "PRESSURE=";
const char CMD_LUMIN_LOW[] PROGMEM = "LUMIN_LOW=";
const char CMD_LUMIN_HIGH[] PROGMEM = "LUMIN_HIGH=";
const char CMD_MIN_TEMP_AIR[] PROGMEM = "MIN_TEMP_AIR=";
const char CMD_MAX_TEMP_AIR[] PROGMEM = "MAX_TEMP_AIR=";
const char CMD_HYGR_MINT[] PROGMEM = "HYGR_MINT=";
const char CMD_HYGR_MAXT[] PROGMEM = "HYGR_MAXT=";
const char CMD_PRESSURE_MIN[] PROGMEM = "PRESSURE_MIN=";
const char CMD_PRESSURE_MAX[] PROGMEM = "PRESSURE_MAX=";
const char CMD_LOG_INTERVAL[] PROGMEM = "LOG_INTERVALL=";
const char CMD_TIMEOUT[] PROGMEM = "TIMEOUT=";
const char CMD_FILE_MAX_SIZE[] PROGMEM = "FILE_MAX_SIZE=";

struct CommandEntry {
  const char* name;
  void (*handler)(const char*);
};

const CommandEntry commands[] PROGMEM = {
  {CMD_LUMIN, setLumin},
  {CMD_TEMP_AIR, setTempAir},
  {CMD_HYGR, setHygr},
  {CMD_PRESSURE, setPressure},
  {CMD_LUMIN_LOW, setLuminLow},
  {CMD_LUMIN_HIGH, setLuminHigh},
  {CMD_MIN_TEMP_AIR, setMinTempAir},
  {CMD_MAX_TEMP_AIR, setMaxTempAir},
  {CMD_HYGR_MINT, setHygrMint},
  {CMD_HYGR_MAXT, setHygrMaxt},
  {CMD_PRESSURE_MIN, setPressureMin},
  {CMD_PRESSURE_MAX, setPressureMax},
  {CMD_LOG_INTERVAL, setLogInterval},
  {CMD_TIMEOUT, setTimeout},
  {CMD_FILE_MAX_SIZE, setFileMaxSize}
};

void resetToDefault() {
  memcpy_P(&config, &DEFAULT_CONFIG, sizeof(Configuration));
}

void Config() {
  if (Serial.available()) {
    char buffer[20];
    byte len = Serial.readBytesUntil('\n', buffer, sizeof(buffer)-1);
    buffer[len] = '\0';
    strupr(buffer);
    
    if (strcmp(buffer, "VERSION") == 0) {
      Serial.println(F("Version 1.0 - Lot 2024B"));
    }
    else if (strcmp(buffer, "RESET") == 0) {
      resetToDefault();
      Serial.println(F("OK - Configuration réinitialisée"));
    }
    else {
      bool found = false;
      for (uint8_t i = 0; i < (sizeof(commands)/sizeof(commands[0])); i++) {
        CommandEntry entry;
        memcpy_P(&entry, &commands[i], sizeof(CommandEntry)); // copie depuis PROGMEM

        size_t keylen = strlen_P((PGM_P)entry.name);
        if (strncmp_P(buffer,(PGM_P)entry.name,keylen)==0) {
          const char* value = buffer + keylen;
          entry.handler(value);
          found = true;
          break;
        }
      }
      if (!found) {
        Serial.println(F("ERREUR - Commande non reconnue"));
      }
    TCNT1=0;
    tps=0;
    }
  }
}