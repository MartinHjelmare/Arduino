/*
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * http://www.mysensors.org/build/dimmer
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

#include <SPI.h>
#include <MySensors.h>

#define SN "DimmableRGBLED"
#define SV "1.0"
#define CHILD_ID 1
#define LED_PIN 5

char rgb[7] = "ffffff"; // RGB value.
int currentLevel = 0;  // Current dimmer level.
MyMessage dimmerMsg(CHILD_ID, V_PERCENTAGE);
MyMessage lightMsg(CHILD_ID, V_STATUS);
MyMessage rgbMsg(CHILD_ID, V_RGB);


void before() {

}

void setup() {

}

void presentation()
{
  sendSketchInfo(SN, SV);
  present(CHILD_ID, S_RGB_LIGHT);
  // Send initial values.
  send(lightMsg.set(currentLevel > 0 ? 1 : 0));
  send(dimmerMsg.set(currentLevel));
  send(rgbMsg.set(rgb));
}


void loop()
{

}

void receive(const MyMessage &message) {
  if (message.type == V_RGB) {
    // Retrieve the RGB value from the incoming message.
    // RGB LED not implemented, just a dummy print.
    String hexstring = message.getString();
    hexstring.toCharArray(rgb, sizeof(rgb));
    Serial.print("Changing color to ");
    Serial.println(rgb);
    send(rgbMsg.set(rgb));
  }

  if (message.type == V_STATUS || message.type == V_PERCENTAGE) {
    // Retrieve the light status or dimmer level from the incoming message.
    int requestedLevel = atoi(message.data);

    // Adjust incoming level if this is a V_LIGHT update [0 == off, 1 == on].
    requestedLevel *= (message.type == V_STATUS ? 100 : 1);

    // Clip incoming level to valid range of 0 to 100
    requestedLevel = requestedLevel > 100 ? 100 : requestedLevel;
    requestedLevel = requestedLevel < 0   ? 0   : requestedLevel;

    // Change level value of LED pin.
    analogWrite(LED_PIN, (int)(requestedLevel / 100. * 255));
    currentLevel = requestedLevel;

    // Update the gateway with the current V_STATUS and V_PERCENTAGE.
    send(lightMsg.set(currentLevel > 0 ? 1 : 0));
    send(dimmerMsg.set(currentLevel));
    }
}
