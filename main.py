#!/home/pi/bin/python
# -*- coding: utf-8 -*-
from concurrent import futures

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop

import RPi.GPIO as GPIO

import grpc

from matrix.showSpotify import showSpotify
from matrix.showWeather import show_weather
from proto.matrix_pb2 import GPIOReply, StatusReply, EmptyMsg, MatrixState, MatrixChangeModeReply
from proto.matrix_pb2_grpc import MatrixServicer, add_MatrixServicer_to_server
from matrix.showTime import showTime
import argparse
import os
import constants

outlets_gpio = [constants.OUTLET_1_GPIO, constants.OUTLET_2_GPIO, constants.OUTLET_3_GPIO, constants.ARDUINO1_GPIO, constants.ARDUINO2_GPIO]
outlets_state = [False, False, False, False, False]
matrix_thread_array = [None]
matrix_state = [MatrixState.MATRIX_TIME]

serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=4, block_orientation=-90, rotate=0, blocks_arranged_in_reverse_order=False)
brightness = 255
HOME_ASSISTANT_TOKEN=os.environ["HOME_ASSISTANT_TOKEN"]
sensors=["sensor.ewelink_th01_temperature"]

class MatrixServicer(MatrixServicer):
    def outletOn(self, request, context):
        print("Received OutletRequest for ", request.id)
        if request.on:
            GPIO.output(outlets_gpio[request.id], GPIO.HIGH)
        else:
            GPIO.output(outlets_gpio[request.id], GPIO.LOW)
        outlets_state[request.id] = request.on
        return GPIOReply(statusList=outlets_state)

    def getStatus(self, request, context):
        return StatusReply(gpios=outlets_state, matrixState=matrix_state[0],matrixBrightness=brightness)

    def matrixSetActivated(self, request, context):
        return EmptyMsg()

    def matrixSetSensorNames(self, request, context):
        self.sensors=request.sensors
        return EmptyMsg()

    def matrixSetMode(self, request, context):
        if matrix_thread_array[0] is not None:
            matrix_thread_array[0].raise_exception()
            matrix_thread_array[0].join()
            matrix_thread_array[0] = None
        matrix_state[0]=request.state
        if request.state == MatrixState.MATRIX_TIME:
            matrix_thread_array[0] = showTime('Thread 1', device,HOME_ASSISTANT_TOKEN,sensors)
        elif request.state == MatrixState.MATRIX_WEATHER:
            matrix_thread_array[0] = show_weather('Thread 1', device)
        elif request.state == MatrixState.MATRIX_SPOTIFY:
            matrix_thread_array[0] = showSpotify('Thread 1', device)
        elif request.state == MatrixState.MATRIX_MVV:
            print("Currently not available - waiting for new API")
        elif request.state == MatrixState.MATRIX_QUIT:
            exit()

        if request.state == MatrixState.MATRIX_STANDBY:
            device.cleanup()
        else:
            matrix_thread_array[0].start()

        return MatrixChangeModeReply(state=request.state)

    def matrixSetBrightness(self, request, context):
        return EmptyMsg()


def serve(address,port):
    print("Starting Server...")
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    add_MatrixServicer_to_server(
        MatrixServicer(), server)
    concataddress = address + ":" + str(port)
    print("Server at: ", concataddress)
    server.add_insecure_port(concataddress)
    server.start()
    print("Waiting for connections...")
    server.wait_for_termination()
    print("Terminated.")


def init_gpios():
    GPIO.setmode(GPIO.BCM)

    GPIO.setup(constants.RELAIS1_GPIO, GPIO.OUT)
    GPIO.setup(constants.RELAIS2_GPIO, GPIO.OUT)
    GPIO.setup(constants.ARDUINO1_GPIO, GPIO.OUT)
    GPIO.setup(constants.ARDUINO2_GPIO, GPIO.OUT)

    for outlet in outlets_gpio:
        GPIO.setup(outlet, GPIO.OUT)
        GPIO.output(outlet, GPIO.LOW)

    GPIO.output(constants.RELAIS1_GPIO, GPIO.LOW)
    GPIO.output(constants.RELAIS2_GPIO, GPIO.LOW)
    GPIO.output(constants.ARDUINO1_GPIO, GPIO.LOW)
    GPIO.output(constants.ARDUINO2_GPIO, GPIO.LOW)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, help="Define the port of the API")
    args = parser.parse_args()
    if args.port is not None:
        print(f"Port number provided: {args.port}")
        init_gpios()
        matrix_thread_array[0] = showTime('Thread 1', device,HOME_ASSISTANT_TOKEN,sensors)
        matrix_thread_array[0].start()
        serve("0.0.0.0", args.port)
    else:
        print("Port number not provided.")

