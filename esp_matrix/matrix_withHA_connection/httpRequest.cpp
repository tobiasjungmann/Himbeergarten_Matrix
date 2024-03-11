#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include "httpRequest.h"
#include "credentials.h"

namespace httpRequest{

void waitForConnection(){
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
#ifdef DEBUG
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
#else
#endif
    payload = http.getString();
  } else {
#ifdef DEBUG
    Serial.print("HTTP Error code: ");
    Serial.println(httpResponseCode);
#else
#endif
  }
  http.end();

  return payload;
}

String httpRequest(const char* serverName) {
  HTTPClient http;

  http.begin(serverName);
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
#ifdef DEBUG
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
#else
#endif
    payload = http.getString();
  } else {
#ifdef DEBUG
    Serial.print("HTTP Error code: ");
    Serial.println(httpResponseCode);
#else
#endif
  }
  http.end();

  return payload;
}

}