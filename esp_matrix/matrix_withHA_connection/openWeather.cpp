#include <ArduinoJson.h>
#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#endif


#include "openWeather.h"
#include "httpRequest.h"
#include "matrix.h"
#include "credentials.h"



namespace openWeather {
// Weather
#define MAX_TEMP_AGE 60
String jsonBuffer;
int outsideTempAge = MAX_TEMP_AGE;
char outsideTempValue[8] = "00:00";


void showTemperature() {
  if (outsideTempAge >= MAX_TEMP_AGE) {
    String serverPath = String("http://api.openweathermap.org/data/2.5/weather?q=") + String(OPEN_WEATHER_CITY) + String(",") + String(OPEN_WEATHER_COUNTRY_CODE) + String("&APPID=") + String(OPEN_WEATHER_API_KEY) + String("&units=metric");

    jsonBuffer = httpRequest::httpGETRequest(serverPath.c_str());
    Serial.println(jsonBuffer);
    DynamicJsonDocument myObject(1024 * 2);  // Adjust the size according to your JSON string

    DeserializationError error = deserializeJson(myObject, jsonBuffer);
    if (error) {
      Serial.print("Deserialization failed: ");
      Serial.println(error.c_str());
      return;
    }

    float temp = myObject["main"]["temp"];
    sprintf(outsideTempValue, "%.1fC", temp);

#ifdef DEBUG
    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.println(outsideTempValue);
#else
#endif
    outsideTempAge = 0;
  }

  matrix::displayStaticText(outsideTempValue);
  outsideTempAge++;
}
}
