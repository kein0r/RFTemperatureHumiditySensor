/** @ingroup RFTemperatureHumiditySensor
 * @{
 */
#include <ioCC2530.h>
#include <PlatformTypes.h>
#include <board.h>
#include <WatchdogTimer.h>
#include <IEEE_802.15.4.h>
#include <CC253x.h>
#include <string.h>
#include <DHT22.h>
#include <ADC.h>
#include <ppd42ns.h>
#include "RFTemperatureHumiditySensor.h"

IEEE802154_Config_t IEEE802154_Config;

IEEE802154_DataFrameHeader_t  IEEE802154_TxDataFrame;
IEEE802154_DataFrameHeader_t  IEEE802154_RxDataFrame;
IEEE802154_Payload radioRxPayload[100];
sensorInformation_t sensorInformation;

/**
 * \mainpage RF TemperaturHumidity and Airquality
 * @brief IEEE 802.15.4 sensor for Temperature, Humidity and Airquality
 * 
 * The following sensors are connected to the board. All sensors are connected to P0.
 * - PPD42NS P1 (1um) is connected to P0.2, P2 (2.5um) is connected P0.3
 * - Second PPD42NS P1 (1um) is connected to P0.4, P2 (2.5um) is connected P0.5
 * - DHT22 is connected to P0.7
 * - Internal temperature sensor is connected to ADC channel 14
 * Sensor will sent three messages. First and second message will only contain temperature
 * and humidity, third message will, after 30 seconds, contain particle value also.
 */

void measureAllValues();

void main( void )
{
  sleepTimer_t sleepTime;
  Board_init(); /* calls CC253x_Init */
  /* P0.2 to P0.4 are used for peripheral function timer 1 input capture (channel 0 to 4) for PPD42NS sensor */
  
  ADC_init(APCFG_ADCINPUT0, ADCCON1_STSEL_ADCON1STEL);
  ADC_connectTemperaturSensorToADC();
  
  PPD42NS_init();
  
  ledInit();
  DHT22_init();
  
  /* Prepare rx and tx  buffer for IEEE 802.15.4 (Must be done before interrupt enable) */
  IEEE802154_RxDataFrame.payload = radioRxPayload;
  IEEE802154_TxDataFrame.payload = (IEEE802154_Payload*) &sensorInformation;
  
  /* General radio configuration and set-up of IEEE 802.15.4 module */
  IEEE802154_Config.Channel = RFTemperatureHumiditySensor_Channel;
  IEEE802154_Config.PanID = RFTemperatureHumiditySensor_PanID;
  IEEE802154_Config.shortAddress = RFTemperatureHumiditySensor_ShortAddress;
  IEEE802154_radioInit(&(IEEE802154_Config));
  
  /* Tx source address is preloaded with chip's own 64bit address. For now we use 16bit source address. */
  IEEE802154_TxDataFrame.fcf.sourceAddressMode = IEEE802154_FCF_ADDRESS_MODE_16BIT;
  /* Prepare header for IEEE 802.15.4 Tx message. */
  IEEE802154_TxDataFrame.fcf.frameType = IEEE802154_FCF_FRAME_TYPE_DATA;  /* 3: 0x01 */
  IEEE802154_TxDataFrame.fcf.securityEnabled = IEEE802154_FCF_SECURITY_DISABLED; /* 1: 0x0 */
  IEEE802154_TxDataFrame.fcf.framePending = 0x0; /* 1:0x0 */
  IEEE802154_TxDataFrame.fcf.ackRequired = IEEE802154_FCF_ACKNOWLEDGE_REQUIRED; /* 1: 0x1 */
#ifdef IEEE802154_ENABLE_PANID_COMPRESSION
  IEEE802154_TxDataFrame.fcf.panIdCompression = IEEE802154_FCF_PANIDCOMPRESSION_ENABLED; /* 1: 0x1 */
#else
  IEEE802154_TxDataFrame.fcf.panIdCompression = IEEE802154_FCF_PANIDCOMPRESSION_DISABLED; /* 1: 0x0 */
#endif
  IEEE802154_TxDataFrame.fcf.destinationAddressMode = RFTemperatureHumiditySensor_DestinationAdressingMode;
  IEEE802154_TxDataFrame.fcf.frameVersion = 0x00;
  IEEE802154_TxDataFrame.fcf.sourceAddressMode = RFTemperatureHumiditySensor_SourceAdressingMode;
  /* preset variable to some meaningfull values */
  IEEE802154_TxDataFrame.sequenceNumber = 0x00;
  IEEE802154_TxDataFrame.destinationPANID = RFTemperatureHumiditySensor_PanID;
  IEEE802154_TxDataFrame.destinationAddress.shortAddress = 0xffff;   /* broadcast */
  IEEE802154_TxDataFrame.sourceAddress.shortAddress = RFTemperatureHumiditySensor_ShortAddress;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[0] = IEEE_EXTENDED_ADDRESS0;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[1] = IEEE_EXTENDED_ADDRESS1;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[2] = IEEE_EXTENDED_ADDRESS2;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[3] = IEEE_EXTENDED_ADDRESS3;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[4] = IEEE_EXTENDED_ADDRESS4;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[5] = IEEE_EXTENDED_ADDRESS5;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[6] = IEEE_EXTENDED_ADDRESS6;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[7] = IEEE_EXTENDED_ADDRESS7;
  enableAllInterrupt();
  
  sensorInformation.id = RFTemperatureHumiditySensor_SensorID;

  sleepTime.value = RFTemperatureHumiditySensor_SleepTime;
  
  while(1)
  {
#ifdef PPD42NS_SENSOR0CONNECTED
    /* reset values for PPD42NS because first two messages will be sent without */
    sensorInformation.ppd42ns1umParticleConcentrationSensor0 = 0.0;
    sensorInformation.ppd42ns25umParticleConcentrationSensor0 = 0.0;
#endif
#ifdef PPD42NS_SENSOR1CONNECTED
    /* reset values for PPD42NS because first two messages will be sent without */
    sensorInformation.ppd42ns1umParticleConcentrationSensor1 = 0.0;
    sensorInformation.ppd42ns25umParticleConcentrationSensor1 = 0.0;
#endif

    ledOn();
    measureAllValues();
    ledOff();
    IEEE802154_radioSentDataFrame(&IEEE802154_TxDataFrame, sizeof(sensorInformation_t));
    
    ledOn();
    measureAllValues();
    ledOff();
    IEEE802154_radioSentDataFrame(&IEEE802154_TxDataFrame, sizeof(sensorInformation_t));
    
    ledOn();
    measureAllValues();
   /* Because PPD42NS needs aprox 30s for a complete sensor read-out we wait after we read
    * the other values */
#ifdef PPD42NS_SENSOR0CONNECTED
    PPD42NS_waitForNextSenorValue();
    PPD42NS_readSensor0P1Value(&sensorInformation.ppd42ns1umParticleConcentrationSensor0);
    PPD42NS_readSensor0P2Value(&sensorInformation.ppd42ns25umParticleConcentrationSensor0);
#endif
#ifdef PPD42NS_SENSOR1CONNECTED
    PPD42NS_waitForNextSenorValue();
    PPD42NS_readSensor1P1Value(&sensorInformation.ppd42ns1umParticleConcentrationSensor1);
    PPD42NS_readSensor1P2Value(&sensorInformation.ppd42ns25umParticleConcentrationSensor1);
#endif
    ledOff();
    IEEE802154_radioSentDataFrame(&IEEE802154_TxDataFrame, sizeof(sensorInformation_t));
    
    CC253x_IncrementSleepTimer(sleepTime);
    CC253x_ActivatePowerMode(SLEEPCMD_MODE_PM2);
  }
}

