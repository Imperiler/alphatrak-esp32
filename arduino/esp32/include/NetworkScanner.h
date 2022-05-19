#ifndef NETWORKSCANNER_H
#define NETWORKSCANNER_H

#include "ArduinoJson.h"

void setupWifi();
void setupModem();
extern ArduinoJson::JsonObject scanWifi();
extern ArduinoJson::JsonObject scanGSM();

#endif