#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266Ping.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPping.h>
#endif

#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

#include <SpotifyArduino.h>
#include <SpotifyArduinoCert.h>
#include <ArduinoJson.h>

#include "credentials.h"
#include "gpio_config.h"

size_t current_pos = 0;
size_t max_pos = 80;

// Spotify
#define SPOTIFY_MARKET "DE"
WiFiClientSecure client;
SpotifyArduino spotify(client, clientId, clientSecret, SPOTIFY_REFRESH_TOKEN);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "us.pool.ntp.org");  //3600);


// Matrix
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CHAR_SPACING 4  // pixels between characters
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola mx = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


enum matrixState {
  DISPLAY_OFF,
  SPOTIFY,
  CLOCK,
  WEATHER_OUTSIDE,
  WEATHER_INSIDE
};


// Weather
String city = "Munich";
String countryCode = "DE";
String jsonBuffer;

#define MAX_TEMP_AGE 60
#define CONTINUOUS_CLOCK_SECONDS 40
#define TIME_BETWEEN_PINGS 30000  //30sec
#define INTERVAL_TEMP_UPDATE 2000
#define INTERVAL_CLOCK_UPDATE 1000

int outsideTempAge = MAX_TEMP_AGE;
char outsideTempValue[8] = "00:00";
int insideTempAge = MAX_TEMP_AGE;
char insideTempValue[8] = "00:00";

String station_id_garching = "";
String station_id_oly = "";

// states whether something is currently displayed in the matrix
boolean matrix_cleared = true;

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
  timeClient.setTimeOffset(3600);  // Set your time zone offset (in seconds) here

#if defined(ESP8266)
  client.setFingerprint(SPOTIFY_FINGERPRINT);  // These expire every few months
#elif defined(ESP32)
  client.setCACert(spotify_server_cert);
#endif

  Serial.println("Refreshing Access Tokens");
  //if (!spotify.refreshAccessToken()) {
  //  Serial.println("Failed to get access tokens");
  //}

  pinMode(GPIO_SWITCH_SPOTIFY, INPUT_PULLDOWN);
  pinMode(GPIO_SWITCH_WEATHER_OUTSIDE, INPUT_PULLDOWN);
  pinMode(GPIO_SWITCH_WEATHER_INSIDE, INPUT_PULLDOWN);
  pinMode(GPIO_SWITCH_TIME, INPUT_PULLDOWN);
}

void showInsideTempFromHA() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  String apiEndpoint = "http://" + String(HA_IP_ADDRESS) + ":8123/api/states/" + String(HA_TEMP_SENSOR);
  HTTPClient http;

  String bearerToken = "Bearer " + String(homeAssistantToken);
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
  mx.displayText(insideTempValue, PA_CENTER, 25, 500, PA_PRINT, PA_PRINT);
  mx.displayAnimate();
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);
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
  http.end();

  return payload;
}

String httpRequest(const char* serverName) {
  HTTPClient http;

  http.begin(serverName);
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
  http.end();

  return payload;
}

void showTemperatureOutside() {
  if (outsideTempAge >= MAX_TEMP_AGE) {
    if (WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";
      //String serverPath = "google.de";//efa.mvv-muenchen.de/xhr_trips?language=de&locationServerActive=1&stateless=1&type_origin=any&type_destination=any&itdDate=20231211&itdTripDateTimeDepArr=dep&itdTime=1532&nameInfo_origin=91000100&nameInfo_destination=1000460&useRealtime=1&includedMeans=1&itOptionsActive=1&ptOptionsActive=1&imparedOptionsActive=1&computationType=sequence&inclMOT_0=off&inclMOT_7=off&inclMOT_1=off&inclMOT_5=off&inclMOT_2=off&inclMOT_6=off&inclMOT_4=off&inclMOT_10=off&inclMOT_17=off&changeSpeed=normal&trITDepMOT=100&trITDepMOTvalue100=10&trITArrMOT=100&trITArrMOTvalue100=10&trITDepMOTvalue101=15&trITArrMOTvalue101=15&routeType=leasttime&adv_opt_use_realtime=on&lineRestriction=403&itdLPxx_pAndRSM=auto&msMacro_1114=true&msMacro_1112=true&msMacro_1111=true&msMacro_1105=true&msMacro_1106=true&msMacro_1107=true&msMacro_1113=true&msMacro_1110=true&msMacro_1116=true&msMacro_1100=true&msMacro_1006=true&msMacro_1102=true&msMacro_1103=true&msMacro_1109=true&msMacro_1101=true&msMacro_1104=true&msMacro_1003=true&msMacro_1001=true&msMacro_1124=true&msMacro_1115=true&msMacro_1122=true&msMacro_1123=true&coordOutputFormat=MRCV&session_key=11bc3d85593d691a&_=1703333409739";

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

      sprintf(outsideTempValue, "%.1fC", temp);
      Serial.println(outsideTempValue);
    } else {
      Serial.println("WiFi Disconnected");
    }
    outsideTempAge = 0;
  }

  mx.displayText(outsideTempValue, PA_CENTER, 25, 500, PA_PRINT, PA_PRINT);
  mx.displayAnimate();
  outsideTempAge++;
}


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
  mx.displayScroll(message, PA_RIGHT, PA_SCROLL_LEFT, 48);
  while (!mx.displayAnimate()) { ; }
}

