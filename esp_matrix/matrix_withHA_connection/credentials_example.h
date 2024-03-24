#ifndef CRED_H
#define CRED_H
/**
* Example format for teh credentials file
* rename to credentails.h and fill in own values.
* credentials.h is in the .gitignore file
*/

// Wifi
static const char *WIFI_SSID = "name_of_your_wifi";
static const char *WIFI_PASSWORD = "password_of_your_wifi";

static const char* OPEN_WEATHER_API_KEY = "open_weather_api_key";
static const char OPEN_WEATHER_CITY[] = "Munich";
static const char OPEN_WEATHER_COUNTRY_CODE[] = "DE";

// Spotify
static const char SPOTIFY_CLIENT_ID[] = "spotify_client_id";
static const char SPOTIFY_CLIENT_SECRET[] = "spotify_client_secret";
static const char SPOTIFY_REFRESH_TOKEN[] =  "spotify_refresh_token";

static const IPAddress PHONE_IP(192, 168, 178, 59);

// Homeassistant
static const char* HA_IP_ADDRESS = "192.168.178.61";
static const char* HA_TEMP_SENSOR = "sensor.ewelink_th01_temperature";
static const char* HA_HUMIDITY_SENSORS[1] = {"sensor.esp_4c_11_ae_13_f2_ef_hum_5_percent"}; // query from the list of existant sensors once available
static const char* HA_TOKEN = "homeassistant_token";

#endif // CRED_H