#include "Main.h"
#include "WiFi.h"


TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

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