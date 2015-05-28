import numpy as np
import zmq
import serial
import re
import time
import pandas as pd

# Set up the ZeroMQ context
context = zmq.Context()

# Connect to the main control signal
receiver = context.socket(zmq.SUB)
receiver.connect("tcp://localhost:5555")
receiver.setsockopt(zmq.SUBSCRIBE,"")

# Bind to publishing socket
publisher = context.socket(zmq.PUB)
publisher.bind("tcp://*:5556")

# Arduino connection -- see psdamp.ino for arduino code
port = '/dev/ttyACM0'
baud_rate = 115200

# Connect to the serial port and flush the input
ser = serial.Serial(port, baud_rate, timeout=0.1)
ser.read(ser.inWaiting())

# Do initial handshake with Arduino board. The Arduino is writing the
# character 'A' to the serial port to indicate that it is ready, when it
# receives a character it will start accepting commands.
while ser.inWaiting()==0:
  pass

ser.write('X') # Tell Arduino we're ready for data (any character will work)
ser.flushInput()

# Main Loop
while True:
  # Poll receiver for start command
  msg = receiver.recv()
  print('Received: ' + msg)

  if msg[:5]=="START":
    ser.write("T")
    ser.flush()

    while True:
      # Check if any messages have come from control
      try:
        msg = receiver.recv(zmq.NOBLOCK)
        #receiver.send("TORQUE STOP")
      except zmq.ZMQError:
        msg = ""

      # If we get a stop message, break out
      if msg=='STOP':
        ser.write('D')
        ser.flush()
        break

      # Otherwise, get some data and publish it
      d = ser.readline()
      print(d)
      publisher.send('torque {}'.format(d))

  elif msg=='END':
    break

# Close down the serial connection
ser.close()
