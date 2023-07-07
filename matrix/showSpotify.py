#!/home/pi/bin/python
# -*- coding: utf-8 -*-

import threading
from datetime import *
import time as timetosleep
import ctypes
import spotipy

from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

device_wrapper = [None]
spotify_scope = 'user-read-currently-playing'
spotify_username = "invalid"
spotify_token = "invalid"  # spotipy.util.prompt_for_user_token(username, scope, "http://127.0.0.1:8080/callback")


# show song title between time
class showSpotify(threading.Thread):
    def __init__(self, name, device):
        threading.Thread.__init__(self)
        self.name = name
        device_wrapper[0]=device

    def run(self):
        print('Spotify thread started')
        try:
            while True:
                sp = spotipy.Spotify(auth=spotify_token)
                current_song = sp.current_user_playing_track()
                if current_song != None:
                    print(current_song['item']['name'])
                    show_message(device_wrapper[0], current_song['item']['name'], fill="white", font=proportional(CP437_FONT))
                for i in range(0, 20):
                    t = datetime.now(tz=None)
                    text(device_wrapper[0], (0, 0), str(t.hour).zfill(2) + ":" + str(t.minute).zfill(2), fill="white",
                        font=proportional(CP437_FONT))
                    timetosleep.sleep(1)
        finally:
            print('Spotify thread ended')

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