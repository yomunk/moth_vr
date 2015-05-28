#include <SPI.h>
#include <Servo.h>

// Channel Selection Bytes
////////////////////////////////////////////////////////////////////////////////////
const byte ch0_code = 0b10010100; // ADC channel 0 - PSD Channel 1
const byte ch1_code = 0b11010100; // ADC channel 1 - PSD Channel 2
const byte ch2_code = 0b10100100; // ADC channel 2 - Currently unused
const byte ch3_code = 0b11100100; // ADC channel 3 - Currently unused
////////////////////////////////////////////////////////////////////////////////////

unsigned int PSD1;
unsigned int PSD2;
unsigned long start_time;
float centroid;
int inByte=0;

Servo autolander;

void setup() {
SPI.begin();
// SPI.setBitOrder(MSBFIRST);
// SPI.setDataMode(SPI_MODE0);
SPI.setClockDivider(SPI_CLOCK_DIV16);
digitalWrite(SS,HIGH);

autolander.attach(9);
autolander.write(170);

Serial.begin(115200);
establishContact();

}
////////////////////////////////////////////////////////////////////////////////////

void loop() {
  if (Serial.available() > 0) {
    inByte = Serial.read();
    if (inByte=='U') {
      start_time = millis();
      autolander.write(60);
      while (Serial.available() == 0) {
        get_data();
      }
      autolander.write(170);
    }
  }
}


void get_data() {
      PSD1 = ads7841(ch0_code);
      PSD2 = ads7841(ch1_code);
      centroid = (float(PSD1)-float(PSD2))/(PSD1+PSD2);
      Serial.print("T: ");
      Serial.print((millis()-start_time)/1000., 3);
      Serial.print("\t PSD1: ");
      Serial.print(PSD1);
      Serial.print("\t PSD2: ");
      Serial.print(PSD2);
      Serial.print("\t Torque: ");
      Serial.println(centroid, 5);
}

void establishContact(){
  while(Serial.available() <= 0) {
    Serial.print('A');
    delay(300);
  }
  Serial.read();
}

// ADS7841 function
////////////////////////////////////////////////////////////////////////////////////
unsigned int ads7841(const byte control){ // Function to read ADS7841
  int bitnum; // Return value
  digitalWrite(SS,LOW); // Activate ADS7841
  SPI.transfer(control); // Transfer control byte
  byte msb = SPI.transfer(0); // Read MSB & LSB
  byte lsb = SPI.transfer(0);
  digitalWrite(SS,HIGH); // Deactivate ADS7841
  msb = msb & 0x7F; // Isolate readings and form final reading
  lsb = lsb >> 3;
  bitnum = (word(msb) << 5) | lsb;
  return bitnum; // Return
}
////////////////////////////////////////////////////////////////////////////////////

