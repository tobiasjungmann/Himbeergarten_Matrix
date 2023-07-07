# Himbeergarten RPi

## Humidity Utils
This module controls the humidity level of indoor plants and writes these values in regular intervals into a Firebase database.
Therefore, multiple Arduinos can be connected to a central RaspberryPi.

## Matrix Utils
Displays time, currently played songs and departure information on a MAX7219 LED Matrix.
Additionally, relais can be connected to control external outlets as well as lights. 

## Setup
### Install Libraries
Setup "spotipy": ``sudo -H pip install spotipy``

Setup "MVV API": ``sudo -H pip install mvg-api``

Setup "netifaces": ``sudo -H pip install netifaces``

Setup "Python_Weather": ``sudo -H pip install python-weather``

Setup "Firebase": ``sudo -H pip install firebase-admin``

Setup "GPIOs": ``pip install RPi.GPIO``

Setup "gRPC": ``sudo -H pip install grpcio-tools``

### Setup Libraries

Setup MAX7219 Library: Follow the steps in the [installation manual](https://luma-led-matrix.readthedocs.io/en/latest/install.html) and connect the matrix to the correct GPIO Pins.

Configure the Firebase by adding the authentification credentials: Download the credentials and specifiy the path in an environment variable: ``export GOOGLE_APPLICATION_CREDENTIALS=""/path/to/service-account-file.json"``

Add client ID: ``export SPOTIPY_CLIENT_ID='your-spotify-client-id'``

Add client Secret: ``export SPOTIPY_CLIENT_SECRET='your-spotify-client-secret'``

Add redirect URL:``export SPOTIPY_REDIRECT_URI='your-app-redirect-url'`` (can be found under edit in the spotify developer dashboard)


## Run the Program
To prevent any complications with the spotify authentication, ideally start the programm when the RPi is connected to a monitor and has a network connection.

Manually start with: python3 rpiReceiver.py

Autostart with cronjobs:
`sudo crontab -e `
and add the lines in the end (important: last line in the file is not executed, so add a blank line.):

``PYTHONPATH=/home/pi/.local/lib/python3.9/site-packages``

``@reboot python /home/pi/Himbeergarten_RPi/rpiReceiver.py > /home/pi/logs/himbeergarten_log.txt``

Save file and reboot RPi.


## Used Open Source Projects

[Python-Weather](https://github.com/vierofernando/python-weather) (MIT-License)

[MVG-API](https://github.com/leftshift/python_mvg_api) (MIT-License)

[Netifaces](https://github.com/al45tair/netifaces) (MIT-License)

[Luma.LED_Matrix: Display drivers for MAX7219](https://luma-led-matrix.readthedocs.io/en/latest/intro.html) (MIT-License)

[Python GPIO](https://sourceforge.net/projects/raspberry-gpio-python/) (MIT-License)
