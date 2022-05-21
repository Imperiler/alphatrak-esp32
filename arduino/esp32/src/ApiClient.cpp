#include "Main.h"
#include "ApiClient.h"
#include "TinyGsmClient.h"
#include <ArduinoHttpClient.h>
#include "Utils.h"
#include "ModemUtils.h"


int postData(String requestBody)
{        
    HttpClient http(client, server, port);
    Serial.println("----------------------SUBMITTING REQUEST-------------------");
    ensureModemGprsConnected();

    client.connect(server,port);

    if (client.connect(server, port)) {
        Serial.println("connected to server");
        http.beginRequest();                                            // start transmission
        http.post(resource);                                            // specify server location to post to
        // send headers
        http.sendHeader("Content-Type", "application/json");
        http.sendHeader("Content-Length", requestBody.length());
        http.beginBody();
        // send body
        http.print(requestBody);
        http.endRequest();

        // read the status code and body of the response
        int statusCode = http.responseStatusCode();
        String response = http.responseBody();
        Serial.println(response);
        http.stop();
        return (statusCode);
    }

    else {
            Serial.println("I'm in danger lol server not connected");
    }
}