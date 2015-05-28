import zmq
import time

context = zmq.Context()

control = context.socket(zmq.PUB)
control.bind("tcp://*:5555")

time.sleep(1)

flight_speeds=[2,4]
fog_min = [10,20]
replicates = 3

trial_duration = 60
rest_duration = 60

for k in range(replicates):
    for i in flight_speeds:
      for j in fog_min:
        control.send("START {} {} {}".format(i, j, 2*j))
        print("Sent start")
        time.sleep(trial_duration)
        control.send("STOP")
        print("Sent stop")
        time.sleep(rest_duration)

time.sleep(1)
control.send('END')
