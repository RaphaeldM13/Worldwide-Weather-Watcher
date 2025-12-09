#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include "RTClib.h"

extern uint32_t yymmdd;

void StartRTC();
void TakeRTC();
bool checkRTC();

#endif