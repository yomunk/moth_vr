import serial

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

print(ser.read(10))

ser.write("A")

ser.write("U")

data = []

while len(data)<1000:
  data.append(ser.readline())

ser.write("D")
ser.close()


