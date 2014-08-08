indicator=A

for ((i = 1; i <=5; i++))
do

screen -d -m -S mothVR torquemeter/torquemeter.out
screen -S mothVR -X screen python onlinefilter/filter.py centroid_x filterx
screen -S mothVR -X screen switchboard/switchboard.out
screen -S mothVR -X screen python autolander/autolander.py
screen -S mothVR -X screen data_recorder/data_recorder.out   #datagatherer
screen -S mothVR -X screen abdotracker/cpp/abdotracker.out
screen -S mothVR -X screen ./run_forest.sh

sleep 60s
pkill screen
python ~/Projects/autolander/endlander.py
cp ~/Projects/DATASTREAM/DATA.csv ~/zach/$indicator$i.csv
sleep 120s

done
#forest
#screen -d -m -S lander ~/Projects/virtual_forest/scripts/endauto.bash


exit 0



