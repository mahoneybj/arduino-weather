#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiS3.h> 
#include "WiFiSSLClient.h"
#include "IPAddress.h"
#include "ArduinoJson.h"
#include "Arduino_LED_Matrix.h"
#include <stdint.h>
#include "symbols.h"

ArduinoLEDMatrix matrix;
LiquidCrystal_I2C lcd(0x27,  16, 2);
char ssid[] = "Pig";           // Your WiFi network SSID
char pass[] = "TommyInnit72";  // Your WiFi network password
const int buttonPinH = 2; 
const int buttonPinD = 3; 
int buttonStateH = 0;
int buttonStateD = 0;

#define LED_PIN 6
#define LED_COUNT 65
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ400);

JsonArray dataHour;
JsonArray dataDaily;

void setup() {
  Serial.begin(9600);
  matrix.begin();
  delay(5000);

  strip.begin();
  strip.show();

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
  matrix.loadFrame(wifi);
  lcd.init();
  lcd.backlight();
  strip.setPixelColor(11, 255, 0, 255);
  strip.show();

  currentWeather();
}

void loop() {
  buttonStateH = digitalRead(buttonPinH);
  buttonStateD = digitalRead(buttonPinD);

  if (buttonStateH == HIGH) {
    fetchHour();
  }
  
  if(buttonStateD == HIGH) {
    fetchDaily();
  }
}

void fetchHour() {
  WiFiSSLClient client; 

  const char server[] = "api.weatherbit.io";
  const char* resource = "/v2.0/forecast/hourly?lat=-45.8741&lon=170.5036&key=dd2f30b33a644bac846c95daa1975889&hours=6";

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
    DynamicJsonDocument doc(1024); // Adjust the size as per your JSON response size
    DeserializationError error = deserializeJson(doc, jsonResponse);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Extract data from JSON response
    dataHour = doc["data"];
    for (int i = 0; i < dataHour.size(); i++) {
      JsonObject hour = dataHour[i];
      const char* description = hour["weather"]["description"];
      float temp = hour["temp"];
      // Extract time from datetime
      String time = hour["timestamp_local"].as<String>().substring(11);

      int icon = hour["weather"]["code"];
      // Print or store the extracted data for each hour
      Serial.print(icon);
      Serial.print("Time: ");
      Serial.println(time);
      Serial.print("Description: ");
      Serial.println(description);
      Serial.print("Temperature: ");
      Serial.println(temp);

      switch (icon) {
        case 200 ... 299:
          matrix.loadFrame(lightning);
        break;
        case 300 ... 399:
          matrix.loadFrame(lightrain);
        break;
        case 500:
          matrix.loadFrame(lightrain);
        break;
        case 501:
          matrix.loadFrame(lightrain);
        break;
        case 502:
          matrix.loadFrame(heavyrain);
        break;
        case 510 ... 523:
          matrix.loadFrame(rainshower);
        break;
        case 600 ... 699:
          matrix.loadFrame(snow);
        break;
        case 700 ... 799:
          matrix.loadFrame(suncloud);
        break;
        case 800:
          matrix.loadFrame(sun);
        break;
        case 801 ... 803:
          matrix.loadFrame(suncloud);
        break;
        case 804:
          matrix.loadFrame(cloud);
        break;
        default:
    // statements
        break;
      }
      lcd.setCursor(0,0);
      lcd.print("Time: " + time);
      lcd.setCursor(0,1);
      lcd.print("Temp: " + String(temp) + "c");
      delay(2000);
      lcd.clear();
    }

    // Close connection
    client.stop();
  } else {
    Serial.println("Connection failed");
    matrix.loadFrame(nowifi);

    Serial.println(client.getWriteError());
  }
  currentWeather();
}

