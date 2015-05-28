screen -d -m -S MothVR
screen -S MothVR -t torquelander -X screen python torquelander/torquelander.py
screen -S MothVR -t torquefilter -X screen python onlinefilter/torquefilter.py
screen -S MothVR -t switchboard -X screen python switchboard/switchboard.py
screen -S MothVR -t controller -X screen python util/dummy_control.py
screen -S MothVR -t visualization -X screen python visualization/plotdata.py

screen -r MothVR
