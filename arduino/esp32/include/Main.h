#ifndef MAIN_H
#define MAIN_H


// ----------------------------- define serial -----------------------------
#define SerialMon Serial                          // set debug console to serial monitor
#define SerialAT Serial1                          // set serial at commands to sim module
#define TINY_GSM_MODEM_SIM7000                    // define modem type
#define TINY_GSM_DEBUG SerialMon
#define TINY_GSM_RX_BUFFER        1024            // Set RX buffer to 1Kb


// ----------------------------- esp32 pins -----------------------------
#define PWR_PIN                   4
#define UART_BAUD                 9600
#define PIN_DTR                   25
#define PIN_TX                    27
#define PIN_RX                    26

#define SD_MISO                   2
#define SD_MOSI                   15
#define SD_SCLK                   14
#define SD_CS                     13
#define LED_PIN                   12
#define PIN_BAT_ADC               35
#define ADC_BATTERY_LEVEL_SAMPLES 100

#define GSM_PIN ""


// ----------------------------- time vars -----------------------------
#define uS_TO_S_FACTOR            1000000         // Conversion factor for micro seconds to seconds
#define DEFAULT_TIME_TO_SLEEP     30              // Time ESP32 will go to sleep (in miliseconds)


// ----------------------------- storage vars -----------------------------
#define EEPROM_SIZE               1


// ----------------------------- includes ---------------------------------
#include "WiFi.h"
#include <ArduinoHttpClient.h>
#include "TinyGsmClient.h"


// ----------------------------- network vars -----------------------------

const char apn[] = "fast.t-mobile.com";            // APN GSM module setting

const char server[] = "684026405456.ngrok.io";     // server to connect to
const char resource[] = "/api/transmission/";      // server location to post to
const int port = 80;                               // port to connect to


#endif