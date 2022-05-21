#include "Main.h"
#include "Utils.h"
#include "ModemUtils.h"
#include "TinyGsmClient.h"
#include "NetworkScanner.h"
#include "WiFi.h"
#include <ArduinoHttpClient.h>



ArduinoJson::JsonObject scanWifi()
{
    setupWifi();
    DynamicJsonDocument doc(1024);
    DynamicJsonDocument wifiDoc(1024);

    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    // serialize network info
    ArduinoJson::JsonObject scanObject = wifiDoc.to<JsonObject>();
    ArduinoJson::JsonArray scanArray = doc.to<JsonArray>();    

    if (n == 0) {  // if no networks found, inform
        Serial.println("no networks found");
        return(scanObject);
    } 
    else {  
        Serial.print(n);
        Serial.println(" networks found");
    

        for (int i = 0; i < n; ++i) {
            StaticJsonDocument<400> objectDoc;

            ArduinoJson::JsonObject networkObjct = objectDoc.to<JsonObject>();

            networkObjct["ssid"] = (WiFi.SSID(i));
            networkObjct["macAddress"] = WiFi.BSSIDstr(i);
            networkObjct["signalStrength"] = WiFi.RSSI(i);
            networkObjct["channel"] = WiFi.channel(i);


            scanArray.add(networkObjct);
        }
    scanObject["wifi_networks"] = scanArray;
    disableWifi();          // disable wifi for power saving
    return(scanObject);
    }
}


ArduinoJson::JsonObject scanGSM()
{
    ensureModemGprsConnected();
    DynamicJsonDocument doc(1024);
    DynamicJsonDocument gsmDoc(1024);

    Serial.println("gsm scan start");


    float lat      = 0;
    float lon      = 0;
    float accuracy = 0;
    int   year     = 0;
    int   month    = 0;
    int   day      = 0;
    int   hour     = 0;
    int   min      = 0;
    int   sec      = 0;
    for (int8_t i = 15; i; i--) {
      Serial.println("Requesting current GSM location");
      if (modem.getGsmLocation(&lon, &lat, &accuracy, &year, &month, &day, &hour,
                             &min, &sec)) {
        Serial.println("Latitude:" + String(lat, 8) + "\tLongitude:" + String(lon, 8));
        Serial.println("Accuracy:" + String(accuracy));
        Serial.println("Year:" + String(year) + "\tMonth:" + month + "\tDay:" + day);
        Serial.println("Hour:" + String(hour) + "\tMinute:" + min + "\tSecond:" + sec);
      break;
    } else {
      DBG("Couldn't get GSM location, retrying in 15s.");
      delay(15000L);
    }
  }
}

// ArduinoJson::JsonObject scanTower()
// {
//     // char res[];   // need to actually calculate max size and allocate based on that
//     Serial.println("=====Inquiring UE system information=====");
//     modem.sendAT("+CPSI?");
//     if (modem.waitResponse(1000L) == 1) {
//         String res1 = SerialAT.readString();
//         // char buf[100];
//         // String res = Serial1.readStringUntil('\n');
//         // int rlen = Serial1.readBytesUntil('\n', buf, 100);
        
//         // prints the received data
//         Serial.print("I received: ");
//         Serial.println(res1);
//         // for(int i = 0; i < rlen; i++)
//         //   Serial.print(buf[i]);
//         }
//     }

String scanTower()
{
    String res;   // need to actually calculate max size and allocate based on that
    Serial.println("=====Inquiring UE system information=====");
    modem.sendAT("+CPSI?");
    if (modem.waitResponse(1000L, res) == 1) {
        res.replace(GSM_NL "OK" GSM_NL, "");
        res.replace("+CPSI: ", "");
        Serial.println(res);
        return (res);
    }
}