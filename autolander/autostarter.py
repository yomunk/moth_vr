#!/usr/bin/env python

import serial
import time




serial = serial.Serial('/dev/ttyACM0', 9600)
serial.write('D')
time.sleep(10)

