/**
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 */
// Enable MY_DEBUG flag for MY_DEBUG prints. This will add a lot to the size of the final sketch but good
// to see what is actually is happening when developing
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
#define MY_RF24_CHANNEL	78

#include <SPI.h>
#include <MySensors.h>
//#include <TimeLib.h>

#define CHILD_ID_HUM 0

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)
boolean gotTime = false;

void presentation()
{
  // Send the Sketch Version Information to the Gateway
  sendSketchInfo("TestRecTime", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID_HUM, S_HUM);

  #ifdef MY_DEBUG
    Serial.println(F("Presentation complete"));
  #endif
}

void setup()
{

  #ifdef MY_DEBUG
    Serial.println(F("Requesting time"));
  #endif
  //Sync time with the server
  requestTime();
  wait(2000, C_INTERNAL, I_TIME);

} // END Setup

void loop() {
  // if we don't have time , request it from controller
  if (gotTime == false)
  {
    requestTime();
    #ifdef MY_DEBUG
      Serial.println(F("Requesting time"));
    #endif
  }

  // all done, sleep a bit
  #ifdef MY_DEBUG
    Serial.println(F("=========================== sweet dreams  ============================="));
  #endif
  smartSleep(SLEEP_TIME); //sleep a bit
} // End Loop


void receive(const MyMessage &message)
{

} // End receive

void receiveTime(unsigned long newTime)
{
  Serial.println(F("Time received from controller..."));
  Serial.println(newTime);
  gotTime = true;
} // End receiveTime
