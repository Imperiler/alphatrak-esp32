#define SerialMon Serial  // set debug console to serial monitor
#define SerialAT Serial1  // set serial at commands to sim module
#define TINY_GSM_MODEM_SIM7000
// #define TINY_GSM_DEBUG SerialMon
#define SerialAT Serial1  // set serial at commands to sim module
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

#include "NetworkScanner.h"
#include "WiFi.h"
// #include "TinyGsmClient.h"
#include <ArduinoHttpClient.h>
#include "TinyGSM.h"
#include "TinyGsmClientSIM7000.h"
// #include <TinyGsmClient.h>

#define TINY_GSM_DEBUG SerialMon
#define TINY_GSM_USE_GPRS true

#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
// TinyGsm        modem(SerialAT);
TinyGsmClient client(modem);  // TinyGSM Client for Internet connection

const char apn[] = "fast.t-mobile.com";
// const char apn[] = "epc.tmobile.com";

const int  port = 443;
const char server[]   = "gorest.co.in";
const char resource[] = "/public/v2/users/100";

HttpClient    http(client, server, port);


void setupWifi()
{
        // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}

void setupModem()
{
    // HTTPClient http;

    // Set GSM module baud rate, auto for testing, modem.set for production
    TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
    // modem.setBaud(115200);
    SerialMon.println("Initializing modem...");
    // modem.restart();
    modem.init();
    String name = modem.getModemName();
    String modemInfo = modem.getModemInfo();

    SerialMon.print("Modem name: ");
    SerialMon.println(name);

    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

    SerialMon.print("Sim Status: ");
    SerialMon.println(modem.getSimStatus());

    SerialMon.print("Sim CCID: ");
    SerialMon.println(modem.getSimCCID());


    SerialMon.println("waiting for network registration");
    modem.waitForNetwork(600000L);

    SerialMon.println("Connecting to " + (String)apn);

    if (!modem.isGprsConnected()) {
        SerialMon.println("Connecting to network");
        modem.gprsConnect(apn);
    }


    bool res = modem.isGprsConnected();
    String status = ("GPRS status:", res ? "connected" : "not connected");
    SerialMon.println(status);

    if (modem.isNetworkConnected()) { SerialMon.println("Network connected"); }

    SerialMon.print("IEMI is ");
    SerialMon.println(modem.getIMEI());

  String ccid = modem.getSimCCID();
  DBG("CCID:", ccid);

  String imei = modem.getIMEI();
  DBG("IMEI:", imei);

  String imsi = modem.getIMSI();
  DBG("IMSI:", imsi);

  String cop = modem.getOperator();
  DBG("Operator:", cop);

  IPAddress local = modem.localIP();
  DBG("Local IP:", local);

  int csq = modem.getSignalQuality();
  DBG("Signal quality:", csq);


}

ArduinoJson::JsonObject scanWifi()
{
    DynamicJsonDocument doc(1024);
    DynamicJsonDocument wifiDoc(1024);

    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {  // if no networks found, inform
        Serial.println("no networks found");
    } 
    else {  
        Serial.print(n);
        Serial.println(" networks found");
               
        
        // serialize network info
        ArduinoJson::JsonObject scanObject = wifiDoc.to<JsonObject>();
        ArduinoJson::JsonArray scanArray = doc.to<JsonArray>();
        ArduinoJson::JsonObject wifi_network = scanArray.createNestedObject();


        for (int i = 0; i < n; ++i) {
            // wifi_network["ssid"] = (WiFi.SSID(i));
            StaticJsonDocument<400> objectDoc;

            // ArduinoJson::JsonArray networks = wifi_network.createNestedArray();
            ArduinoJson::JsonObject networkObjct = objectDoc.to<JsonObject>();

            networkObjct["ssid"] = (WiFi.SSID(i));
            networkObjct["mac"] = WiFi.BSSIDstr(i);
            networkObjct["channel"] = WiFi.channel(i);
            networkObjct["strength"] = WiFi.RSSI(i);
            scanArray.add(networkObjct);

            scanObject["wifi_networks"] = scanArray;
            delay(10);
        }
        return(scanObject);
    }
}


ArduinoJson::JsonObject scanGSM()
{
    DynamicJsonDocument doc(1024);
    DynamicJsonDocument gsmDoc(1024);

    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {  // if no networks found, inform
        Serial.println("no networks found");
    } 
    else {  
        Serial.print(n);
        Serial.println(" networks found");
               
        
        // serialize network info
        ArduinoJson::JsonObject scanObject = gsmDoc.to<JsonObject>();
        ArduinoJson::JsonArray scanArray = doc.to<JsonArray>();
        ArduinoJson::JsonObject wifi_network = scanArray.createNestedObject();

        for (int i = 0; i < n; ++i) {
            // wifi_network["ssid"] = (WiFi.SSID(i));
            StaticJsonDocument<400> objectDoc;

            // ArduinoJson::JsonArray networks = wifi_network.createNestedArray();
            ArduinoJson::JsonObject networkObjct = objectDoc.to<JsonObject>();

            networkObjct["ssid"] = (WiFi.SSID(i));
            networkObjct["mac"] = WiFi.BSSIDstr(i);
            networkObjct["channel"] = WiFi.channel(i);
            networkObjct["strength"] = WiFi.RSSI(i);
            scanArray.add(networkObjct);

            scanObject["wifi_networks"] = scanArray;
            delay(10);
        }
        return(scanObject);
    }
}
