#ifndef SENSORCONFIG_H
#define SENSORCONFIG_H

const uint16_t FORWARDER_PORT = 12348;        // Port of the forwarder interface


// TODO better storage instead of harcoding
/*
const int AirValue    //the value which is returned when the sensor is dry
const int WaterValue  //the value which is returned when the sensor is submerged in water

Setup values for the specific sensors:
Sensor    Air   Water
1         880   470
2
3
4
5         859   481
*/

typedef struct {
    const int AirValue;    // the value which is returned when the sensor is dry
    const int WaterValue;  // the value which is returned when the sensor is submerged in water
    const uint8_t gpio;    // gpio number to which the sonsorr is connected - must be an analog port
    const uint8_t label;   // number which is hand written on the sensor to be able to distinguish them
} sensor_t;

#if defined(ESP8266)
static sensor_t sensors[1] = { { 859, 481, 0, 5 } };
#elif defined(ESP32)
static sensor_t sensors[3] = { { 0, 0, 36, 2 }, { 0, 0, 39, 3 }, { 0, 0, 34, 4 } };
#endif
#define MAX_SENSOR_COUNT 16;
#endif //SENSORCONFIG_H
