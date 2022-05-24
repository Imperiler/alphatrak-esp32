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
    DynamicJsonDocument transmitData(2048);
    transmitData["device"]  = getDeviceInfo();  
    JsonObject scanResultsArray = transmitData.createNestedObject("scan_results");
    scanResultsArray["wifi_networks"] = scanWifi();
   
   
    /** boot GSM modem and scan **/
    setupModem();  
    scanResultsArray["cell_tower"] = scanGSM();


    String requestBody;                              // initialize request body
    serializeJson(transmitData, requestBody);        // json serialize result into request body
    serializeJsonPretty(transmitData, Serial);
    return (requestBody);
}


int createTransmission()
{
    String requestBody = serializeTransmission();   
    int httpResponseCode = postData(requestBody);    // post data and store response code
     
    if (httpResponseCode == 200) {                   // if we get server okay, clear the document
          doc.clear();
          modemPowerOff();
    }
    else {                                           // if we don't get server 200, wait and try re-submit
          DEBUG_WARNING_SERIAL.println("Transmission failed, trying again");
          delay(10);
          httpResponseCode = postData(requestBody);
          doc.clear();                               // likely don't need to explicitly clear doc, as it should be deleted once out of scope
          // print error to serial if failure occured
          if (!(httpResponseCode == 200)) {DEBUG_ERROR_SERIAL.println(
             "Error: could not submit transmission data");
          }
    }
    return httpResponseCode;
}

void loop()
{
    createTransmission();                            // gather and post json to server

    Serial.println("trying to get operator...");
    scanGSM();

    // go to sleep
    enterSleep();    
}