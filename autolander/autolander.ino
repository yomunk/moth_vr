

#include <Servo.h> 
 
Servo autolander;  // create servo object to control a servo 
int incomingByte;      // a variable to read incoming serial data into
int high_angle = 175;
int low_angle = 60;


void setup() {
  // initialize serial communication:
  Serial.begin(9600); 
  autolander.attach(9);  // attaches the servo on pin 9 to the servo object 
  autolander.write(high_angle);
}

void loop() {
  // see if there's incoming serial data:
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    if (incomingByte == 'U') {
        autolander.write(high_angle);
        delay(15);
    } 
    if (incomingByte == 'D') {
      autolander.write(low_angle);                  // sets the servo position according to the scaled value 
      delay(15);                           // waits for the servo to get there 
  }
  }
}

