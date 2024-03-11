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

#include <ArduinoJson.h>

#include "credentials.h"
#include "gpio_config.h"
#include "spotify.h"
#include "matrix.h"
#include "mvg.h"
#include "httpRequest.h"
#include "openWeather.h"
#include "homeAssistant.h"

size_t current_pos = 0;
size_t max_pos = 80;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "us.pool.ntp.org");  //3600);


#define CONTINUOUS_CLOCK_SECONDS 40
#define TIME_BETWEEN_PINGS 30000  //30sec
#define INTERVAL_TEMP_UPDATE 2000
#define INTERVAL_CLOCK_UPDATE 1000




void setup() {
  Serial.begin(115200);

  // Initialize the NTP client
  timeClient.begin();
  timeClient.setTimeOffset(3600);  // Set your time zone offset (in seconds) here

  pinMode(GPIO_POTI, INPUT);

  pinMode(GPIO_SWITCH_SPOTIFY, INPUT_PULLDOWN);
  pinMode(GPIO_SWITCH_WEATHER_OUTSIDE, INPUT_PULLDOWN);
  pinMode(GPIO_SWITCH_WEATHER_INSIDE, INPUT_PULLDOWN);
  pinMode(GPIO_SWITCH_TIME, INPUT_PULLDOWN);

  matrix::setup();
  spotify::setup();
}


void showTime() {
  timeClient.update();
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  char message[6] = "00:00";
  sprintf(message, "%02d:%02d", hour, minute);
  matrix::displayStaticText(message);
}

void displayClock() {
  matrix::updateBrightness(analogRead(GPIO_POTI));
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
      spotify::showSpotifyCurrentlyPlaying();
    }
  } else {
    if (current_pos == max_pos) {
      current_pos = 0;
      if (digitalRead(GPIO_SWITCH_WEATHER_OUTSIDE)) {
        openWeather::showTemperature();
        delay(INTERVAL_TEMP_UPDATE);
      }

      if (digitalRead(GPIO_SWITCH_WEATHER_INSIDE)) {
        homeAssistant::showTemperature();
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
  current_pos++;
}



void loop() {
  bool pingres = Ping.ping(PHONE_IP);
  printDebugSwitches(pingres);
  if (pingres && (digitalRead(GPIO_SWITCH_SPOTIFY) || digitalRead(GPIO_SWITCH_TIME) || digitalRead(GPIO_SWITCH_WEATHER_OUTSIDE) || digitalRead(GPIO_SWITCH_WEATHER_INSIDE))) {
    executeCurrentStage();
  } else {
    matrix::matrixActivateSleep();
    Serial.println("Going to sleep");
    //current_pos = 0;
    delay(TIME_BETWEEN_PINGS);
  }
Serial.println("test");
  mvg::showMVGDepartureInfo("Garching", "Olympiazentrum");
  delay(10000);
}
