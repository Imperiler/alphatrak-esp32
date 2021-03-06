#include "Main.h"


TinyGsm modem(SerialAT);
TinyGsmClient client(modem);


// ------- GSM Functions -----
void modemPowerOn()
{
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  delay(1000); //Datasheet Ton mintues = 1S
//   digitalWrite(PWR_PIN, HIGH);
}


void modemPowerOff()
{
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1500); //Datasheet Ton mintues = 1.2S
//   digitalWrite(PWR_PIN, HIGH);
}


void modemRestart()
{
    modemPowerOff();
    delay(1000);
    modemPowerOn();
}


bool ensureModemNetworkConnected() {
    bool isConnected = modem.isNetworkConnected();  // current connected status
    if (isConnected) {
        return true;
    }
    if (!isConnected) {
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
        // if we are not connected by this point, we might need to reset
        else {
          DEBUG_WARNING_SERIAL.println("warning: could not connect, trying reboot...");
          ESP.restart();
        }
    }
  }
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

    // if (!modem.restart()) {
    //     // modemRestart();
    //     delay(2000);
    //     Serial.println("Failed to restart modem, waiting for restart to finish");
    // }
    modem.restart();


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

    if (!modem.isNetworkConnected())
      ensureModemNetworkConnected();
 

    Serial.println();
    Serial.println("Device is connected .");
    Serial.println();


    // Serial.println("=====Inquiring UE system information=====");
    // modem.sendAT("+CPSI?");
    // if (modem.waitResponse(1000L, res) == 1) {
    //     res.replace(GSM_NL "OK" GSM_NL, "");
    //     Serial.println(res);
    // }

}


void ensureModemGprsConnected() {
    // if we are already connected, break
    if (modem.isGprsConnected())
    { 
        return;
    }

    // if not network connected
    if (!modem.isNetworkConnected())
    {
      ensureModemNetworkConnected();
    }

    if (!modem.isGprsConnected()) {modem.gprsConnect(apn);}
      
}