#include <ioCC2530.h>
#include "PlatformTypes.h"
#include <board.h>
#include <dht22.h>

int main( void )
{
  volatile DHT22State_t DHT22State;
  P0DIR_0 = HAL_PINOUTPUT;
  P0DIR_2 = HAL_PINOUTPUT;
  P0DIR_4 = HAL_PINOUTPUT;
  /* switch on sensor (P0.0 connected to GND, P0.2 connected to VCC) */
  P0 = HAL_LOW;
  P0_0 = HAL_LOW;
  P0_2 = HAL_HIGH;
  ledInit();
  DHT22_init();
  while(1)
  {
    ledOn();
    DHT22State = DHT22_readValues();
    DHT22State++;
    DHT22_init();
    ledOff();
    delay_us(10000);
  }
  return 0;
}
