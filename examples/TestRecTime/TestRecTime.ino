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

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
#define CHILD_ID_BARO 2
#define CHILD_ID_FOREC 3
#define CHILD_ID_VBATT 4
#define CHILD_ID_VSOLAR 5
#define CHILD_ID_MOISTURE 6
#define CHILD_ID_RAIN_LOG 7  // Keeps track of accumulated rainfall
#define CHILD_ID_TRIPPED_INDICATOR 8  // Indicates Tripped when rain detected

MyMessage msgVBatt(CHILD_ID_VBATT, V_VOLTAGE);
MyMessage msgHumidity(CHILD_ID_HUM, V_HUM);
MyMessage msgTemperature(CHILD_ID_TEMP, V_TEMP);
MyMessage msgTripped(CHILD_ID_TRIPPED_INDICATOR, V_TRIPPED);
MyMessage msgRainRate(CHILD_ID_RAIN_LOG, V_RAINRATE);
MyMessage msgRain(CHILD_ID_RAIN_LOG, V_RAIN);
MyMessage msgPressure(CHILD_ID_BARO, V_PRESSURE);
MyMessage msgPressurePrefix(CHILD_ID_BARO, V_UNIT_PREFIX);  // Custom unit message.
MyMessage msgForecast(CHILD_ID_FOREC, V_FORECAST);
MyMessage msgVSolar(CHILD_ID_VSOLAR, V_VOLTAGE);
MyMessage msgMoisture(CHILD_ID_MOISTURE, V_LEVEL);
MyMessage msgMoisturePrefix(CHILD_ID_MOISTURE, V_UNIT_PREFIX);  // Custom unit message.

boolean metric = true;
unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)
bool gotTime = false;
float oldVBatt = 0;
float lastTemperature;
float lastHumidity;
int lastTripped = -1;
unsigned int lastRainRate = 0;
int lastRain = 0;
float lastPressure = -1;
int lastForecast = -1;
float oldVSolar = 0;
int oldMoisture = 0;

void presentation()
{
  // Send the Sketch Version Information to the Gateway
  sendSketchInfo("TestRecTime", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID_VBATT, S_MULTIMETER);
  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);
  present(CHILD_ID_TRIPPED_INDICATOR, S_MOTION);
  present(CHILD_ID_RAIN_LOG, S_RAIN);
  present(CHILD_ID_BARO, S_BARO);
  present(CHILD_ID_FOREC, S_BARO);
  present(CHILD_ID_VSOLAR, S_MULTIMETER);
  present(CHILD_ID_MOISTURE, S_MOISTURE);

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

  // send pressure prefix
  send(msgPressurePrefix.set("atm"));
  // send moisture prefix
  send(msgMoisturePrefix.set("vol/vol"));

} // END Setup

void loop() {
  // if we don't have time , request it from controller
  if (gotTime == false)
  {
    #ifdef MY_DEBUG
      Serial.println(F("Requesting time"));
    #endif
    requestTime();
    wait(2000, C_INTERNAL, I_TIME);
  }

  // send battery voltage
  float batteryV  = 3.00;
  if (batteryV != oldVBatt) {
    send(msgVBatt.set(batteryV, 2));
    oldVBatt = batteryV;
  }
  // send temperature reading
  float temperature = 20;
  if (temperature != lastTemperature)
  {
    send(msgTemperature.set(temperature, 1));
    lastTemperature = temperature;
  }
  // send humidity reading
  float humidity = 30;
  if (humidity != lastHumidity)
  {
    send(msgHumidity.set(humidity, 0));
    lastHumidity = humidity;
  }
  // send tripped
  int tripped = 1;
  if (tripped != lastTripped)
  {
    send(msgTripped.set(tripped));
    lastTripped = tripped;
  }
  // send rainRate
  unsigned int rainRate = 2;
  if (rainRate != lastRainRate)
  {
    send(msgRainRate.set(rainRate));
    lastRainRate = rainRate;
  }
  // send rain
  int rain = 1;
  if (rain != lastRain)
  {
    send(msgRain.set(rain));
    lastRain = rain;
  }
  // send pressure
  float pressure = 1.05;
  if (pressure != lastPressure) {
    send(msgPressure.set(pressure, 2));
    lastPressure = pressure;
  }
  // send forecast
  int forecast = 1;
  if (forecast != lastForecast)
  {
    send(msgForecast.set(forecast));
    lastForecast = forecast;
  }
  // send VSolar
  float vSolar = 4.20;
  if (vSolar != oldVSolar) {
    send(msgVSolar.set(vSolar, 2));
    oldVSolar = vSolar;
  }
  // send moisture
  int moisture = 1;
  if (moisture != oldMoisture)
  {
    send(msgMoisture.set(moisture));
    oldMoisture = moisture;
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
  gotTime = false;
} // End receiveTime
