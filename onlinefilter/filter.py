import sys
import zmq
import numpy as np
from scipy.signal import iirfilter
import re

if len(sys.argv) != 3:
    print("Usage: onlinefilter.py <input_topic> <output_topic>")
    sys.exit()

input_topic = sys.argv[1]
output_topic = sys.argv[2]

context=zmq.Context()
b,a = iirfilter(4, 0.1, btype='lowpass', analog=False, ftype='butter')

subscriber=context.socket(zmq.SUB)
subscriber.connect("tcp://127.0.0.1:5556")
subscriber.setsockopt(zmq.SUBSCRIBE, input_topic)

publisher=context.socket(zmq.PUB)
publisher.bind('tcp://*:5561')


input_buffer=np.zeros(len(a))
output_buffer=np.zeros(len(b))

while True:
  y=re.findall(r'-?\d+', subscriber.recv())[0]

  input_buffer=np.roll(input_buffer, 1)
  output_buffer=np.roll(output_buffer, 1)

  input_buffer[0] = y
  output_buffer[0] = np.dot(b, input_buffer) -\
          np.dot(a[1:], output_buffer[1:])

  print(y, output_buffer[0])
  msg = "{} {}".format(output_topic, output_buffer[0])
  print msg

  publisher.send(msg)




