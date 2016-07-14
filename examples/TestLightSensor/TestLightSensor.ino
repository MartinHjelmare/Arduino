/**
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * http://www.mysensors.org/build/light
 */

#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_RF24_CHANNEL	78

//#include <BH1750.h>
//#include <Wire.h>
//#include <SPI.h>
#include <MySensors.h>

#define SN "LightLuxSensor"
#define SV "1.0"
#define CHILD_ID 1
unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

//BH1750 lightSensor;
MyMessage msg(CHILD_ID, V_LEVEL);
//MyMessage msgPrefix(CHILD_ID, V_UNIT_PREFIX);  // Custom unit message.
uint16_t lastlux = 0;
bool initialValueSent = false;

void setup()
{
  sendSketchInfo(SN, SV);
  present(CHILD_ID, S_LIGHT_LEVEL);
  //lightSensor.begin();
}

void loop()
{
  if (!initialValueSent) {
    Serial.println("Sending initial value");
    //send(msgPrefix.set("lux"));  // Set custom unit.
    send(msg.set(lastlux));
    Serial.println("Requesting initial value from controller");
    request(CHILD_ID, V_LEVEL);
    wait(2000, C_SET, V_LEVEL);
  }
  uint16_t lux = 10;
  //uint16_t lux = lightSensor.readLightLevel();  // Get Lux value
  if (lux != lastlux) {
      send(msg.set(lux));
      lastlux = lux;
  }

  sleep(SLEEP_TIME);
}

void receive(const MyMessage &message) {
  if (message.type == V_LEVEL) {
    if (!initialValueSent) {
      Serial.println("Receiving initial value from controller");
      initialValueSent = true;
    }
  }
}
