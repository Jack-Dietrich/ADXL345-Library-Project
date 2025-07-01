#include <Arduino.h>
#include <SPI.h> //for SPI to ADXL345

/*
Pins on ESP32 Devkit V1
MOSI: 23
MISO: 19
SCK: 18
SS:5

*/

#define ss 10 //slave select on pin 10

// put function declarations here:

int adxl345_init(){

  /*
  CS most go low at start of transmission, high at end of transmission
  MODE should be 3 as both CPOL and CPHA are 1
  */
  Serial.begin(115200);
  pinMode(ss,OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);//unsure if this is the correct way, unable to find in documentation so far...
  SPI.setDataMode(3); //mode 3 as CPOL and CPHA 1
  digitalWrite(ss,LOW);//START transmission
  SPI.transfer(0X31); //want to clear bit in DATA_FORMAT register in order to select 4 wire SPI
  

  digitalWrite(ss,HIGH);//end transmission
}


void setup() {
  adxl345_init();
  
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
