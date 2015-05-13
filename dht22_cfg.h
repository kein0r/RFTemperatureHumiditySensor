/** @ingroup DHT22
 * @{
 */
#ifndef DHT22_CFG_H_
#define DHT22_CFG_H_

/*******************| Inclusions |*************************************/
#include <board.h>
#include <CC253x.h>
#include <ioCC2530.h>
   
/*******************| Macros |*****************************************/ 

/**
 * Debug macro for sensor timing analysis. To global variables will be added
 * which contain the counter values when MCU communicates with the sensor.
 * These values can be used to adapt the counter values in this file to your
 * set-up.
*/
#define DHT22_DEBUG

/*
 * use macros from HAL layer for internal comparison.
*/
#define DHT22_DATALINE_LOW              (uint8_t)Px_LOW
#define DHT22_DATALINE_HIGH             (uint8_t)Px_HIGH

/*
 * Platform specific function to set data line to output
*/ 
#define DHT22_SetDataLineOutput()       P0DIR_4 = Px_PINOUTPUT

/*
 * Platform specific function to set data line to input
*/ 
#define DHT22_SetDataLineInput()        P0DIR_7 = Px_PININPUT

/*
 * Platform specific function to write logic low to sensor
*/ 
#define DHT22_WriteDataBitLow()         P0_7 = DHT22_DATALINE_LOW

/*
 * Platform specific function to write logic high to sensor
*/ 
#define DHT22_WriteDataBitHigh()        P0_7 = DHT22_DATALINE_HIGH

/*
 * Platform specific function to read bit from data line
*/ 
#define DHT22_ReadDataBit()             P0_7

/**
 * Number of uS to wait using delay_us function for the start signal of the MCU. 
 * According to datasheet at least 18ms
 */
#define DHT22_MCUSendStartSignalTime            (uint16_t)10000

/**
 * Number of cycle for sensor response after start signal in uS. According to 
 * datasheet 20-40uS.
 * Enable DHT22_DEBUG and look at the value in DHT22_sensorWaitCounter[0] and
 * DHT22_sensorWaitCounter[1] to find a good values to set here. The values
 * in the debugging variables should be after readout approx. half of the 
 * values you set here
*/
#define DHT22_MCUWaitForSensorResponse          (uint8_t)200

/**
 * Number of cycle to wait for sensor to detect a zero. According to datasheet
 * time is 26uS to 28uS.
 * Enable DHT22_DEBUG and look at the values in DHT22_sensorBitWaitCounter to
 * find a good values to set here
*/
#define DHT22_MCUWaitForSensorSendZero          (uint8_t)80

/** 
 * Invalid value for temperatur in case CRC does not match (sint16)
*/
#define DHT22_TemperaturInvalidValue            (sint16_t)0xffff

/** 
 * Invalid value for relative humidity in case CRC does not match (uint16)
*/
#define DHT22_RelativeHumidityInvalidValue      (uint16_t)0xffff
   
/*******************| Type definitions |*******************************/

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

#endif
/** @}*/
