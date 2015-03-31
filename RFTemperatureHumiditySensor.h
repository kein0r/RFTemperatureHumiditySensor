/** @ingroup RFTemperatureHumiditySensor
 * @{
 */
#ifndef RFTEMPERATUREHUMIDITYSENSOR_H_
#define RFTEMPERATUREHUMIDITYSENSOR_H_
   
/*******************| Inclusions |*************************************/
#include <PlatformTypes.h>
#include <IEEE_802.15.4.h>
   
/*******************| Macros |*****************************************/
   
/** 
 * Channel to be used for IEEE 802.15.4 radio. The channels are numbered 11 
 * through 26.
*/
#define RFTemperatureHumiditySensor_Channel                     0x19

/**
 * Default Short address of this node (big-endian). For now we choose 0xaf00 + sensorID (see below)
*/
#define RFTemperatureHumiditySensor_ShortAddress                (IEEE802154_ShortAddress_t)0x42af

/**
 * Default PanID of this note (big-endian).
*/
#define RFTemperatureHumiditySensor_PanID                       (IEEE802154_PANIdentifier_t)0x3332

/**
 * Default source and destination address mode
*/
#define RFTemperatureHumiditySensor_DestinationAdressingMode    IEEE802154_FCF_ADDRESS_MODE_16BIT
#define RFTemperatureHumiditySensor_SourceAdressingMode         IEEE802154_FCF_ADDRESS_MODE_16BIT

/**
 * ID of this sensor. Must be unique within the network
*/
#define RFTemperatureHumiditySensor_SensorID                    0x42

/**
 * Time in multiple of 1/32kHz for the sensor to sleep between two read-outs
*/
#define RFTemperatureHumiditySensor_SleepTime                   (uint32_t)0xfffff

/*******************| Type definitions |*******************************/

/**
  * \brief No buffer is used to temporary store the received and sent frames.
  * For each frame that is sent or received a buffer must be allocated before
  * sending or receiving.
  * For this a message element (struct, array etc.) must be declared and allocated
  * to which the payload pointer of the IEE802154_header_t will point.
  */
typedef struct {
  uint8_t id;                     /*!< message id must always come first */
  sint16_t dht22Temperatur;       /*!< temperatur of dht22 */
  uint16_t dht22RelativeHumidity; /*!< relative humidity of dht22 */
} sensorInformation_t;

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

#endif
/** @}*/