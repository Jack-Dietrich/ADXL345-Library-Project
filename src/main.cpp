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

//initial setup



//defining hspi and vspi

#define VSPI 3
#define DATA_START 0x32
#define ADXL345_POWER_CTL 0x2D


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

void writeReg(byte reg, byte buff){


  //SET WRITE
  //reg &= ~(1<<7); //set last bit to 0 so we write(not needed)



  digitalWrite(VSPI_SS,LOW);//start transmission

  //send reg to adxl
  vspi.transfer(reg);


  vspi.transfer(buff);

  digitalWrite(VSPI_SS,HIGH); //end transmission


}

void readAccel(int *x, int *y, int *z){
  byte buff[6]; //6 byte register to hold both x, y, z data (each coordinate has 2 registers)

  readReg(DATA_START,6,buff); //read 6 bytes into our buffer.
	
  /*
  Here we are storing into the respective variables what we have read from the sensor. We are casting to ensure c++ does not choose incorrect data types. Buff[0] will be the first byte of our data, we then or it with the
  2nd byte stored in buff[1]. This needs to be shifted left 8 bits to align correctly to form the 16 bit value.
  */
  *x = (int16_t)((((int)buff[1]) << 8) | buff[0]);
	*y = (int16_t)((((int)buff[3]) << 8) | buff[2]);
	*z = (int16_t)((((int)buff[5]) << 8) | buff[4]);
}

void on(){

  writeReg(ADXL345_POWER_CTL,0); //WAKE
  writeReg(ADXL345_POWER_CTL,16); //Auto sleep
  writeReg(ADXL345_POWER_CTL,8);  //Mesure
}

void setup() {
  /*
  Notes:

  */

  
  Serial.begin(9600);
  vspi.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
  vspi.setDataMode(3); //mode 3 as CPOL and CPHA 1.
  pinMode(VSPI_SS,OUTPUT);
  digitalWrite(VSPI_SS,HIGH);//slave selct should be active lo, so go high asap

  //the delay is not needed
  
  on();
  

  //set spi bit section
  byte buff[1];

  //read from register 31
  readReg(0x31,1,buff);


  //manipulate to clear bit d6(xor with 1 in bit d6)(we want 4 wire spi, so should clear bit)
  buff[0] &= ~(1 << 6);
  //write back to register 0x31
  writeReg(0x31,buff[0]);

}

//after setup, adxl345 should work in 4 wire spi mode


void loop() {


  int x,y,z; //create integers to store the x, y, z data

  readAccel(&x,&y,&z);


  
  
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.println(z);

}

// put function definitions here:
