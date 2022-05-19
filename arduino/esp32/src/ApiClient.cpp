#include "ApiClient.h"
#include "WiFi.h"
#include "HTTPClient.h"

const char* serverName = "http://192.168.5.29:8000/api/transmission/";


int postData(String requestBody)
{
//Check WiFi connection status
      if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/json");

      // Data to send with HTTP POST
      // Send HTTP POST request
      int httpResponseCode = http.POST(requestBody);
        
      // Free resources
      http.end();
      return (httpResponseCode);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}