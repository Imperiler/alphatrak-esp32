#include "Main.h"
#include "Utils.h"
#include "ModemUtils.h"
#include "NetworkScanner.h"
#include "TinyGsmClient.h"
#include <EEPROM.h>
#include "ApiClient.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"


DynamicJsonDocument doc(1024);                       // initialize json doc


void setup()
{
    Serial.begin(115200);                            // initialize serial console
    delay(200);                                      // give board time to init
    Serial.println("running setup");                 // inform running setup
    setCpuFrequencyMhz(160);                         //clock cpu down

    EEPROM.begin(EEPROM_SIZE);                       // initialize eeprom

    setupModem();                                    // make modem ready

    Serial.println("Setup done");
}


String serializeTransmission()
{
    // consruct json to send to server
    JsonObject transmitData = doc.to<JsonObject>();  // make doc into JSON object

    transmitData["device"] = getDeviceInfo();        // device info
    transmitData["scan_results"] = scanWifi();       // get wifi results

    String requestBody;                              // initialize request body
    serializeJson(doc, requestBody);                 // json serialize result into request body
    serializeJsonPretty(doc, Serial);
    return (requestBody);
}


int postTransmission()
{
    String requestBody = serializeTransmission();   
    int httpResponseCode = postData(requestBody);    // post data and store response code
     
    // Serial.print("HTTP Response code: ");
    // Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {                   // if we get server okay, clear the document
          doc.clear();
    }
    else {                                           // if we don't get server 200, wait and try re-submit
          delay(10);
          postData(requestBody);
          doc.clear();
    }
}

void loop()
{
    // scanGSM();
    postTransmission();                              // gather and post json to server

    Serial.println("trying to get operator...");
    scanTower();
    
    // go to sleep
    enterSleep();    
}