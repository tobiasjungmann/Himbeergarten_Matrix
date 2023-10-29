#define GPIO_POTI 36

// Switches Labeld from the right (0) to the poti (3) 
#define SWITCH_0 19
#define SWITCH_1 18
#define SWITCH_2 5
#define SWITCH_3 17

#define GPIO_SWITCH_SPOTIFY SWITCH_0
#define GPIO_SWITCH_WEATHER_OUTSIDE SWITCH_1
#define GPIO_SWITCH_WEATHER_INSIDE SWITCH_2
#define GPIO_SWITCH_TIME SWITCH_3

#if defined(ESP8266)
#define CLK_PIN 6   // or SCK
#define CS_PIN 7    // or SS
#define DATA_PIN 8  // or MOSI
#elif defined(ESP32)
#define CLK_PIN 12   // or SCK
#define CS_PIN 14    // or SS
#define DATA_PIN 27  // or MOSI
#endif