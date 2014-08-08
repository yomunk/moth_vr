import sys
import zmq
import numpy as np
import matplotlib.pyplot as plt
import re

context=zmq.Context()

subscriber=context.socket(zmq.SUB)
subscriber.connect("tcp://127.0.0.1:5556")
subscriber.connect("tcp://127.0.0.1:5557")
#subscriber.setsockopt(zmq.SUBSCRIBE, "")
subscriber.setsockopt(zmq.SUBSCRIBE, 'centroid_x')
subscriber.setsockopt(zmq.SUBSCRIBE, 'x_filt')

raw = []
filt = []

for i in range(1000):
  msg = subscriber.recv()
  topic = msg.split()[0]
  if topic == 'centroid_x':
    raw.append(re.findall(r'\d+', msg)[0])
  elif topic=='x_filt':
    filt.append(re.findall(r'\d+.\d+', msg)[0])

plt.plot(raw, '-k')
plt.plot(filt, '-r')
plt.show()


