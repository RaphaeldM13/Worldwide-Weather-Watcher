#ifndef CONFIG_H
#define CONFIG_H

// --- Structure de configuration ---
struct Configuration {
  bool lumin;
  uint16_t lumin_low;
  uint16_t lumin_high;
  bool temp_air;
  int8_t min_temp_air;
  int8_t max_temp_air;
  bool hygr;
  uint8_t hygr_mint;
  uint8_t hygr_maxt;
  bool pressure;
  uint16_t pressure_min;
  uint16_t pressure_max;
  uint16_t log_interval;
  uint16_t file_max_size;
  uint16_t timeout;
};

// --- Variable globale de configuration ---
// (elle est définie dans Config.cpp)
extern Configuration config;

// --- Fonctions accessibles depuis ton .ino ---
void resetToDefault();
void Config();

#endif 