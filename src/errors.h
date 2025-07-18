#pragma once

namespace adxl345 {
typedef enum { //define some errors as error_code_t
    /* Common Errors 0 - 99 */
    ERR_CODE_SUCCESS = 0,
    ERR_CODE_UNKNOWN = 1,
    ERR_CODE_INVALID_ARG = 2,
    ERR_CODE_INVALID_STATE = 3,

    /* FreeRTOS errors */
    ERR_CODE_MUTEX_TIMEOUT = 100,
    ERR_CODE_QUEUE_FULL = 101,
    ERR_CODE_INVALID_QUEUE_MSG = 102,

    /* Driver errors */
    ERR_CODE_I2C_TRANSFER_TIMEOUT = 200,

    /* Logging errors */
    ERR_CODE_BUFF_TOO_SMALL = 300,
    ERR_CODE_LOG_MSG_SILENCED = 301,
} error_code_t;


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

//function declarations
error_code_t readReg(byte reg, int numBytes, byte buff[]);
error_code_t writeReg(byte reg, byte buff);
error_code_t readAccel(int *x, int *y, int *z);
void on();
error_code_t setSPI();
error_code_t setClearBit(byte reg, int bitNum,int setClear);
void setRange(int gRange);
void setRate(int rate);
void TOUCH_ISR();
}

