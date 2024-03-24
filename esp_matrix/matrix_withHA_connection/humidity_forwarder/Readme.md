# Forward Humidity - Library
WIP - export into another repository
Requests all sensors which should be read with their corresponding GIPO number form a HA instance.
Reads the states and forwards them to the HA

can be integrated in other esp projects in order to reduce the overall amount of devices

File Structure:
THis repo must be cloned into the repository in the directory where it will be used.
This directory must contain a file credentials.h with the following content:

```c
static const char *WIFI_SSID = "name_of_your_wifi";
static const char *WIFI_PASSWORD = "1234567890";
```