#ifndef MODEM_UTILS_H
#define MODEM_UTILS_H


void setupModem();
void modemPowerOff();
void modemPowerOn();
void modemRestart();
void ensureModemNetworkConnected();
void ensureModemGprsConnected();

#endif