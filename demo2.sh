#!/bin/sh

cd virtual_forest/scripts
./run_forest.sh &
cd ../..
python torquelander/torquelander.py &
python onlinefilter/torquefilter.py &
python switchboard/switchboard.py &
python util/dummy_control.py &
python visualization/plotdata.py &


