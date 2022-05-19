#ifndef NETWORKSCANNER_H
#define NETWORKSCANNER_H

#include "ArduinoJson.h"

void setupWifi();
void setupModem();
void modemPowerOff();
void modemPowerOn();
extern ArduinoJson::JsonObject scanWifi();
extern ArduinoJson::JsonObject scanGSM();

#endif