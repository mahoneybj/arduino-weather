#include <WiFiS3.h>  // WiFi library for the UNO R4 WiFi

char ssid[] = "Pig";           // Your WiFi network SSID
char pass[] = "TommyInnit72";  // Your WiFi network password

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
  WiFiClient client;

  // IP address of the server you want to fetch JSON data from
  const char* server = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/dunedin?unitGroup=metric&key=Y9WFKCES5ASSNTRYHU347ZTC3&contentType=json";
  const char* resource = "/data.json";

  if (client.connect(server, 443)) {  // Use port 443 for HTTPS
    client.print("GET ");
    client.print(resource);
    client.println(" HTTP/1.1");
    client.println("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();  // End of HTTP headers
  } else {
    Serial.println("Connection failed");
  }

}