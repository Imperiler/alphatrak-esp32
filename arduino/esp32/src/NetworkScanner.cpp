#define SerialMon Serial  // set debug console to serial monitor
#define SerialAT Serial1  // set serial at commands to sim module

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
// #define TINY_GSM_DEBUG SerialMon
#define SerialAT Serial1  // set serial at commands to sim module

#define GSM_PIN ""

const char apn[] = "fast.t-mobile.com";


#include "TinyGsmClient.h"
#include "NetworkScanner.h"
#include "WiFi.h"
#include <ArduinoHttpClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

#define UART_BAUD           9600
#define PIN_DTR             25
#define PIN_TX              27
#define PIN_RX              26
#define PWR_PIN             4

#define SD_MISO             2
#define SD_MOSI             15
#define SD_SCLK             14
#define SD_CS               13
#define LED_PIN             12




const int  port = 443;
const char server[]   = "gorest.co.in";
const char resource[] = "/public/v2/users/100";

TinyGsmClient client(modem);
// HttpClient    http(client, server, port);


void setupWifi()
{
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}


void modemPowerOn()
{
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1000); //Datasheet Ton mintues = 1S
  digitalWrite(PWR_PIN, HIGH);
}

void modemPowerOff()
{
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1500); //Datasheet Ton mintues = 1.2S
  digitalWrite(PWR_PIN, HIGH);
}


void modemRestart()
{
    modemPowerOff();
    delay(1000);
    modemPowerOn();
}


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
