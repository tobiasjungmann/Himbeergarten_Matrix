#define GPIO_POTI 36
#define GPIO_SWITCH_SPOTIFY 19
#define GPIO_SWITCH_WEATHER_OUTSIDE 18
#define GPIO_SWITCH_WEATHER_INSIDE 5
#define GPIO_SWITCH_TIME 17

#if defined(ESP8266)
#define CLK_PIN 6   // or SCK
#define CS_PIN 7    // or SS
#define DATA_PIN 8  // or MOSI
#elif defined(ESP32)
#define CLK_PIN 12   // or SCK
#define CS_PIN 14    // or SS
#define DATA_PIN 27  // or MOSI
#endif