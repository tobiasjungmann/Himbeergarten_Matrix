#!/home/pi/bin/python
# -*- coding: utf-8 -*-

import threading
from datetime import *
import time as timetosleep
import ctypes

from luma.core.render import canvas
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

device_wrapper = [None]
class showTime(threading.Thread):

    def __init__(self, name, device):
        threading.Thread.__init__(self)
        self.name = name
        device_wrapper[0]=device

    def run(self):
        print('Clock thread started')
        try:
            while True:
                t = datetime.now(tz=None)
                with canvas( device_wrapper[0]) as draw:
                    text(draw, (0, 0), str(t.hour).zfill(2) + ":" + str(t.minute).zfill(2), fill="white",
                         font=proportional(CP437_FONT))
                timetosleep.sleep(1)
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
