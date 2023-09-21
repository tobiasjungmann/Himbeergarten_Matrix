#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
//#include <Arduino_JSON.h>
#include "credentials.h"

#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

#include <SpotifyArduino.h>
#include <SpotifyArduinoCert.h>
#include <ArduinoJson.h>


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

#define CLK_PIN 12   // or SCK
#define DATA_PIN 27  // or MOSI
#define CS_PIN 14    // or SS

#define CHAR_SPACING 4  // pixels between characters
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola mx = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Weather
String city = "Munich";
String countryCode = "DE";
String jsonBuffer;

void printTime(char* pMsg) {
}
/*
void printText(uint8_t modStart, uint8_t modEnd, char* pMsg) {
  uint8_t state = 0;
  uint8_t curLen;
  uint16_t showLen;
  uint8_t cBuf[8];
  int16_t col = ((modEnd + 1) * COL_SIZE) - 1;

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  do {
    switch (state) {
      case 0:  // Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0') {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }

        // retrieve the next character form the font file
        showLen = mx.getChar(*pMsg++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1:  // display the next part of the character
        mx.setColumn(col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen) {
          showLen = CHAR_SPACING;
          state = 2;
        }
        break;

      case 2:  // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3:  // display inter-character spacing or end of message padding (blank columns)
        mx.setColumn(col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;  // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void scrollText(const char* p) {
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts

 // PRINTS("\nScrolling text");
  mx.clear();

  while (*p != '\0') {
    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);

    for (uint8_t i = 0; i <= charWidth; i++)  // allow space between characters
    {
      mx.transform(MD_MAX72XX::TSL);
      mx.setColumn(0, (i < charWidth) ? cBuf[i] : 0);
      delay(100);
    }
  }
}
*/

/*
// TODO combine with the poti

void intensity()
// Demonstrates the control of display intensity (brightness) across
// the full range.
{
  uint8_t row;

  PRINTS("\nVary intensity ");

  mx.clear();

  // Grow and get brighter
  row = 0;
  for (int8_t i=0; i<=MAX_INTENSITY; i++)
  {
    mx.control(MD_MAX72XX::INTENSITY, i);
    if (i%2 == 0)
      mx.setRow(row++, 0xff);
    delay(DELAYTIME*3);
  }

  mx.control(MD_MAX72XX::INTENSITY, 8);
}*/

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
  http.begin(apiEndpoint.c_str());
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
  // ... or don't!
  //client.setInsecure();

  // If you want to enable some extra debugging
  // uncomment the "#define SPOTIFY_DEBUG" in SpotifyArduino.h

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
/*
void showTemperatureOutside() {
  if (WiFi.status() == WL_CONNECTED) {
    String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";

    jsonBuffer = httpGETRequest(serverPath.c_str());
    Serial.println(jsonBuffer);
    JSONVar myObject = JSON.parse(jsonBuffer);

    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    Serial.print("JSON object = ");
    Serial.println(myObject);
    Serial.print("Temperature: ");
    Serial.println(myObject["main"]["temp"]);
    /*Serial.print("Pressure: ");
    Serial.println(myObject["main"]["pressure"]);
    Serial.print("Humidity: ");
    Serial.println(myObject["main"]["humidity"]);
    Serial.print("Wind Speed: ");
    Serial.println(myObject["wind"]["speed"]);//
  } else {
    Serial.println("WiFi Disconnected");
  }
}*/

// todo  P.displayShutdown(true); if it is switchd off?


void showTime() {
  // todo don't load it from the internet everytime
  //         mx.setTextAlignment(PA_CENTER);
  //       mx.displayClear();
  //       mx.displayReset();
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
    /* Serial.print("Track: ");
    Serial.println(currentlyPlaying.trackName);
    for (int i = 0; i < currentlyPlaying.numArtists; i++) {
      Serial.print("Name: ");
      Serial.println(currentlyPlaying.artists[i].artistName);
    }
    Serial.print("Album: ");
    Serial.println(currentlyPlaying.albumName);


    long progress = currentlyPlaying.progressMs;  // duration passed in the song
    long duration = currentlyPlaying.durationMs;  // Length of Song
    Serial.print("Elapsed time of song (ms): ");
    Serial.print(progress);
    Serial.print(" of ");
    Serial.println(duration);
    Serial.println();*/
    char message[256] = "";
    sprintf(message, "%s - %s", currentlyPlaying.trackName, currentlyPlaying.artists[0].artistName);
    mx.displayAnimate();
    mx.displayScroll(message, PA_LEFT, PA_PRINT, 25);
    Serial.println("Message to print");
    Serial.println(message);
    //   mx.displayText(message, PA_LEFT, 25, 500, PA_SCROLL_LEFT, PA_PRINT);

    // mx.displayReset();
    //   mx.displayText("mge", PA_LEFT, 25, 2000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    //       mx.displayText("Your Scrolling Text Here", PA_CENTER, PA_SCROLL_LEFT, 1000);
    //mx.run();
    //mx.displayAnimate();
    //scrollText(message);
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
void displayScrollText(char* message){
  mx.displayClear();
  mx.displayScroll(message, PA_RIGHT, PA_SCROLL_LEFT, 25);
  while (!mx.displayAnimate()) { ; }
}

void loop() {
   for(int i=0;i<20;i++){
  mx.setIntensity(50);
  showTime();
  delay(1000);
  }
  showSpotifyCurrentlyPlaying();
  //showTemperatureOutside();
}