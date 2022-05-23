#ifndef UTILS_H
#define UTILS_H

#include "ArduinoJson.h"

extern TinyGsm modem;

void setupWifi();
void enterSleep();
void disableWifi();
uint16_t measureBatVoltage();
int getSleepTime();
String getDeviceMac();
extern ArduinoJson::JsonObject getDeviceInfo();

#endif