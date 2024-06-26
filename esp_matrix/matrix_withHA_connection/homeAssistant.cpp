#include <vector>

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
        const char *name;
    };


#define MAX_TEMP_AGE 60

    int insideTempAge = MAX_TEMP_AGE;
    char insideTempValue[8] = "00:00";
    String ha_base_url;
    const String ha_base_sensor_id = "sensor.esp_matrix_";


    bool queryJsonData(DynamicJsonDocument &myObject, String sensor) {
        String url = ha_base_url + String(sensor);
        Serial.println(url);
        String payload = httpRequest::httpRequestHA(url.c_str());

        Serial.println(payload);
        DeserializationError error = deserializeJson(myObject, payload);
        if (error) {
            Serial.print("Deserialization failed: ");
            Serial.println(error.c_str());
            return false;
        }
        return true;
    }


    void showTemperature() {
        DynamicJsonDocument myObject(1024 * 2);
        if (queryJsonData(myObject, HA_TEMP_SENSOR)) {

            float temp = myObject["state"];
            Serial.println(temp);
            sprintf(insideTempValue, "%.1fC", temp);

            matrix::displayStaticText(insideTempValue);
        }
    }


    void showHumidityInPercent() {
        matrix::displayScrollText("Plant Humidity");
        for (auto sensor: HA_HUMIDITY_SENSORS) {
            DynamicJsonDocument myObject(1024 * 2);
            if (queryJsonData(myObject, sensor)) {
                int temp = myObject["state"];
                Serial.println(temp);
                sprintf(insideTempValue, "%i\%", temp);

                matrix::displayStaticText(insideTempValue);
                delay(1000);
            }
        }
    }

/*void getStatesToShow() {
  String url = ha_base_url + String("sensor.matrix_4_train");
  String payload = httpRequest::httpRequestHA(url.c_str());
  Serial.println(payload);

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
*/
    void setup(IPAddress forwarderAddress) {
        ha_base_url = "http://" + forwarderAddress.toString() + ":8123/api/states/";
        //delay(5000);
        //todo für jeden laufen lassen
     /*   String url = ha_base_url + ha_base_sensor_id + "_invalid_new3";
        Serial.println("url: " + url);
        //String payload = httpRequest::httpRequestHA(url.c_str());
        //Serial.println("Payload" + payload);

        // if (payload.equals("{}")) {
        Serial.println("Creating new matrix entires");
        //String bearerToken = "Bearer " + String(HA_TOKEN);
        //HTTPClient http;
        //WiFiClient client;
        const char *jsonData = "{\"name\": \"my_name\",\"state\": \"0\"}";  //"{\"state\": \"0\"}";
        //  http.begin(client, url);
        // http.addHeader("Authorization", bearerToken.c_str());
        HTTPClient http;
        WiFiClient client;

        String bearerToken = "Bearer " + String(HA_TOKEN);
        http.begin(client, url);
        http.addHeader("Authorization", bearerToken.c_str());

        http.addHeader("Content-Type", "application/json");
        // http.addHeader("Content-Length", String(strlen(jsonData)));
        //http.addHeader("Connection", "close");

        // http.beginBody();
        int statusCode = http.POST(jsonData);
        //http.endRequest();

        // Wait for the server's response
        // int statusCode = http.responseStatusCode();
        Serial.println("Status: asdfasdfasdfasdfölkjölkjölkjölkjasdfasdfasdasdffölkjölkjölkjölkj");
        Serial.println("" + statusCode);
        String response = http.getString();
        http.end();

        Serial.println("Response: " + response);
        //} else {
        //}

        Serial.println("Sensor Setup completed");
        delay(1000000);*/

        /*DynamicJsonDocument myObject(1024 * 2);

        DeserializationError error = deserializeJson(myObject, payload);
        if (error) {
          Serial.print("Deserialization failed: ");
          Serial.println(error.c_str());
          return;
        }*/
    }

/**
* todo implemnt to load current status from home assitant
*/
    void updateSelectedStates();

    std::vector <MatrixState> getStatesToShow() {
        return std::vector < MatrixState > {SPOTIFY,
                                            TRAIN,
                                            TEMP_INSIDE,
                                            TEMP_OUTSIDE,
                                            TIME,
                                            HUMIDITY};
    }
}
