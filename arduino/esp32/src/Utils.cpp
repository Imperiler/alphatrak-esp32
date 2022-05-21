#include "Main.h"
#include "WiFi.h"
#include <EEPROM.h>
#include "ModemUtils.h"
#include "ArduinoJson.h"
#include "driver/adc.h"


// ------- WiFi Functions -----
void setupWifi()
{
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.persistent(false);                   // Supposedly reading and writing WAP info from RAM (default) causes delays and wears down the flash
    delay(1);
    WiFi.mode(WIFI_STA);
    delay(1);
    WiFi.disconnect();
    delay(1);
}

void disableWifi()
{
  WiFi.disconnect(true);
  delay(1);
  WiFi.mode(WIFI_OFF);                        // Switch WiFi off
  delay(1);
  btStop();                                   // Power down BT for best power saving
}

// ------- Power Functions -----
int getSleepTime()
{
  int TIME_TO_SLEEP;

  if (EEPROM.read(0) == 255) {
      TIME_TO_SLEEP = DEFAULT_TIME_TO_SLEEP;
    }

  else {
      TIME_TO_SLEEP = EEPROM.read(0);
    }

  return(TIME_TO_SLEEP);
}


void enterSleep()
{
    int TIME_TO_SLEEP = getSleepTime();
    modemPowerOff();                          // power off modem
    // Set LED OFF
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    // begin sleep sequence
    Serial.println("going to sleep for " + String(TIME_TO_SLEEP) + " seconds...");
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();  
}


// this will return 0 if power provided to the usb-c port, else voltage will be shown
uint16_t measureBatVoltage()
{
  uint32_t in = 0;
  for (int i = 0; i < ADC_BATTERY_LEVEL_SAMPLES; i++)
  {
    in += (uint32_t)analogRead(PIN_BAT_ADC);
  }
  in = (int)in / ADC_BATTERY_LEVEL_SAMPLES;

  uint16_t bat_mv = ((float)in / 4096) * 3600 * 2;
  return(bat_mv);
}


ArduinoJson::JsonObject getDeviceInfo()
{
  DynamicJsonDocument deviceDoc(1024);
  ArduinoJson::JsonObject deviceObject = deviceDoc.to<JsonObject>();

  const String deviceMac = WiFi.macAddress();         // MAC to identify device to the server
  const String deviceModel = "temp";
  const uint16_t batt = measureBatVoltage();

  deviceDoc["device_mac"] = deviceMac;
  deviceDoc["batt"] = batt;


  return(deviceObject);
}