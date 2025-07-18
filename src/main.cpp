#include <Arduino.h>
#include <SPI.h> //for SPI to ADXL345

/*
Pins on ESP32 Devkit V1
MOSI: 23
MISO: 19
SCK: 18
SS:5
Interrupt pin for tap detection: D4(GPIO 4)
Touch led on D2

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
#define LED 15 //define as pin 15 for now

//definitions for registers

#define ADXL345_DEVID 0x00
#define ADXL345_THRESH_TAP 0x1D
#define ADXL345_OFSX 0x1E
#define ADXL345_OFSY 0x1F
#define ADXL345_OFSZ 0x20
#define ADXL345_DUR 0x21
#define ADXL345_LATENT 0x22
#define ADXL345_WINDOW 0x23
#define ADXL345_THRESH_ACT 0x24
#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27
#define ADXL345_THRESH_FF 0x28
#define ADXL345_TIME_FF 0x29
#define ADXL345_TAP_AXES 0x2A
#define ADXL345_ACT_TAP_STATUS 0x2B
#define ADXL345_BW_RATE 0x2C
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_INT_ENABLE 0x2E
#define ADXL345_INT_MAP 0x2F
#define ADXL345_INT_SOURCE 0x30
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
#define ADXL345_DATAX1 0x33
#define ADXL345_DATAY0 0x34
#define ADXL345_DATAY1 0x35
#define ADXL345_DATAZ0 0x36
#define ADXL345_DATAZ1 0x37
#define ADXL345_FIFO_CTL 0x38
#define ADXL345_FIFO_STATUS 0x39

//pin delcarations

const int VSPI_MISO = 19;
const int VSPI_MOSI = 23;
const int VSPI_SCLK = 18;
const int VSPI_SS = 5;

//global variable(testing)
int blink = 0;


/* FreeRTOS config*/

//mutex
static SemaphoreHandle_t mutex; //for the mutex when doing spi communication

//queue
static QueueHandle_t ledQueue;//queue to store led blink requests


///

typedef struct ledMsg {
  int test;
};


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

  xSemaphoreTake(mutex,0);//take mutex, don't block

  digitalWrite(VSPI_SS,LOW);//start transmission

  //send addr

  vspi.transfer(reg);

  for(int i = 0 ; i < numBytes; i++){//for each byte that we want to read
    buff[i] = vspi.transfer(0x00);//transfer 0 to prompt adxl for new byte of info
  }


  digitalWrite(VSPI_SS,HIGH); //end transmission

  xSemaphoreGive(mutex);



}

/**
@brief write to a register

@param reg the reg you want to write to
@param buff a byte of data you would like to write to reg

*/
void writeReg(byte reg, byte buff){

  xSemaphoreTake(mutex,0); //take mutex(trying not to block)


  digitalWrite(VSPI_SS,LOW);//start transmission

  //send reg to adxl
  vspi.transfer(reg);


  vspi.transfer(buff);

  digitalWrite(VSPI_SS,HIGH); //end transmission

  xSemaphoreGive(mutex);

}


/**
@brief read acceleration data from registers

@param x,y,z register you want to store x, y, z data in respectively

*/
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
  writeReg(ADXL345_POWER_CTL,16); //Auto sleep(BIT D4)
  writeReg(ADXL345_POWER_CTL,8);  //Mesure
}

void setSPI(){

  //TODO: test this with setClearBit function

  //set spi bit section
  byte buff[1];

  //read from register 31
  readReg(0x31,1,buff);


  //manipulate to clear bit d6(xor with 1 in bit d6)(we want 4 wire spi, so should clear bit)
  buff[0] &= ~(1 << 6);
  //write back to register 0x31
  writeReg(0x31,buff[0]);

}




/**
@brief set or clear a bit in the register

@param reg register you want set or clear a bit in
@param bitNum bit to clear(indexing starts at 0)
@param setClear 1 if you want to set the bit, 0 if you want to clear the bit
*/
void setClearBit(byte reg, int bitNum,int setClear){

  byte buff[1];

  //read whats already in the register
  readReg(reg,1,buff);

  if(setClear){//we want to set bit
    Serial.println("Setting bit");

    buff[0] |= (1 << bitNum); //or with bit shifted (this was toggling before)

  }else{//we want to clear bit
    Serial.println("Clearing bit");

    buff[0] &= ~(1 << bitNum);

  }

  writeReg(reg,buff[0]); //write back the modified contents


}

/**
@brief sets range of the adxl345 from 2g to 16g

@param gRange either 2,4,8,16 for the g range

*/
void setRange(int gRange){
  //setting range from +-2g to +- 16g

  switch(gRange){
    case 2:
      Serial.println("Setting g range: 2g");
      setClearBit(ADXL345_DATA_FORMAT,0,0);
      setClearBit(ADXL345_DATA_FORMAT,1,0);
    break;
    

    case 4:
      Serial.println("Setting g range: 4g");
      setClearBit(ADXL345_DATA_FORMAT,0,1);
      setClearBit(ADXL345_DATA_FORMAT,1,0);
    break;


    case 8:
      Serial.println("Setting g range: 8g");

      setClearBit(ADXL345_DATA_FORMAT,0,0);
      setClearBit(ADXL345_DATA_FORMAT,1,1);
    break;


    case 16:
      Serial.println("Setting g range: 16g"); 
      setClearBit(ADXL345_DATA_FORMAT,0,1);
      setClearBit(ADXL345_DATA_FORMAT,1,1);
    break;

    default:
      //set range to 16g if not specified
      Serial.println("Setting g range: 16g");
      setClearBit(ADXL345_DATA_FORMAT,0,1);
      setClearBit(ADXL345_DATA_FORMAT,1,1);
  }


}


