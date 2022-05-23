#include "Main.h"
#include "ApiClient.h"
#include "TinyGsmClient.h"
#include <ArduinoHttpClient.h>
#include "Utils.h"
#include "ModemUtils.h"


int postData(String requestBody)
{        
    HttpClient http(client, server, port);
    DEBUG_INFORMATION_SERIAL.println("========Submitting request========");
    ensureModemGprsConnected();

    client.connect(server,port);

    if (client.connect(server, port)) {
        DEBUG_INFORMATION_SERIAL.println("succesfully connected to server");
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
        DEBUG_INFORMATION_SERIAL.println(response);
        http.stop();
        return (statusCode);
    }

    else {
        DEBUG_ERROR_SERIAL.println("========json post failed and we're all in danger lol========");
    }
}