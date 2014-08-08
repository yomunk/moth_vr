#!/usr/bin/env python

import serial
import zmq
import re
import string
import time

context = zmq.Context()
socket = context.socket(zmq.SUB)

socket.setsockopt(zmq.SUBSCRIBE, "centroid_y")
socket.connect("tcp://*:5556")

context2 = zmq.Context()
socketpub = context2.socket(zmq.PUB)
socketpub.bind("tcp://*:5566")

s = serial.Serial('/dev/ttyACM0', 9600)

begtime = time.time()
timeend = 0


stopped = False;
value = 0
oldvalue = value
x = 0
while True:
  diffval = 0
  x+= 1
  for x in range(0,10):
     message = socket.recv()
     topic, content  = message.split()
    # map(float, content.strip().split('\r\n'))

     value =int(re.sub(r'\D', '', content))
     if(oldvalue != value and  stopped == False):

       diffval+=1

    # print(diffval)
  timesince = time.time() - begtime
  print(timesince)
  if(diffval < 2  and timesince > 10):
    socketpub.send("STOP NOW")
    print("STOP")
    s.write('U')

    stopped = True
  else:
    s.write('D')
    oldvalue = value
    diffval = 0
    print("")
