# Himbeergarten Matrix Utils
This module can be controlled by the app in the repositry [Himbeergarten_App](https://github.com/tobiasjungmann/Himbeergarten_App)
It displays the time, currently played songs, information about the weather or MVV departure information on a MAX7219 LED Matrix.
Additionally, GPIOs can be switched on and off, for example in combination with relais to control external outlets as well as lights. 

## Setup
### Install Libraries
Setup "spotipy": ``sudo -H pip install spotipy``

Setup "MVV API": ``sudo -H pip install mvg-api``

Setup "netifaces": ``sudo -H pip install netifaces``

Setup "Python_Weather": ``sudo -H pip install python-weather``

Setup "GPIOs": ``pip install RPi.GPIO``

Setup "gRPC": ``sudo -H pip install grpcio-tools``

### Setup Libraries

Setup MAX7219 Library: Follow the steps in the [installation manual](https://luma-led-matrix.readthedocs.io/en/latest/install.html) and connect the matrix to the correct GPIO Pins.

Add client ID: ``export SPOTIPY_CLIENT_ID='your-spotify-client-id'``

Add client Secret: ``export SPOTIPY_CLIENT_SECRET='your-spotify-client-secret'``

Add redirect URL:``export SPOTIPY_REDIRECT_URI='your-app-redirect-url'`` (can be found under edit in the spotify developer dashboard)


## Run the Program
To prevent any complications with the spotify authentication, ideally start the programm when the RPi is connected to a monitor and has a network connection.

Manually start with: `python rpiReceiver.py`

Autostart with cronjobs:
`sudo crontab -e `
and add the lines in the end (important: last line in the file is not executed, so add a blank line):

``PYTHONPATH=/home/pi/.local/lib/python3.9/site-packages``

``@reboot python /home/pi/Himbeergarten_RPi/rpiReceiver.py > /home/pi/logs/himbeergarten_log.txt``

Save file and reboot RPi.


## Used Open Source Projects

[Python-Weather](https://github.com/vierofernando/python-weather) (MIT-License)

[MVG-API](https://github.com/leftshift/python_mvg_api) (MIT-License)

[Netifaces](https://github.com/al45tair/netifaces) (MIT-License)

[Luma.LED_Matrix: Display drivers for MAX7219](https://luma-led-matrix.readthedocs.io/en/latest/intro.html) (MIT-License)

[Python GPIO](https://sourceforge.net/projects/raspberry-gpio-python/) (MIT-License)
