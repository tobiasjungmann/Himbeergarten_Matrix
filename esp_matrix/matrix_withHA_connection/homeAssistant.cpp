
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

struct StateInfo {
  MatrixState state;
  bool displayed;
  const char* name;
};


#define MAX_TEMP_AGE 60

int insideTempAge = MAX_TEMP_AGE;
char insideTempValue[8] = "00:00";
const String ha_base_url = "http://" + String(HA_IP_ADDRESS) + ":8123/api/states/";
const String ha_base_sensor_id = "sensor.esp_matrix_" + WiFi.macAddress();

void showTemperature() {
  String url = ha_base_url + String(HA_TEMP_SENSOR);
  String payload = httpRequest::httpRequestHA(url.c_str());

  DynamicJsonDocument myObject(1024 * 2);

  DeserializationError error = deserializeJson(myObject, payload);
  if (error) {
    Serial.print("Deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  //Serial.print("Temperature: ");
  float temp = myObject["state"];
  Serial.println(temp);
  sprintf(insideTempValue, "%.1fC", temp);

  matrix::displayStaticText(insideTempValue);
}

void setup() {
  delay(5000);
  //todo für jeden laufen lassen
  String url = ha_base_url + ha_base_sensor_id + "_invalid";
  Serial.println("url: " + url);
  String payload = httpRequest::httpRequestHA(url.c_str());
  Serial.println("Payload" + payload);

  if (payload.equals("{}")) {
    Serial.println("Creating new matrix entires");
    String bearerToken = "Bearer " + String(HA_TOKEN);
    HTTPClient http;
    WiFiClient client;
    const char* jsonData = "{\"state\": \"0\"}";
    http.begin(client, url);
    http.addHeader("Authorization", bearerToken.c_str());

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Content-Length", String(strlen(jsonData)));
    //http.addHeader("Connection", "close");

    // http.beginBody();
    int statusCode = http.POST(jsonData);
    //http.endRequest();

    // Wait for the server's response
    // int statusCode = http.responseStatusCode();
    Serial.println("Status: " + statusCode);
    String response = http.getString();
    Serial.println("Created:" + response);
  } else {
  }

  Serial.println("Senosor Setup completed");
  delay(1000000);

  /*DynamicJsonDocument myObject(1024 * 2);

  DeserializationError error = deserializeJson(myObject, payload);
  if (error) {
    Serial.print("Deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }*/
}

void updateSelectedStates();
bool isStateVisible(MatrixState state);
}
