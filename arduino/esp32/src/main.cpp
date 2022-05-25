#include "Main.h"
#include "Utils.h"
#include "ModemUtils.h"
#include "NetworkScanner.h"
#include "TinyGsmClient.h"
#include <EEPROM.h>
#include "ApiClient.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"


void setup()
{
    Serial.begin(115200);                            // initialize serial console
    delay(200);                                      // give board time to init
    DEBUG_INFORMATION_SERIAL.println(                // inform running setup
    "========Setup begin========");

    DEBUG_INFORMATION_SERIAL.println(                // inform clock cpu down
    "========Clock CPU down to " + 
    String(CPU_MHZ) + "Mhz========");
 
    setCpuFrequencyMhz(CPU_MHZ);                     //clock cpu down
    EEPROM.begin(EEPROM_SIZE);                       // initialize eeprom


    DEBUG_INFORMATION_SERIAL.println(
    "========Setup done========");                   // inform setup complete
}


String serializeTransmission()
{
    /** consruct json to send to server **/
    JsonObject deviceJson = getDeviceInfo();
    int deviceByes =  JSON_OBJECT_SIZE(deviceJson);
    
    JsonObject wifiJson = scanWifi();
    int wifiBytes = JSON_OBJECT_SIZE(wifiJson);
    
    /** boot GSM modem and scan **/
    setupModem();  
    JsonObject gsmJson = scanGSM();
    int gsmBytes = JSON_OBJECT_SIZE(gsmBytes);

    int payloadBytes = (deviceByes + wifiBytes + gsmBytes + 100);
   
    DynamicJsonDocument transmitData(payloadBytes);

   
    transmitData["device"]  = deviceJson;
    JsonObject scanResultsArray = transmitData.createNestedObject("scan_results");
    scanResultsArray["wifi_networks"] = wifiJson;
    scanResultsArray["cell_tower"] = gsmJson;


    String requestBody;                              // initialize request body
    serializeJson(transmitData, requestBody);        // json serialize result into request body
    serializeJsonPretty(transmitData, Serial);
    return (requestBody);
}


int postTransmission(String requestBody)
{
    int httpResponseCode = postData(requestBody);    // post data and store response code
     
    if (httpResponseCode == 200) {                   // if we get server okay, clear the document
          modemPowerOff();
    }
    else {                                           // if we don't get server 200, wait and try re-submit
          DEBUG_WARNING_SERIAL.println("Transmission failed, trying again");
          delay(10);
          httpResponseCode = postData(requestBody);
          // print error to serial if failure occured
          if (!(httpResponseCode == 200)) {DEBUG_ERROR_SERIAL.println(
             "Error: could not submit transmission data");
          }
    }
    return httpResponseCode;
}

void loop()
{
    postTransmission(serializeTransmission());       // gather and post json to server
    enterSleep();                                    // go to sleep
}