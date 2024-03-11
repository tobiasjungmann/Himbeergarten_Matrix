
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include <Arduino.h>
#include <ArduinoJson.h>

#include "homeAssistant.h"
#include "httpRequest.h"
#include "credentials.h"

#include "matrix.h"

namespace homeAssistant {

#define MAX_TEMP_AGE 60

int insideTempAge = MAX_TEMP_AGE;
char insideTempValue[8] = "00:00";

void showTemperature() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  String apiEndpoint = "http://" + String(HA_IP_ADDRESS) + ":8123/api/states/" + String(HA_TEMP_SENSOR);
  String bearerToken = "Bearer " + String(homeAssistantToken);

  HTTPClient http;
  WiFiClient client;

  http.begin(client, apiEndpoint.c_str());
  http.addHeader("Authorization", bearerToken.c_str());

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
    DynamicJsonDocument myObject(1024 * 2);

    DeserializationError error = deserializeJson(myObject, response);
    if (error) {
      Serial.print("Deserialization failed: ");
      Serial.println(error.c_str());
      return;
    }

    Serial.print("Temperature: ");
    float temp = myObject["state"];
    Serial.println(temp);
    sprintf(insideTempValue, "%.1fC", temp);

  } else {
    Serial.print("Error on HTTP request: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  matrix::displayStaticText(insideTempValue);
}

void test(){
Serial.println("Hello");
}
}
