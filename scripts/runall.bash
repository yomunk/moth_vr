mothname="moth17posgain_"
picname="pictures"
datenow=$(date +%Y-%m-%d)
mkdir ~/YOGONE/$datenow
for ((i = 1; i <=60; i++))
do
rm -R frames
mkdir frames
rm DATA.csv
screen -d -m -S mothVR torquemeter/torquemeter.out
screen -S mothVR -X screen python onlinefilter/filter.py centroid_x filterx
screen -S mothVR -X screen switchboard/switchboard.out
screen -S mothVR -X screen python autolander/autolander.py
screen -S mothVR -X screen data_recorder/a.out   #datagatherer
screen -S mothVR -X screen abdotracker/cpp/abdotracker.out
screen -S mothVR -X screen ./run_forest.sh

sleep 60s
pkill screen
python ~/Projects/autolander/endlander.py
cp DATA.csv ~/YOGONE/$datenow/$mothname$i.csv
mkdir ~/YOGONE/$datenow/$mothname$i$picname
cp -R frames ~/YOGONE/$datenow/$mothname$i$picname
mail -s $mothname$i zacharyhorvitz@gmail.com < timeelapsed.txt

sleep 120s

done

#forest
#screen -d -m -S lander ~/Projects/virtual_forest/scripts/endauto.bash

echo "finished"
exit 0



