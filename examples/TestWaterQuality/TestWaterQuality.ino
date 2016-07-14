/**
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * http://www.mysensors.org/build/light
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
#define MY_RF24_CHANNEL	78

#include <SPI.h>
#include <MySensors.h>

#define CHILD_ID 0

unsigned long SLEEP_TIME = 20000; // Sleep time between reads (in milliseconds)

MyMessage msg(CHILD_ID, V_EC);
int ec = 10;
int lastEc = -1;


void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Water Quality", "1.0");

  // Register all sensors to gateway (they will be created as child devices)
  present(CHILD_ID, S_WATER_QUALITY);
}

void loop()
{
  if (ec >= 50) {
    ec = 10;
  }
  Serial.println(ec);
  if (ec != lastEc) {
      send(msg.set(ec));
      lastEc = ec;
  }
  ec += 5;
  sleep(SLEEP_TIME);
}
