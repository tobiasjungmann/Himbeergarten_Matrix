
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
  String url = "http://" + String(HA_IP_ADDRESS) + ":8123/api/states/" + String(HA_TEMP_SENSOR);
  String payload = httpRequest::httpRequestHA(url.c_str());

  DynamicJsonDocument myObject(1024 * 2);

  DeserializationError error = deserializeJson(myObject, payload);
  if (error) {
    Serial.print("Deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  Serial.print("Temperature: ");
  float temp = myObject["state"];
  Serial.println(temp);
  sprintf(insideTempValue, "%.1fC", temp);

  matrix::displayStaticText(insideTempValue);
}

void registerAtHa() {
  String url = "http://" + String(HA_IP_ADDRESS) + ":8123/api/states/sensor.esp_matrix_" + WiFi.macAddress();
  String payload = httpRequest::httpRequestHA(url.c_str());

  DynamicJsonDocument myObject(1024 * 2);

  DeserializationError error = deserializeJson(myObject, payload);
  if (error) {
    Serial.print("Deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  Serial.print("Temperature: ");
  float temp = myObject["state"];
  Serial.println(temp);
  sprintf(insideTempValue, "%.1fC", temp);

  matrix::displayStaticText(insideTempValue);
}
}
