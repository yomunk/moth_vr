import zmq
import time

context = zmq.Context()

control = context.socket(zmq.PUB)
control.bind("tcp://*:5555")

time.sleep(1)

def start(conditions):
    speed, fog_min = conditions
    control.send("START {} {} {}".format(speed, fog_min, 2*fog_min))

conditions = {'1': (2, 4), '2': (2, 8), '3': (4,4), '4': (4,8)}

while True:
    cmd = raw_input('Ready: ')
    if cmd in conditions.keys():
        start(conditions[cmd])
        raw_input('Press any key to stop: ')
        control.send("STOP")
    else:
        control.send("STOP")
        break

time.sleep(1)
control.send('END')