void updateBrightness() {
  int analogValue = analogRead(GPIO_POTI);
  float brightness = analogValue / 256;
  mx.setIntensity(brightness);
}

void displayClock() {
  updateBrightness();
  showTime();
}

void printDebugSwitches(bool pingres) {
#ifdef DEBUG
  Serial.print("Phone reachable: ");
  Serial.println(pingres);
  Serial.print("GPIO_SWITCH_SPOTIFY: ");
  Serial.println(digitalRead(GPIO_SWITCH_SPOTIFY));
  Serial.print("GPIO_SWITCH_TIME: ");
  Serial.println(digitalRead(GPIO_SWITCH_TIME));
  Serial.print("GPIO_SWITCH_WEATHER_OUTSIDE: ");
  Serial.println(digitalRead(GPIO_SWITCH_WEATHER_OUTSIDE));
  Serial.print("GPIO_SWITCH_WEATHER_INSIDE: ");
  Serial.println(digitalRead(GPIO_SWITCH_WEATHER_INSIDE));
#else
#endif
}

void executeCurrentStage() {
  if (current_pos == max_pos / 2) {
    if (digitalRead(GPIO_SWITCH_SPOTIFY)) {
      Serial.println("Spotify");
      showSpotifyCurrentlyPlaying();
    }
  } else {
    if (current_pos == max_pos) {
      current_pos = 0;
      if (digitalRead(GPIO_SWITCH_WEATHER_OUTSIDE)) {
        showTemperatureOutside();
        delay(INTERVAL_TEMP_UPDATE);
      }

      if (digitalRead(GPIO_SWITCH_WEATHER_INSIDE)) {
        showInsideTempFromHA();
        delay(INTERVAL_TEMP_UPDATE);
      }

    } else {
      if (digitalRead(GPIO_SWITCH_TIME)) {
        Serial.println("Time");
        displayClock();
        delay(INTERVAL_CLOCK_UPDATE);
      }
    }
  }

  matrix_cleared = false;
  current_pos++;
}

String getTrainStationId(String station) {
  String url = String("https://www.mvg.de/api/fib/v2/location?query=") + station;
  String station_data = httpRequest(url.c_str());
 // Serial.println(station_data);

  //Less memory consumption than complete JSON deserialization
  int index = station_data.indexOf("globalId");
  if (index >= 0) {
    return station_data.substring(index + 11, index + 23);
  }
  return "Invalid";
}

String getTrainDeprature(String from, String to) {
  String url = String("https://www.mvg.de/api/fib/v2/connection?originStationGlobalId=")
               + from
               + String("&destinationStationGlobalId=")
               + to
               + String("&routingDateTimeIsArrival=false&transportTypes=SCHIFF,RUFTAXI,BAHN,UBAHN,TRAM,SBAHN,BUS,REGIONAL_BUS");
  Serial.println(url);
  String departure_data = httpRequest(url.c_str());
  //Serial.println(departure_data);

  //Less memory consumption than complete JSON deserialization
  int index = departure_data.indexOf("plannedDeparture");
  if (index >= 0) {
    return departure_data.substring(index + 29, index + 44);
  }
  return "Invalid";
}

void getMVGInfos() {
  if (station_id_garching.length()==0) {
    Serial.println("Loading Station Names");
    station_id_garching = getTrainStationId("Garching");
    station_id_oly = getTrainStationId("Olympiazentrum");
  }
  Serial.println(getTrainDeprature(station_id_garching, station_id_oly));
}

void loop() {
  /* bool pingres = Ping.ping(PHONE_IP);
  printDebugSwitches(pingres);
  showTemperatureOutside();
  if (pingres && (digitalRead(GPIO_SWITCH_SPOTIFY) || digitalRead(GPIO_SWITCH_TIME) || digitalRead(GPIO_SWITCH_WEATHER_OUTSIDE) || digitalRead(GPIO_SWITCH_WEATHER_INSIDE))) {
    executeCurrentStage();
  } else {
    Serial.println("Going to sleep");
    if (!matrix_cleared) {
      matrix_cleared = true;
      mx.displayClear();
      // todo  P.displayShutdown(true); if it is switchd off?
    }
    //current_pos = 0;
    delay(TIME_BETWEEN_PINGS);
  }*/

  getMVGInfos();
  delay(10000);
}
