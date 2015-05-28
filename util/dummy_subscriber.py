import zmq
import matplotlib.pyplot as plt
import numpy as np

context = zmq.Context()
subscriber = context.socket(zmq.SUB)
subscriber.connect("tcp://localhost:5561")
subscriber.setsockopt(zmq.SUBSCRIBE, "")

while True:
  msg = subscriber.recv()
  print(msg)
