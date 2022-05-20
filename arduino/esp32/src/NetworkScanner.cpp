#include "Main.h"
#include "Utils.h"
#include "TinyGsmClient.h"
#include "NetworkScanner.h"
#include "WiFi.h"
#include <ArduinoHttpClient.h>

void setupModem()
{
    delay(10);

    // Set LED OFF
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    modemPowerOn();

    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    String res;

    while (!modem.init()) {
        modemRestart();
        delay(2000);
        Serial.println("Failed to restart modem, waiting for restart to finish");
    }

    Serial.println("========SIMCOMATI======");
    modem.sendAT("+SIMCOMATI");
    modem.waitResponse(1000L, res);
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
    res = "";
    Serial.println("=======================");

    Serial.println("=====Preferred mode selection=====");
    modem.sendAT("+CNMP?");
    if (modem.waitResponse(1000L, res) == 1) {
        res.replace(GSM_NL "OK" GSM_NL, "");
        Serial.println(res);
    }
    res = "";
    Serial.println("=======================");


    Serial.println("=====Preferred selection between CAT-M and NB-IoT=====");
    modem.sendAT("+CMNB?");
    if (modem.waitResponse(1000L, res) == 1) {
        res.replace(GSM_NL "OK" GSM_NL, "");
        Serial.println(res);
    }
    res = "";
    Serial.println("=======================");


    String name = modem.getModemName();
    Serial.println("Modem Name: " + name);

    String modemInfo = modem.getModemInfo();
    Serial.println("Modem Info: " + modemInfo);

    // Unlock SIM card with a PIN if needed
    if ( GSM_PIN && modem.getSimStatus() != 3 ) {
        modem.simUnlock(GSM_PIN);
    }


    for (int i = 0; i <= 4; i++) {
        uint8_t network[] = {
            2,  /*Automatic*/
            13, /*GSM only*/
            38, /*LTE only*/
            51  /*GSM and LTE only*/
        };
        Serial.printf("Try %d method\n", network[i]);
        modem.setNetworkMode(network[i]);
        delay(3000);
        bool isConnected = false;
        int tryCount = 60;
        while (tryCount--) {
            int16_t signal =  modem.getSignalQuality();
            Serial.print("Signal: ");
            Serial.print(signal);
            Serial.print(" ");
            Serial.print("isNetworkConnected: ");
            isConnected = modem.isNetworkConnected();
            Serial.println( isConnected ? "CONNECT" : "NO CONNECT");
            if (isConnected) {
                break;
            }
            delay(1000);
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        }
        if (isConnected) {
            break;
        }
    }
    // Turn on LED when connected
    digitalWrite(LED_PIN, HIGH);

    Serial.println();
    Serial.println("Device is connected .");
    Serial.println();


    Serial.println("=====Inquiring UE system information=====");
    modem.sendAT("+CPSI?");
    if (modem.waitResponse(1000L, res) == 1) {
        res.replace(GSM_NL "OK" GSM_NL, "");
        Serial.println(res);
    }

}


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
    return(scanObject);
    }
}


ArduinoJson::JsonObject scanGSM()
{
    if (!modem.isGprsConnected()) { SerialMon.println("NOT GPRS connected"); }

    modem.gprsConnect(apn);
    
    if (modem.isGprsConnected()) { SerialMon.println("GPRS connected"); }
    if (!modem.isGprsConnected()) { SerialMon.println("NOT GPRS connected"); }
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