void setRate(int rate){
  byte buff;

  switch (rate)
  {
  case 3200:
  //write to 0x2c the corrisponding code
  buff = 0b1111;
  break;
  
  
  
  case 1600:
  buff = 0b1110;
  
  break;

  case 800:
  buff = 0b1101;
  break;

  case 400:
  buff = 0b1100;

  break;


  case 200:
  buff = 0b1011;
  break;
  
  case 100:
  buff = 0b1011;
  break;
    
  case 25:
  buff = 0b1000;
  break;

  case 0:
  buff = 0b0011;
  break;

  default:
    //DEFAULT is case 100 as per datasheet
    break;

  }
  xSemaphoreTake(mutex,0);//take mutex
  writeReg(ADXL345_BW_RATE,buff);
  xSemaphoreGive(mutex);//give it back
}


void TOUCH_ISR(){
  Serial.println("IN ISR");

  //this may be interrupted by other processes(if in freeRTOS)
  blink = 1;

  ledMsg a;

  a.test=1;
  
  xQueueSendFromISR(ledQueue,&a,NULL);//send a led blink event to the queue


  //need to clear interrupts, not sure if this should be done in this isr or later  

}


 void toggleLED(){
    digitalWrite(LED,HIGH);//turn LED on
    delay(1000);//keep on for 1 second
    digitalWrite(LED,LOW);//LED off
}

void setup() {
  /*
  Notes:

  */

  
  Serial.begin(115200);

  vTaskDelay(1000 /portTICK_PERIOD_MS);//wait 1 second

  vspi.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
  vspi.setDataMode(3); //mode 3 as CPOL and CPHA 1.
  pinMode(VSPI_SS,OUTPUT);
  digitalWrite(VSPI_SS,HIGH);//slave selct should be active lo, so go high asap

  //interrupt setup
  pinMode(4,INPUT); //set INT1 AS INTERRUPT
  pinMode(LED,OUTPUT);//led for interrupt


  attachInterrupt(4,TOUCH_ISR,FALLING);//pin will go from high to low as per datasheet

  /*
  Notes
  -Thresh tap precision is 62.5mg/lsb

  */

  //end of interrupt setup

  //set thresh tap register, duration register

  /*FreeRTOS setup*/
  mutex = xSemaphoreCreateMutex(); //create mutex, assign to mutex handle
  ledQueue = xQueueCreate(10,sizeof(ledMsg));//arbitrarily size of 10 
 


  


  //setup of adxl345
  on();
  
  setSPI();

  setRange(16);//set to 16g's

  setRate(100);

  //interrupts setup
  writeReg( ADXL345_INT_MAP, 0); // send all interrupts to ADXL345's INT1 pin



  //threshtap
  writeReg(ADXL345_THRESH_TAP,48);//this is just a test value, need to figure out an actual value of a tap (need accuracy of the data register measurements)

  //tap duration
  writeReg(ADXL345_DUR,0x1F);

  //tap axis enable
  writeReg(ADXL345_TAP_AXES,0x1);//only enable z tap detection

  //want to read tap axis to make sure we set it correctly

  //ENABLE INTERRUPTS
  setClearBit(ADXL345_INT_ENABLE,6,1);//set bit 6 to a 1 to enable interrupt for single tap

  byte buff;
  readReg(ADXL345_TAP_AXES,1,&buff);

  //TODO implement tap detection with interrupts
  //by default the interrupt register is set to all zeros so all interrupts will be sent to int1 pin.

  //set 
  

}

//after setup, adxl345 should work in 4 wire spi mode


void loop() {


  int x,y,z; //create integers to store the x, y, z data

  //readAccel(&x,&y,&z);

  byte tapActivity;

  readReg(ADXL345_INT_SOURCE,1,&tapActivity); //if we read int source and only see anything it should mean there was a single tap as all other interrupts are disabled
  //this should be read at the beginning to reset any tap activity, then at the end of isr to reset

  ledMsg buff;//create buffer to read into for receiving
  if(xQueueReceive(ledQueue,&buff,0) == pdTRUE){//if we get something in the queue
    toggleLED();
  }
  

  if(blink){
    digitalWrite(LED,HIGH);//turn LED on
    delay(1000);//keep on for 1 second
    digitalWrite(LED,LOW);//LED off
    blink = 0;//reset blink
  }

  delay(100);//give more time to read in logic analyzer

  

}

/*
Notes
-2g range means max value on any axis is 2g = 512
-reading int source, clears the interrupts.


*/