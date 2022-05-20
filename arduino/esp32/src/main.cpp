#include "Main.h"
#include "Utils.h"
#include "NetworkScanner.h"
#include "TinyGsmClient.h"
#include <EEPROM.h>
#include "ApiClient.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;                     // Set timer to 5 seconds (5000)


#define EEPROM_SIZE 1

int TIME_TO_SLEEP;                                   // instantiate sleep time var


void setup()
{
    Serial.begin(115200);                            // initialize serial console
    delay(7000);                                     // give serial console time to init
    Serial.println("running setup");                 // inform running setup

    // get and set preferences
    EEPROM.begin(EEPROM_SIZE);                       //initialize eeprom
    if (EEPROM.read(0) == NAN) {
        TIME_TO_SLEEP = DEFAULT_TIME_TO_SLEEP;
      }

    else {
        TIME_TO_SLEEP = EEPROM.read(0);
      }

    setupModem();
    delay(6000);

    Serial.println("Setup done");
}


void loop()
{

    // scanGSM();
    // consruct json to send to server
    DynamicJsonDocument doc(1024);                   // initialize json doc
    JsonObject transmitData = doc.to<JsonObject>();  // make doc into JSON object
    transmitData["device"] = deviceId;               // device ID for server
    transmitData["scan_results"] = scanWifi();       // get wifi results

 
    Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

    if ((millis() - lastTime) > timerDelay) {

      String requestBody;                           // initialize request body
      serializeJson(doc, requestBody);              // json serialize result into request body
      // serializeJsonPretty(doc, Serial);             // TEMP print json to console
      
      int httpResponseCode = postData(requestBody); // post data and store response code
     
      // Serial.print("HTTP Response code: ");
      // Serial.println(httpResponseCode);
      if (httpResponseCode == 200) {               // if we get server okay, go to sleep step
          doc.clear();
          // set to sleep for 5 secs
          // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
          // esp_deep_sleep_start();  
      }
      else {                                      // if we don't get server 200, wait and try re-submit
          delay(500);
          postData(requestBody);
          doc.clear();

          // set to sleep for 5 secs
          // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
          // esp_deep_sleep_start();  
      }
        
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    delay(500);
}