void fetchDaily() {
  WiFiSSLClient client; 

  const char server[] = "api.weatherbit.io";
  const char* resource = "/v2.0/forecast/daily?lat=-45.8741&lon=170.5036&key=dd2f30b33a644bac846c95daa1975889&days=5";

  if (client.connect(server, 443)) {  
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
    DynamicJsonDocument doc(1024); // Adjust the size as per your JSON response size
    DeserializationError error = deserializeJson(doc, jsonResponse);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Extract data from JSON response
    dataDaily = doc["data"];
    for (int i = 0; i < dataDaily.size(); i++) {
      JsonObject day = dataDaily[i];
      const char* description = day["weather"]["description"];
      float maxTemp = day["max_temp"];
      float minTemp = day["min_temp"];
      int icon = day["weather"]["code"];
      // Extract time from datetime
      String date = day["datetime"].as<String>();
      // Print or store the extracted data for each hour
      Serial.print("Day: ");
      Serial.println(date);
      Serial.print("Description: ");
      Serial.println(description);
      Serial.print("Max temp: ");
      Serial.println(maxTemp);

      switch (icon) {
        case 200 ... 299:
          matrix.loadFrame(lightning);
        break;
        case 300 ... 399:
          matrix.loadFrame(lightrain);
        break;
        case 500:
          matrix.loadFrame(lightrain);
        break;
        case 501:
          matrix.loadFrame(lightrain);
        break;
        case 502:
          matrix.loadFrame(heavyrain);
        break;
        case 510 ... 523:
          matrix.loadFrame(rainshower);
        break;
        case 600 ... 699:
          matrix.loadFrame(snow);
        break;
        case 700 ... 799:
          matrix.loadFrame(suncloud);
        break;
        case 800:
          matrix.loadFrame(sun);
        break;
        case 801 ... 803:
          matrix.loadFrame(suncloud);
        break;
        case 804:
          matrix.loadFrame(cloud);
        break;
        default:
    // statements
        break;
      }



      lcd.setCursor(0,0);
      lcd.print(date);
      lcd.setCursor(0,1);
      lcd.print("Temp:" + String(maxTemp) + " " + String(minTemp));
      delay(2000);
      lcd.clear();
    }

    // Close connection
    client.stop();
  } else {
    Serial.println("Connection failed");
    matrix.loadFrame(nowifi);
    Serial.println(client.getWriteError());
  }
  currentWeather();
}
  void currentWeather(){
WiFiSSLClient client; 

  const char server[] = "api.weatherbit.io";
  const char* resource = "/v2.0/forecast/hourly?lat=-45.8741&lon=170.5036&key=dd2f30b33a644bac846c95daa1975889&hours=6";

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
    DynamicJsonDocument doc(1024); // Adjust the size as per your JSON response size
    DeserializationError error = deserializeJson(doc, jsonResponse);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Extract data from JSON response
    dataHour = doc["data"];
    for (int i = 0; i < 1; i++) {
      JsonObject hour = dataHour[i];
      const char* description = hour["weather"]["description"];
      float temp = hour["temp"];
      // Extract time from datetime
      String time = hour["timestamp_local"].as<String>().substring(11);

      int icon = hour["weather"]["code"];
      // Print or store the extracted data for each hour
      Serial.print(icon);
      Serial.print("Time: ");
      Serial.println(time);
      Serial.print("Description: ");
      Serial.println(description);
      Serial.print("Temperature: ");
      Serial.println(temp);

      switch (icon) {
        case 200 ... 299:
          matrix.loadFrame(lightning);
        break;
        case 300 ... 399:
          matrix.loadFrame(lightrain);
        break;
        case 500:
          matrix.loadFrame(lightrain);
        break;
        case 501:
          matrix.loadFrame(lightrain);
        break;
        case 502:
          matrix.loadFrame(heavyrain);
        break;
        case 510 ... 523:
          matrix.loadFrame(rainshower);
        break;
        case 600 ... 699:
          matrix.loadFrame(snow);
        break;
        case 700 ... 799:
          matrix.loadFrame(suncloud);
        break;
        case 800:
          matrix.loadFrame(sun);
        break;
        case 801 ... 803:
          matrix.loadFrame(suncloud);
        break;
        case 804:
          matrix.loadFrame(cloud);
        break;
        default:
    // statements
        break;
      }
      lcd.setCursor(0,0);
      lcd.print("Time: " + time);
      lcd.setCursor(0,1);
      lcd.print("Temp: " + String(temp) + "c");
      delay(2000);
    }

    // Close connection
    client.stop();
  } else {
    Serial.println("Connection failed");
    matrix.loadFrame(nowifi);
    Serial.println(client.getWriteError());
  }
}


