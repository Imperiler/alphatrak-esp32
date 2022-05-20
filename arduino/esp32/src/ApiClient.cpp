#include "Main.h"
#include "ApiClient.h"
#include "WiFi.h"
#include "TinyGsmClient.h"
#include <ArduinoHttpClient.h>

const char server[] = "7b2f9d63a2f5.ngrok.io";
const int port = 80;



int postData(String requestBody)
{        
    HttpClient http(client, server, port);
    Serial.println("----------------------SUBMITTING REQUEST-------------------");
   
    //Check WiFi connection status
    if (!modem.isGprsConnected()) { 
        SerialMon.println("NOT GPRS connected");
        modem.gprsConnect(apn);
    }

    // http.connect();
    client.connect(server,port);

    if (client.connect(server, port)) {
        Serial.println("connected");
        http.beginRequest();
        http.post("/api/transmission/");
        http.sendHeader("Content-Type", "application/json");
        http.sendHeader("Content-Length", requestBody.length());
        http.beginBody();
        http.print(requestBody);
        http.endRequest();

        // String contentType = ("application/json");
        // Serial.print("request looks like this: ");
        // Serial.println(requestBody);

        // read the status code and body of the response
        int statusCode = http.responseStatusCode();
        String response = http.responseBody();
        Serial.println(response);
        http.stop();
        return (statusCode);
    }

    else {
            Serial.println("I'm in danger lol");
    }
}