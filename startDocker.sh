#!/bin/bash

if lsmod | grep -i spi >/dev/null;
then 
  echo "SPI is enabled"
  sudo docker build -t matrix_utils .
  sudo docker run -it --rm --privileged --name matrix-container matrix_utils
else 
  echo "Error: SPI is not enabled. ENable it via rpi config -> Interfaces -> SPI or \"sudo raspi-config nonint do_spi 0\" and reboot afterwards"
fi
