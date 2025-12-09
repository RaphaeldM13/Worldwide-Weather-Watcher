#include "RTC.h"

uint32_t yymmdd;
RTC_DS1307 rtc; 

void StartRTC()
{
  rtc.begin();
  if(!rtc.isrunning()){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void TakeRTC() {
  DateTime now = rtc.now();
  yymmdd = (now.year() % 100) * 10000UL +now.month() * 100 +now.day();
}

bool checkRTC(){
  if(!rtc.isrunning()) return false;
  return true;
}