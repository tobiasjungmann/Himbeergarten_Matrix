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
#include <time.h>
#include <WiFiUdp.h>

#include "credentials.h"
#include "gpio_config.h"
#include "spotify.h"
#include "matrix.h"
#include "mvg.h"
#include "httpRequest.h"
#include "openWeather.h"
#include "homeAssistant.h"
#include "src/humidityReader.h"

size_t current_pos = 0;
size_t max_pos = 80;

#define CONTINUOUS_CLOCK_SECONDS 40
#define TIME_BETWEEN_PINGS 30000  //30sec
#define INTERVAL_TEMP_UPDATE 2000
#define INTERVAL_CLOCK_UPDATE 1000

#define STATE_BASED_ON_HA  // Remove if the physical switches should be used to activate/deactivate states, if defined the HA defines a set of active matrix states
uint8_t nextSpecialState = 0;

std::vector <homeAssistant::MatrixState> activeMatrixStates;

void setup() {
    Serial.begin(115200);
    IPAddress forwarderAddress = httpRequest::setup();

    matrix::setup();
    spotify::setup();
    humidityForwarder::setup(forwarderAddress);
    homeAssistant::setup(forwarderAddress);

    activeMatrixStates = homeAssistant::getStatesToShow();

#if defined(ESP8266)
    configTime(TIME_TIMEZONE, TIME_SERVER);
#elif defined(ESP32)
    configTime(0, 0, TIME_SERVER);
    setenv("TZ", TIME_TIMEZONE, 1);
    tzset();
#endif

    pinMode(GPIO_POTI, INPUT);

    pinMode(GPIO_SWITCH_SPOTIFY, INPUT_PULLDOWN);
    pinMode(GPIO_SWITCH_WEATHER_OUTSIDE, INPUT_PULLDOWN);
    pinMode(GPIO_SWITCH_WEATHER_INSIDE, INPUT_PULLDOWN);
    pinMode(GPIO_SWITCH_TIME, INPUT_PULLDOWN);
}


void showTime() {
    time_t now;
    tm tm;
    time(&now);
    localtime_r(&now, &tm);

    int hour = tm.tm_hour;
    int minute = tm.tm_min;
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
#ifdef STATE_BASED_ON_HA    // only compile the code for the preselected version - changing later not yet possible
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

            mvg::showMVGDepartureInfo("Garching", "Olympiazentrum");
            homeAssistant::showHumidityInPercent();

        } else {
            if (digitalRead(GPIO_SWITCH_TIME)) {
                Serial.println("Time");
                displayClock();
                delay(INTERVAL_CLOCK_UPDATE);
            }
        }
    }
#else
    if (current_pos == max_pos) {
      current_pos = 0;
      nextSpecialState = (nextSpecialState++) % activeMatrixStates;
      switch (activeMatrixStates[nextSpecialState]) {
        case homeAssistant::SPOTIFY: spotify::showSpotifyCurrentlyPlaying(); break;
        case homeAssistant::TRAIN: mvg::showMVGDepartureInfo("Garching", "Olympiazentrum"); break;
        case homeAssistant::TEMP_OUTSIDE: homeAssistant::showTemperature(); delay(INTERVAL_TEMP_UPDATE); openWeather::showTemperature(); delay(INTERVAL_TEMP_UPDATE);break;
        case homeAssistant::HUMIDITY: homeAssistant::showHumidityInPercent(); break;
        default: break;
      }
      else {    // every other iteration which is not a special case
        displayClock();
        delay(INTERVAL_CLOCK_UPDATE);
      }
    }
#endif
    current_pos++;
}


void loop() {
    /*bool pingres = true;  //Ping.ping(PHONE_IP);
    printDebugSwitches(pingres);
    if (pingres) {
      executeCurrentStage();
    } else {
      matrix::matrixActivateSleep();
      Serial.println("Going to sleep");
      delay(TIME_BETWEEN_PINGS);
    }*/
    //  humidityForwarder::readAndForwardSensors();
    displayClock();
    //            delay(INTERVAL_CLOCK_UPDATE);
    delay(2000);
}
