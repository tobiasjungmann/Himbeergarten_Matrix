#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>
//#include <Arduino_JSON.h>
#include "credentials.h"


#include <SpotifyArduino.h>
#include <SpotifyArduinoCert.h>
#include <ArduinoJson.h>


#define GPIO_POTI 36
#define GPIO_SWITCH_SPOTIFY 23

// Spotify
#define SPOTIFY_MARKET "DE"
WiFiClientSecure client;
SpotifyArduino spotify(client, clientId, clientSecret, SPOTIFY_REFRESH_TOKEN);

unsigned long delayBetweenRequests = 60000;  // Time between requests (1 minute)
unsigned long requestDueTime;                //time when request due


// Homeassistant
const char* homeAssistantUrl = "192.168.0.103";
const char* sensorEntityId = "your_sensor_entity_id";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);


// Matrix
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#if defined(ESP8266)
#define CLK_PIN 6   // or SCK
#define CS_PIN 7    // or SS
#define DATA_PIN 8  // or MOSI
#elif defined(ESP32)
#define CLK_PIN 12   // or SCK
#define CS_PIN 14    // or SS
#define DATA_PIN 27  // or MOSI
#endif

#define CHAR_SPACING 4  // pixels between characters
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola mx = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Weather
String city = "Munich";
String countryCode = "DE";
String jsonBuffer;

void readFromHA() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Make the REST request
  String apiEndpoint = "http://" + String(homeAssistantUrl) + ":8123/api/states/" + String(sensorEntityId);
  HTTPClient http;

  // Set the authorization header
  String bearerToken = "Bearer " + String(homeAssistantToken);
  WiFiClient client;
  http.begin(client, apiEndpoint.c_str());
  http.addHeader("Authorization", bearerToken.c_str());

  // Send GET request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.print("Error on HTTP request: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}


void setup() {
  Serial.begin(115200);
  mx.begin();

  pinMode(GPIO_POTI, INPUT);
  pinMode(GPIO_SWITCH_SPOTIFY, INPUT_PULLDOWN);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the NTP client
  timeClient.begin();
  timeClient.setTimeOffset(7200);  // Set your time zone offset (in seconds) here

#if defined(ESP8266)
  client.setFingerprint(SPOTIFY_FINGERPRINT);  // These expire every few months
#elif defined(ESP32)
  client.setCACert(spotify_server_cert);
#endif

  Serial.println("Refreshing Access Tokens");
  if (!spotify.refreshAccessToken()) {
    Serial.println("Failed to get access tokens");
  }
}


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void showTemperatureOutside() {
  if (WiFi.status() == WL_CONNECTED) {
    String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";

    jsonBuffer = httpGETRequest(serverPath.c_str());
    Serial.println(jsonBuffer);
    DynamicJsonDocument myObject(1024 * 2);  // Adjust the size according to your JSON string

    DeserializationError error = deserializeJson(myObject, jsonBuffer);
    if (error) {
      Serial.print("Deserialization failed: ");
      Serial.println(error.c_str());
      return;
    }

    Serial.print("Temperature: ");
    float temp = myObject["main"]["temp"];
    Serial.println(temp);
    char message[8] = "0.00";
    sprintf(message, "%.1fC", temp);
    Serial.println(message);
    mx.displayText(message, PA_CENTER, 25, 500, PA_PRINT, PA_PRINT);
    mx.displayAnimate();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

// todo  P.displayShutdown(true); if it is switchd off?


void showTime() {
  timeClient.update();
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  char message[6] = "00:00";
  sprintf(message, "%02d:%02d", hour, minute);
  Serial.println(message);
  mx.displayText(message, PA_CENTER, 25, 500, PA_PRINT, PA_PRINT);
  mx.displayAnimate();
}

void printCurrentlyPlayingSpotify(CurrentlyPlaying currentlyPlaying) {
  if (currentlyPlaying.isPlaying) {
    char message[256] = "";
    sprintf(message, "%s - %s", currentlyPlaying.trackName, currentlyPlaying.artists[0].artistName);

    displayScrollText(message);
  } else {
    Serial.println("Spotify is currently not playing");
  }
}

void showSpotifyCurrentlyPlaying() {
  int status = spotify.getCurrentlyPlaying(printCurrentlyPlayingSpotify, SPOTIFY_MARKET);
  if (status == 200) {
    Serial.println("Successfully got currently playing");
  } else if (status == 204) {
    Serial.println("Doesn't seem to be anything playing");
  } else {
    Serial.print("Error: ");
    Serial.println(status);
  }
}
void displayScrollText(char* message) {
  mx.displayClear();
  mx.displayScroll(message, PA_RIGHT, PA_SCROLL_LEFT, 40);
  while (!mx.displayAnimate()) { ; }
}

void updateBrightness(){
   int analogValue = analogRead(36);
  float brightness = analogValue/256;
  mx.setIntensity(brightness);
}
void loop() {
  for (int i = 0; i < 5; i++) {
    updateBrightness();
    showTime();
    delay(1000);
  }
  Serial.println("Digital: ");
  int digitalValue=digitalRead(GPIO_SWITCH_SPOTIFY);
  Serial.println(digitalValue);
  if (digitalRead(GPIO_SWITCH_SPOTIFY)) {
    showSpotifyCurrentlyPlaying();
  }
  //showTemperatureOutside();
  //delay(100000);
}