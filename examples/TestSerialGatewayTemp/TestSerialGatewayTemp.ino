#define MY_DEBUG

// Define node ID
#define MY_NODE_ID 0

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
#define MY_RF24_CHANNEL	78

#define MY_SIGNING_SOFT
#define MY_SIGNING_SOFT_RANDOMSEED_PIN 7
#define MY_SIGNING_REQUEST_SIGNATURES

// Enable serial gateway
#define MY_GATEWAY_SERIAL

// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif

// Flash leds on rx/tx/err
// Uncomment to override default HW configurations
#define MY_DEFAULT_ERR_LED_PIN 4  // Error led pin
#define MY_DEFAULT_RX_LED_PIN  6  // Receive led pin
#define MY_DEFAULT_TX_LED_PIN  5  // the PCB, on board LED

#include <MySensors.h>
#include <DallasTemperature.h>
#include <OneWire.h>

// Send temperature only if changed? 1 = Yes 0 = No
#define COMPARE_TEMP 1

// Pin where dallas sensor is connected
#define ONE_WIRE_BUS 2

// Sleep time between reads (in milliseconds)
unsigned long SLEEP_TIME = 20000;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//OneWire oneWire(ONE_WIRE_BUS);

// Pass the oneWire reference to Dallas Temperature.
//DallasTemperature sensors(&oneWire);

// arrays to hold device address
//DeviceAddress insideThermometer;

#define CHILD_ID_TEMP 0

float lastTemperature;

// Initialize temperature message
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
float temp = 20.0;


void setup()
{
  //sensors.begin();
  //if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");

  // set the resolution to 12 bit (Each Dallas/Maxim device is capable of several different resolutions)
  //sensors.setResolution(insideThermometer, 12);

  //Serial.print("Device 0 Resolution: ");
  //Serial.print(sensors.getResolution(insideThermometer), DEC);
  //Serial.println();
}

void presentation()
{
   // Present locally attached sensors
   // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Gateway Temp", "1.2");

  // Present all sensors to controller
  present(CHILD_ID_TEMP, S_TEMP);
}

void loop()
{
	// Send locally attached sensor data here
 	// Fetch temperatures from Dallas sensors
  //sensors.requestTemperatures();

  //float temp = sensors.getTempC(insideThermometer);
	temp = temp + 0.1;

  // Only send data if temperature has changed and no error
  #if COMPARE_TEMP == 1
  if (lastTemperature != temp && temp != -127.00 && temp != 85.00) {
  #else
  if (temp != -127.00 && temp != 85.00) {
  #endif

    // Send in the new temperature
    //Serial.print("Sending temp: "); Serial.println(temp);
    send(msgTemp.set(temp,1));
    // Save new temperatures for next compare
    lastTemperature=temp;
    }
  wait(SLEEP_TIME);
}
