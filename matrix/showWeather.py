#!/home/pi/bin/python
# -*- coding: utf-8 -*-

import threading
import time as timetosleep
import ctypes

import asyncio
import python_weather

from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

device_wrapper = [None]

# Loosly follows https://github.com/vierofernando/python-weather
async def getweather(device):
    client = python_weather.Client(format=python_weather.METRIC)
    weather = await client.find("Munich")
    weatherString = str(weather.current.temperature) + "°C " + str(weather.current.feels_like) + " " + str(
        weather.current.wind_display)
    show_message(device, weatherString, fill="white", font=proportional(CP437_FONT))#, scroll_delay=0.02)


# show song title between time
class show_weather(threading.Thread):
    def __init__(self, name, device):
        threading.Thread.__init__(self)
        self.name = name
        device_wrapper[0]=device

    def run(self):
        print('Weather thread stated')
        try:
            while True:
                loopa = asyncio.new_event_loop()
                loopa.run_until_complete(getweather(device_wrapper[0]))
                timetosleep.sleep(2)
        finally:
            print('Weather thread ended')

    def get_id(self):
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
