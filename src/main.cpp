#include <Arduino.h>
#include <SPI.h> //for SPI to ADXL345

/*
Pins on ESP32 Devkit V1
MOSI: 23
MISO: 19
SCK: 18
SS:5

--Logic Analyzer channel info--
ch0 - SS
ch1 -miso/sdo
ch2 - mosi/sda
ch3 - scl

*/

//defining hspi and vspi

#define VSPI 3


//pin delcarations

const int VSPI_MISO = 19;
const int VSPI_MOSI = 23;
const int VSPI_SCLK = 18;
const int VSPI_SS = 5;


// put function declarations here:

SPIClass vspi = SPIClass(VSPI);


void setup() {
  /*
  Notes:

  */

  vspi.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
  pinMode(VSPI_SS,OUTPUT);
  digitalWrite(VSPI_SS,HIGH);//slave selct should be active lo, so go high asap

  Serial.begin(115200);

  vspi.setDataMode(3); //mode 3 as CPOL and CPHA 1.
  digitalWrite(VSPI_SS,LOW);//START transmission

  //read from register 31
  vspi.transfer(0xB1);//read register 31(MSB is a 1)

  byte buff;

  int num = 1;//for testing
  
  buff = vspi.transfer(0x00);//transfer 0 in order to prompt for data?





  //manipulate to clear bit d6(xor with 1 in bit d6)
  buff ^= (1 << 6);
  //write back to register 0x31
  vspi.transfer(buff); //want to clear bit in DATA_FORMAT register in order to select 4 wire SPI, first 2 bits can be 0 as writing only one byte

  digitalWrite(VSPI_SS,HIGH);//end transmission
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
