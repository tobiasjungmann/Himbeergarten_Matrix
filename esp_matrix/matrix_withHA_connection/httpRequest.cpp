#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include "httpRequest.h"
#include "credentials.h"

namespace httpRequest {

void waitForConnection() {
  // Connect to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
#ifdef DEBUG
  Serial.println("Connected to WiFi");
#else
#endif
}

void setup() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  waitForConnection();
}

String performGet(HTTPClient&& http) {
  int httpResponseCode = http.GET();

  String payload = "{}";
  if (httpResponseCode > 0) {
#ifdef DEBUG
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
#else
#endif
// todo better status handling - e.g. nonreachable
    if (httpResponseCode != 404) {
      payload = http.getString(); 
    }
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


String httpRequest(const char* url) {
  waitForConnection();
  HTTPClient http;

  http.begin(url);
  int httpResponseCode = http.GET();

  return performGet(std::move(http));
}

String httpGETRequest(const char* url) {
  waitForConnection();
  WiFiClient client;
  HTTPClient http;

  http.begin(client, url);

  return performGet(std::move(http));
}

String httpRequestHA(const char* url) {
  waitForConnection();

  HTTPClient http;
  WiFiClient client;

  String bearerToken = "Bearer " + String(HA_TOKEN);
  http.begin(client, url);
  http.addHeader("Authorization", bearerToken.c_str());

  return performGet(std::move(http));
}
}
