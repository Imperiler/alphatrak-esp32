#ifndef NETWORKSCANNER_H
#define NETWORKSCANNER_H

#include "ArduinoJson.h"


// extern char apn;
extern TinyGsm modem;
extern TinyGsmClient client;
extern const char apn[];
void setupModem();
extern ArduinoJson::JsonObject scanWifi();
extern ArduinoJson::JsonObject scanGSM();

#endif