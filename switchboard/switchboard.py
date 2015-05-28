import zmq

context = zmq.Context()

virtualworld = context.socket(zmq.REP)
virtualworld.bind('tcp://*:5560')

expt = context.socket(zmq.SUB)
expt.connect('tcp://localhost:5555')
expt.setsockopt(zmq.SUBSCRIBE,"")

torque = context.socket(zmq.SUB)
torque.connect('tcp://localhost:5557')
torque.setsockopt(zmq.SUBSCRIBE,"")

abdomen = context.socket(zmq.SUB)
abdomen.connect('tcp://localhost:5559')
abdomen.setsockopt(zmq.SUBSCRIBE,"")

poller = zmq.Poller()
poller.register(expt, zmq.POLLIN)
poller.register(virtualworld, zmq.POLLIN)
poller.register(torque, zmq.POLLIN)
poller.register(abdomen, zmq.POLLIN)

control_vars = {'torque': 0, 'abdomen': 0}

while True:
    try:
        socks=dict(poller.poll())
    except KeyboardInterrupt:
        break

    if expt in socks:
        msg = expt.recv()
        if msg=="END":
            break

    if torque in socks:
        control_vars['torque'] = torque.recv().split()[1]

    if abdomen in socks:
        control_vars['abdomen'] = abdomen.recv().split()[1:]

    if virtualworld in socks:
        msg = virtualworld.recv()
        virtualworld.send(str(control_vars[msg]))

