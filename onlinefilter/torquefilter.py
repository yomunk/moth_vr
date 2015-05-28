import sys
import zmq
import numpy as np
from scipy.signal import iirfilter
import re

context=zmq.Context()
b,a = iirfilter(4, 0.015, btype='lowpass', analog=False, ftype='butter')

subscriber=context.socket(zmq.SUB)
subscriber.connect("tcp://localhost:5555")
subscriber.connect("tcp://localhost:5556")
subscriber.setsockopt(zmq.SUBSCRIBE, "torque")
subscriber.setsockopt(zmq.SUBSCRIBE, "END")

publisher=context.socket(zmq.PUB)
publisher.bind('tcp://*:5557')

gain=-2.2

input_buffer=np.zeros(len(a))
output_buffer=np.zeros(len(b))

while True:

  msg = subscriber.recv()

  if msg=="END":
      break
  else:

      try:
         input_buffer[0] = gain * float(msg.split()[1])
      except:
         continue
      input_buffer=np.roll(input_buffer, 1)
      output_buffer=np.roll(output_buffer, 1)


      output_buffer[0] = np.dot(b, input_buffer) -\
              np.dot(a[1:], output_buffer[1:])

      msg = "filt_torque {}".format(output_buffer[0])

      publisher.send(msg)




