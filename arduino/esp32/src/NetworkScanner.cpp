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

    DEBUG_INFORMATION_SERIAL.println("scan wifi start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    DEBUG_INFORMATION_SERIAL.println("scan done");
    // serialize network info
    ArduinoJson::JsonObject scanObject = wifiDoc.to<JsonObject>();
    ArduinoJson::JsonArray scanArray = doc.to<JsonArray>();    

    if (n == 0) {  // if no networks found, inform
        DEBUG_WARNING_SERIAL.println("no networks found");  // warn if no networks found
        return(scanObject);
    } 
    else {  
        DEBUG_INFORMATION_SERIAL.print(n);
        DEBUG_INFORMATION_SERIAL.println(" networks found");

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


ArduinoJson::JsonObject scanGSMRoughLocation()
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


String getGsmInfo()
{
    String res;   // need to actually calculate max size and allocate based on that
    Serial.println("=====Inquiring UE system information=====");
    modem.sendAT("+CPSI?");
    if (modem.waitResponse(1000L, res) == 1) {
        DEBUG_INFORMATION_SERIAL.println(res);

        res.replace(GSM_NL "OK" GSM_NL, "");
        res.replace("+CPSI: ", "");

        return(res);
    }
}


ArduinoJson::JsonObject scanGSM()
{
  String res = getGsmInfo();
  int str_len = res.length() + 1;
  char resChar[str_len];
  res.toCharArray(resChar, str_len);
  char *saveptr;
  
  DEBUG_INFORMATION_SERIAL.print("full string: ");
  DEBUG_INFORMATION_SERIAL.print(resChar);

  // read all info into vars
  char  *systemMode       = strtok_r(resChar, ",", &saveptr); 
  char  *operationMode    = strtok_r(NULL, ",", &saveptr);
  char  *mcc              = strtok_r(NULL, "-", &saveptr);
  char  *mnc              = strtok_r(NULL, ",", &saveptr);
  char  *tac              = strtok_r(NULL, ",", &saveptr);
  char  *cellId           = strtok_r(NULL, ",", &saveptr);
  char  *pCellId          = strtok_r(NULL, ",", &saveptr);
  char  *freqBand         = strtok_r(NULL, ",", &saveptr);
  char  *earfcn           = strtok_r(NULL, ",", &saveptr);
  char  *dlbw             = strtok_r(NULL, ",", &saveptr);
  char  *ulbw             = strtok_r(NULL, ",", &saveptr);
  char  *rsrq             = strtok_r(NULL, ",", &saveptr);
  char  *rsrp             = strtok_r(NULL, ",", &saveptr);
  char  *rssi             = strtok_r(NULL, ",", &saveptr);
  char  *rssnr            = strtok_r(NULL, ",", &saveptr);

  uint64_t dec_tac = *tac;


  DEBUG_INFORMATION_SERIAL.print("systemMode: ");
  DEBUG_INFORMATION_SERIAL.println(systemMode);
  DEBUG_INFORMATION_SERIAL.print("operationMode: ");
  DEBUG_INFORMATION_SERIAL.println(operationMode);
  DEBUG_INFORMATION_SERIAL.println("mcc: ");
  DEBUG_INFORMATION_SERIAL.println(mcc);
  DEBUG_INFORMATION_SERIAL.println("mnc: ");
  DEBUG_INFORMATION_SERIAL.println(mnc);
  DEBUG_INFORMATION_SERIAL.println("fixed tac: ");
  DEBUG_INFORMATION_SERIAL.println(dec_tac);






  DynamicJsonDocument towerDoc(1024);                       // initialize json doc
  ArduinoJson::JsonObject towerObject = towerDoc.to<JsonObject>();

  towerObject["cellId"] = cellId;
  towerObject["locationAreaCode"] = tac;
  towerObject["mobileCountryCode"] = mcc; 
  towerObject["mobileNetworkCode"] = mnc;
  towerObject["signalStrength"] = rssi;

  return(towerObject);

}