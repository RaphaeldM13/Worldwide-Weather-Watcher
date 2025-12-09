#include <TinyGPS.h>
#include "GPSSens.h"
extern int32_t lat;
extern int32_t lng;

TinyGPS gps;

void readGPS() {
while (SoftSerial.available() > 0) {
  char c = SoftSerial.read();
  if (gps.encode(c)) {
    long lat_tmp, lng_tmp;
    gps.get_position(&lat_tmp, &lng_tmp);
    lat = lat_tmp; // déjà en millionièmes de degré
    lng = lng_tmp;
  }
}

}

bool checkGPS(){
  long lat_tmp, lng_tmp;
  gps.get_position(&lat_tmp, &lng_tmp);
  return (lat_tmp != TinyGPS::GPS_INVALID_ANGLE && lng_tmp != TinyGPS::GPS_INVALID_ANGLE);
}