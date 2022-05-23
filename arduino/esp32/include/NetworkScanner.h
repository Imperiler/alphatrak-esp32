#ifndef NETWORKSCANNER_H
#define NETWORKSCANNER_H

#include "ArduinoJson.h"


extern TinyGsm modem;
extern TinyGsmClient client;
extern const char apn[];
extern ArduinoJson::JsonObject scanWifi();
ArduinoJson::JsonObject scanGSM();

#endif