void measureAllValues()
{
  volatile DHT22State_t DHT22State;
#ifdef USE_GP2Y1050SENSOR
  ADC_ADCValue_t particleSensorValue;
#endif
  
  /* read internal and external temperatur sensor */
  DHT22State = DHT22_readValues();
  
  /* prepare values */
  sensorInformation.dht22Temperature = DHT22_SensorValue.values.Temperatur;
  sensorInformation.dht22RelativeHumidity = DHT22_SensorValue.values.RelativeHumidity;
  
  ADC_startSingleConversion( ADCCON3_EREF_INTERNAL | ADCCON3_SDIV_12BITS_ENOB | ADCCON3_SCH_TEMPERATURESENSOR );
  /* wait for ADC conversion to finish */
  while (ADC_isConversionComplete()) ;
  sensorInformation.internalTemperatureSensor = ADC_readSingleConversionValue();
  
#ifdef USE_GP2Y1050SENSOR
  sensorInformation.averageSharpParticleConcentration = 0;
  sensorInformation.minSharpParticleConcentration = 0xffffffff;
  sensorInformation.maxSharpParticleConcentration = 0x0;
  for (int i=0; i<NUMBEROFSHARPSENSORREADOUTS; i++)
  {
    ADC_startSingleConversion( ADCCON3_EREF_INTERNAL | ADCCON3_SDIV_12BITS_ENOB | ADCCON3_SCH_AIN0 );
    /* wait for ADC conversion to finish */
    while (ADC_isConversionComplete()) ;
    particleSensorValue = ADC_readSingleConversionValue();
    if (particleSensorValue > sensorInformation.maxSharpParticleConcentration)
    {
      sensorInformation.maxSharpParticleConcentration = particleSensorValue;
    }
    if (particleSensorValue < sensorInformation.minSharpParticleConcentration)
    {
      sensorInformation.minSharpParticleConcentration = particleSensorValue;
    }
    sensorInformation.averageSharpParticleConcentration += particleSensorValue;
  }
#endif
}

/**
 * Callback whenever Beacon frame was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_BeaconFrameReceived(uint8_t payloadLength, sint8_t rssi)
{
  
}

/**
 * Callback whenever Data frame was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_DataFrameReceived(uint8_t payloadLength, sint8_t rssi)
{

}

/**
 * Callback whenever Ack frame was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_AckFrameReceived(uint8_t payloadLength, sint8_t rssi)
{

}

/**
 * Callback whenever MAC Command frame was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_MACCommandFrameReceived(uint8_t payloadLength, sint8_t rssi)
{
  
}

/**
 * Callback whenever frame with incorrect CRC was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_CRCError(uint8_t payloadLength, sint8_t rssi)
{
  
}

/** @}*/