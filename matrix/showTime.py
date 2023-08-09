#!/home/pi/bin/python
# -*- coding: utf-8 -*-

import threading
from datetime import *
import time as timetosleep
import ctypes

import pytz
from luma.core.render import canvas
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT
import requests
import constants

device_wrapper = [None]
sensors=[]
HOME_ASSISTANT_TOKEN="secret"

def showSensorData(self):
    for sensor_entity_id in self.sensors:
        home_assistant_url = "192.168.0.6"
        api_endpoint = f"http://{home_assistant_url}:8123/api/states/{sensor_entity_id}"
       # print(self.HOME_ASSISTANT_TOKEN)
       # print(api_endpoint)
        headers = {
            "Authorization": f"Bearer {self.HOME_ASSISTANT_TOKEN}"
        }
        response = requests.get(api_endpoint, headers=headers)
        if response.status_code == 200:
            sensor_data = response.json()
            show_message(device_wrapper[0],sensor_data["attributes"]["friendly_name"]+" "+ sensor_data["state"] +" "+sensor_data["attributes"]["unit_of_measurement"], fill="white", font=proportional(CP437_FONT))
        else:
            print(f"Error: Request failed with status code {response.status_code}")


def lumaPrintSingleText(input):
    with canvas(device_wrapper[0]) as draw:
        text(draw, (0, 0), input, fill="white",
             font=proportional(CP437_FONT))


class showTime(threading.Thread):

    def __init__(self, name, device,token,sensors):
        threading.Thread.__init__(self)
        self.name = name
        device_wrapper[0]=device
        self.HOME_ASSISTANT_TOKEN = token
        self.sensors=sensors
    def run(self):
        print('Clock thread started')
        berlin_timezone = pytz.timezone("Europe/Berlin")

        try:
            counter =0
            while True:
                if counter<constants.INTERVAL_BETWEEN_SENSOR_DATA/constants.INTERVAL_BETWEEN_CLOCK_UPDATES:
                    counter+=1
                    t = datetime.now(berlin_timezone)
                    lumaPrintSingleText(str(t.hour).zfill(2) + ":" + str(t.minute).zfill(2))

                else:
                    counter=0
                    showSensorData(self)
                print(counter)
                timetosleep.sleep(constants.INTERVAL_BETWEEN_CLOCK_UPDATES)

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
