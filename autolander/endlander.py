import serial
import time
s = serial.Serial('/dev/ttyACM0',9600)

begtime = time.time()


while(time.time()-begtime < 5):
  s.write('U')

