#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include "src/humidityStorage.pb.h"
#include "../../credentials.h"
#include "sensorConfig.h"

#include "pb_common.h"
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

namespace humidity {

smart_home_StoreHumidityRequest message = smart_home_StoreHumidityRequest_init_zero;

/**
* Establish the connection to the forwarder if it does not yet exist
*/
bool establishConnectionToForwarder(WiFiClient& client){
   while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  for (int i = 0; i < 15; i++) {
    if (!client.connect(FORWARDER_IP, FORWARDER_PORT)) {
      Serial.println("Connecting to server...");
      delay(1000);
    } else {
      Serial.println("Connected to server");
      return true;
    }
  }
  return false;
}


/**
*Read the current state of the sensor and write it into the struct
*/
void readSensorValues(sensor_t sensor) {
  int soilMoistureValue = analogRead(sensor.gpio);
  Serial.print("Sensor: ");
  Serial.print(sensor.gpio);
  Serial.print("  Humidity Value: ");
  Serial.println(soilMoistureValue);
  int soilMoisturePercent = map(soilMoistureValue, sensor.AirValue, sensor.WaterValue, 0, 100);
  if (soilMoisturePercent > 100) {
    soilMoisturePercent = 100;
  } else if (soilMoisturePercent < 0) {
    soilMoisturePercent = 0;
  }
  message.sensorId = sensor.label;
  message.humidityInPercent = soilMoisturePercent;
  message.humidity = soilMoistureValue;
}


void sendToForwarder(WiFiClient client) {
  uint8_t buffer[128];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  bool status = pb_encode(&stream, smart_home_StoreHumidityRequest_fields, &message);

  if (!status) {
    Serial.println("Failed to encode.");
    return;
  }

  Serial.printf("Amount of Bytes %d\n", stream.bytes_written);
  for (int i = 0; i < stream.bytes_written; i++) {
    Serial.printf("%02X", buffer[i]);
  }
  Serial.println("");

  client.write(buffer, stream.bytes_written);
}

void readAndForwardSensors() {
  WiFiClient client;
  establishConnectionToForwarder(client);

  strcpy(message.deviceMAC, WiFi.macAddress().c_str());

  for (sensor_t s : sensors) {
// TODO get set of sensors from HA
    readSensorValues(s);
    sendToForwarder(client);
  }
}
}