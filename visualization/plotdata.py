#!/usr/bin/env python
import pandas as pd
import matplotlib.pyplot as plt

d = pd.read_csv('DATA.csv', sep=' ', names=['id', 'val', 't'])

torque = d[d['id']=='rawtorque']
abdo = d[d['id']=='centroid_x']

torque['val_norm']=(torque['val']-torque['val'].mean())/torque['val'].std()
abdo['val_norm']=(abdo['val']-abdo['val'].mean())/abdo['val'].std()

plt.plot(torque['t'], torque['val_norm'], '-r')
plt.plot(abdo['t'], abdo['val_norm'], '-k')
plt.xlabel('Time (s)')
plt.show()
