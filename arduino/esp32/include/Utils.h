#ifndef UTILS_H
#define UTILS_H

#include "ArduinoJson.h"


void setupWifi();
void enterSleep();
void disableWifi();
uint16_t measureBatVoltage();
int getSleepTime();
extern ArduinoJson::JsonObject getDeviceInfo();

#endif