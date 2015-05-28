moth_vr
=======

Moth Virtual Reality Software Stack

Setting up a new rig
--

Starting a demo set of trials
--

Run each of the following commands in a separate terminal window.
1. cd virtual_forest/scripts && ./run_forest.sh
2. python torquelander/torquelander.py
3. python onlinefilter/torquefilter.py
4. python util/dummy_control.py
5. (optional) python visualization/plotdata.py

Stack Modules
=============

VR_Forest
---------
The code for running the virtual forest environment is kept in the virtual_forest directory, and is built on top of the Panda3D library. The script <run_forest.sh> is a bash script that ensures that the engine is run on the second display.

Torquelander
------------
Python code that communicates with the Arduino board to control the autolander program and grab data from the PSD.

OnlineFilter
------------
A digital online filter that implements a 4th order Butterworth filter.

Switchboard
---
Accumulates control data from various sources, including (e.g.) torquelander, abdotracker. Listens for requests for updates from the virtual world and provides them.
