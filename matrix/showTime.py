#!/home/pi/bin/python
# -*- coding: utf-8 -*-

import threading
from datetime import *
import time as timetosleep
import ctypes

import pytz
from luma.core.render import canvas
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT
import requests


device_wrapper = [None]
sensors=["sensor.ewelink_th01_temperature"]
HOME_ASSISTANT_TOKEN="secret"
def showSensorData(token):
    for sensor_entity_id in sensors:
        home_assistant_url = "http://192.168.0.6:8123"
      #  sensor_entity_id = "sensor.ewelink_th01_temperature"
        api_endpoint = f"{home_assistant_url}/api/states/{sensor_entity_id}"
        print(HOME_ASSISTANT_TOKEN)
        print(api_endpoint)
        headers = {
            "Authorization": f"Bearer {token}"
        }
        response = requests.get(api_endpoint, headers=headers)
        if response.status_code == 200:
            sensor_data = response.json()
         #   print("Sensor Data:")
         #   print(sensor_data)
         #   print(sensor_data["attributes"]["friendly_name"])
         #   print(sensor_data["state"])
            print(sensor_data["attributes"]["unit_of_measurement"])
            show_message(device_wrapper[0],sensor_data["attributes"]["friendly_name"]+" "+ sensor_data["state"] +" "+sensor_data["attributes"]["unit_of_measurement"], fill="white", font=proportional(CP437_FONT))
        else:
            print(f"Error: Request failed with status code {response.status_code}")


def lumaPrintSingleText(input):
    with canvas(device_wrapper[0]) as draw:
        text(draw, (0, 0), input, fill="white",
             font=proportional(CP437_FONT))


class showTime(threading.Thread):

    def __init__(self, name, device,token):
        threading.Thread.__init__(self)
        self.name = name
        device_wrapper[0]=device
        self.HOME_ASSISTANT_TOKEN = token
    def run(self):
        print('Clock thread started')
        berlin_timezone = pytz.timezone("Europe/Berlin")
        pause_duration=1  # pause between updates in seconds
        interval_sensor_info=3 # show the sensor info every x seconds
        try:
            counter =0
            while True:
                if counter<interval_sensor_info/pause_duration:
                    counter+=1
                    t = datetime.now(berlin_timezone)
                    lumaPrintSingleText(str(t.hour).zfill(2) + ":" + str(t.minute).zfill(2))

                else:
                    counter=0
                    showSensorData(self.HOME_ASSISTANT_TOKEN)
                timetosleep.sleep(pause_duration)

        finally:
            print('Clock thread ended')

    def get_id(self):

        # returns id of the respective thread
        if hasattr(self, '_thread_id'):
            return self._thread_id
        for id, thread in threading._active.items():
            if thread is self:
                return id

    def raise_exception(self):
        thread_id = self.get_id()
        res = ctypes.pythonapi.PyThreadState_SetAsyncExc(thread_id,
                                                         ctypes.py_object(SystemExit))
        if res > 1:
            ctypes.pythonapi.PyThreadState_SetAsyncExc(thread_id, 0)
            print('Exception raise failure')
