#!/home/pi/bin/python
# -*- coding: utf-8 -*-

import threading
import time as timetosleep
import ctypes
from mvg_api import *

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas
from luma.core.virtual import viewport
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

device_wrapper = [None]
startingPoint = "Josef Wirt Weg"
destination = "Dachau"


# shows the time while running
class showMVV(threading.Thread):
    def __init__(self, name, device, start, dest):
        threading.Thread.__init__(self)
        self.name = name
        device_wrapper[0] = device
        self.startingPoint = start
        self.destination = dest

    def run(self):

        # target function of the thread class
        print('MVV thread started start: ', startingPoint,'  destination: ',destination)
        try:
            while True:
                while True:
                    routes = get_route(get_id_for_station(startingPoint), get_id_for_station(destination), time=None,
                                       arrival_time=False, max_walk_time_to_start=None, max_walk_time_to_dest=None,
                                       change_limit=None, ubahn=True, bus=True, tram=True, sbahn=True)

                    show_message(device_wrapper[0], startingPoint + "  ->  " + destination, fill="white",
                                 font=proportional(CP437_FONT))

                    for route in routes:
                        departure = route['departure_datetime']
                        arrival = route['arrival_datetime']
                        with canvas(device_wrapper[0]) as draw:
                            text(draw, (0, 0), str(departure.hour).zfill(2) + ":" + str(departure.minute).zfill(2),
                                 fill="white", font=proportional(CP437_FONT))
                        timetosleep.sleep(3)
                        with canvas(device_wrapper[0]) as draw:
                            device_wrapper[0].contrast(50)
                            text(draw, (0, 0), str(arrival.hour).zfill(2) + ":" + str(arrival.minute).zfill(2),
                                 fill="white", font=proportional(CP437_FONT))
                        timetosleep.sleep(1)
                        with canvas(device_wrapper[0]) as draw:
                            device_wrapper[0].contrast(200)
        finally:
            print('MVV thread ended')

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
