#ifndef MAIN_H
#define MAIN_H

#define SerialMon Serial                          // set debug console to serial monitor
#define SerialAT Serial1                          // set serial at commands to sim module
#define TINY_GSM_MODEM_SIM7000                    // define modem type
#define TINY_GSM_DEBUG SerialMon
#define TINY_GSM_RX_BUFFER 1024                   // Set RX buffer to 1Kb

// setup esp32 pins
#define PWR_PIN             4
#define UART_BAUD           9600
#define PIN_DTR             25
#define PIN_TX              27
#define PIN_RX              26

#define SD_MISO             2
#define SD_MOSI             15
#define SD_SCLK             14
#define SD_CS               13
#define LED_PIN             12

#define GSM_PIN ""

#define uS_TO_S_FACTOR 1000000                    /* Conversion factor for micro seconds to seconds */ 
#define DEFAULT_TIME_TO_SLEEP  5                  /* Time ESP32 will go to sleep (in miliseconds) */

#include "WiFi.h"
#include <ArduinoHttpClient.h>
#include "TinyGsmClient.h"


const String deviceId =   WiFi.macAddress();       // MAC to identify device to the server
const char apn[] = "fast.t-mobile.com";



#endif