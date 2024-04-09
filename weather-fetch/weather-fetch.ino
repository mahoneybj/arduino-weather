#include "Arduino_LED_Matrix.h"
#include <Wire.h>
#include <FastLED.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiS3.h> 
#include "WiFiSSLClient.h"
#include "ArduinoJson.h"
#include "symbols.h"

//Led defines
#define LED_PIN     6
#define NUM_LEDS    8
#define BRIGHTNESS  40
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

char ssid[] = "Pig";           // WiFi network SSID
char pass[] = "TommyInnit72";  // WiFi network password
const int buttonPinH = 2; 
const int buttonPinD = 3; 
int buttonStateH = 0;
int buttonStateD = 0;

//Initlize JsonArrays for daily and hourly data
JsonArray dataHour;
JsonArray dataDaily;

//Initlize R4 wifi built in matrix and LED screen
ArduinoLEDMatrix matrix;
LiquidCrystal_I2C lcd(0x27,  16, 2);

void setup() {
  Serial.begin(9600);
  matrix.begin();
  delay(5000);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  while (!Serial);
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

  //Run current weather
  currentWeather();
}

void loop() {
  //Define buttons for hourly and daily fetch
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

  //Weather api server and path (Using Weatherbit.io)
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
      // Print data for each hour
      Serial.print(icon);
      Serial.print("Time: ");
      Serial.println(time);
      Serial.print("Description: ");
      Serial.println(description);
      Serial.print("Temperature: ");
      Serial.println(temp);

      //Clear led
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();

      //Switch case for each weather code
      switch (icon) {
        case 200 ... 299:
          matrix.loadFrame(lightning);
        break;
        case 300 ... 399:
          matrix.loadFrame(lightrain);
        break;
        case 500:
          matrix.loadFrame(lightrain);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::White;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 501:
          matrix.loadFrame(lightrain);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::Orange;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 502:
          matrix.loadFrame(heavyrain);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::Red;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 510 ... 523:
          matrix.loadFrame(rainshower);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::Yellow;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 600 ... 699:
          matrix.loadFrame(snow);
        break;
        case 700 ... 799:
          matrix.loadFrame(suncloud);
          for (int i = 0; i < NUM_LEDS / 2; i++) {
            leds[i] = CRGB::White;
            leds[i + NUM_LEDS / 2] = CRGB::Yellow;
          }

          FastLED.show();
        break;
        case 800:
          matrix.loadFrame(sun);
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Yellow;
          }

          FastLED.show();
        break;
        case 801 ... 803:
          matrix.loadFrame(suncloud);
          for (int i = 0; i < NUM_LEDS / 2; i++) {
            leds[i] = CRGB::White;
            leds[i + NUM_LEDS / 2] = CRGB::Yellow;
          }

          FastLED.show();
        break;
        case 804:
          matrix.loadFrame(cloud);
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::White;
          }

          FastLED.show();
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
    //Matrix for failed fetch
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
      // Print extracted data for each day
      Serial.print("Day: ");
      Serial.println(date);
      Serial.print("Description: ");
      Serial.println(description);
      Serial.print("Max temp: ");
      Serial.println(maxTemp);

      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      switch (icon) {
        case 200 ... 299:
          matrix.loadFrame(lightning);
        break;
        case 300 ... 399:
          matrix.loadFrame(lightrain);
        break;
        case 500:
          matrix.loadFrame(lightrain);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::White;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 501:
          matrix.loadFrame(lightrain);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::Orange;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 502:
          matrix.loadFrame(heavyrain);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::Red;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 510 ... 523:
          matrix.loadFrame(rainshower);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::Yellow;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 600 ... 699:
          matrix.loadFrame(snow);
        break;
        case 700 ... 799:
          matrix.loadFrame(suncloud);
          for (int i = 0; i < NUM_LEDS / 2; i++) {
            leds[i] = CRGB::White;
            leds[i + NUM_LEDS / 2] = CRGB::Yellow;
          }

          FastLED.show();
        break;
        case 800:
          matrix.loadFrame(sun);
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Yellow;
          }

          FastLED.show();
        break;
        case 801 ... 803:
          matrix.loadFrame(suncloud);
          for (int i = 0; i < NUM_LEDS / 2; i++) {
            leds[i] = CRGB::White;
            leds[i + NUM_LEDS / 2] = CRGB::Yellow;
          }

          FastLED.show();
        break;
        case 804:
          matrix.loadFrame(cloud);
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::White;
          }

          FastLED.show();
        break;
        default:
    // statements
        break;
      }


      lcd.clear();
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
    //Matrix for failed fetch
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
      // Print fetched data
      Serial.print(icon);
      Serial.print("Time: ");
      Serial.println(time);
      Serial.print("Description: ");
      Serial.println(description);
      Serial.print("Temperature: ");
      Serial.println(temp);
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      switch (icon) {
        case 200 ... 299:
          matrix.loadFrame(lightning);
        break;
        case 300 ... 399:
          matrix.loadFrame(lightrain);
        break;
        case 500:
          matrix.loadFrame(lightrain);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::White;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 501:
          matrix.loadFrame(lightrain);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::Orange;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 502:
          matrix.loadFrame(heavyrain);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::Red;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 510 ... 523:
          matrix.loadFrame(rainshower);
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) {
              leds[i] = CRGB::Yellow;
            } else {
              leds[i] = CRGB::Blue;
            }
          } 

          FastLED.show();
        break;
        case 600 ... 699:
          matrix.loadFrame(snow);
        break;
        case 700 ... 799:
          matrix.loadFrame(suncloud);
          for (int i = 0; i < NUM_LEDS / 2; i++) {
            leds[i] = CRGB::White;
            leds[i + NUM_LEDS / 2] = CRGB::Yellow;
          }

          FastLED.show();
        break;
        case 800:
          matrix.loadFrame(sun);
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Yellow;
          }

          FastLED.show();
        break;
        case 801 ... 803:
          matrix.loadFrame(suncloud);
          for (int i = 0; i < NUM_LEDS / 2; i++) {
            leds[i] = CRGB::White;
            leds[i + NUM_LEDS / 2] = CRGB::Yellow;
          }

          FastLED.show();
        break;
        case 804:
          matrix.loadFrame(cloud);
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::White;
          }

          FastLED.show();
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
    //Matrix for failed fetch
    matrix.loadFrame(nowifi);
    Serial.println(client.getWriteError());
  }
}


