#include <WiFiS3.h>  // WiFi library for the UNO R4 WiFi
#include "WiFiSSLClient.h"
#include "IPAddress.h"
#include "ArduinoJson.h"


char ssid[] = "Pig";           // Your WiFi network SSID
char pass[] = "TommyInnit72";  // Your WiFi network password
const char* description;
float temp;

void setup() {
  Serial.begin(9600);
  delay(5000);


  while (!Serial)
    ;
  // Connect to WiFi network
  Serial.print("Attempting to connect to network: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected to WiFi");

  // Fetch JSON data
  fetchJSON();
}

void loop() {
  // Nothing to do here
}

void fetchJSON() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi.");
  }else{
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
}

  WiFiSSLClient client; // Change to WiFiClientSecure for HTTPS


  const char server[] = "api.weatherbit.io";
  const char* resource = "/v2.0/current?lat=-45.8741&lon=170.5036&key=4f42db47030e4ba89eb302bc1274db51";

  if (client.connect(server, 443)) {  // Use port 443 for HTTPS
    client.print("GET ");
    client.print(resource);
    client.println(" HTTP/1.1");
    client.println("Host: api.weatherbit.io");
    client.println("Connection: close");
    client.println();  // End of HTTP headers

    // Read response
    bool headerPassed = false;
    String jsonResponse = "";
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      // Check if we reached the end of the headers
      if (line == "\r") {
        headerPassed = true;
      }
      // If headers are passed, concatenate JSON data
      if (headerPassed && line != "\r") {
        jsonResponse += line;
      }
    }

    // Parse JSON response
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, jsonResponse);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Extract data from JSON response
    description = doc["data"][0]["weather"]["description"];
    temp = doc["data"][0]["temp"];

    // Print extracted data
    Serial.print("Description: ");
    Serial.println(description);
    Serial.print("Temperature: ");
    Serial.println(temp);

    // Close connection
    client.stop();
  } else {
    Serial.println("Connection failed");
    Serial.println(client.getWriteError());
  }
}
