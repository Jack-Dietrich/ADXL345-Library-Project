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

//functions for reading and writing to register


/**
@brief read from a register in the adxl345.

@param reg register you want to read from
@param numBytes number of bytes you want to read
@param buff buffer to store the bytes that are being read
*/
void readReg(byte reg, int numBytes, byte buff[]){
  if(numBytes > 1){//if user wants to read more than 1 byte need to enable 
    reg |= (1 << 6);
  }


  //SET READ 
  reg |= (1<<7); //set last bit so we read

  digitalWrite(VSPI_SS,LOW);//start transmission

  //send addr

  vspi.transfer(reg);

  for(int i = 0 ; i < numBytes; i++){//for each byte that we want to read
    buff[i] = vspi.transfer(0x00);//transfer 0 to prompt adxl for new byte of info
  }

  digitalWrite(VSPI_SS,HIGH); //end transmission

}

void writeReg(byte reg, int numBytes, byte buff[]){
  if(numBytes > 1){//if user wants to read more than 1 byte need to enable 
    reg |= (1 << 6);
  }

  //SET WRITE
  reg &= ~(1<<7); //set last bit to 0 so we write



  digitalWrite(VSPI_SS,LOW);//start transmission

  //send reg to adxl
  vspi.transfer(reg);


  for(int i = 0 ; i < numBytes; i++){//for each byte that we want to write
    vspi.transfer(buff[i]);
  }

  digitalWrite(VSPI_SS,HIGH); //end transmission


}


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

  
  buff = vspi.transfer(0x00);//transfer 0 in order to prompt for data?





  //manipulate to clear bit d6(xor with 1 in bit d6)
  buff ^= (1 << 6);
  //write back to register 0x31
  vspi.transfer(buff); //want to clear bit in DATA_FORMAT register in order to select 4 wire SPI, first 2 bits can be 0 as writing only one byte

  digitalWrite(VSPI_SS,HIGH);//end transmission
}

//after setup, adxl345 should work in 4 wire spi mode


void loop() {

}

// put function definitions here:
