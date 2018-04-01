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
#define MY_RF24_CHANNEL	82

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

#include <SPI.h>
#include <MySensors.h>

#define SN "DimmableRGBLED"
#define SV "1.0"
#define CHILD_ID 1
#define LED_PIN 5

char rgbw[9] = "ffffffff"; // RGBW value.
int currentLevel = 0;  // Current dimmer level.
MyMessage dimmerMsg(CHILD_ID, V_PERCENTAGE);
MyMessage lightMsg(CHILD_ID, V_STATUS);
MyMessage rgbwMsg(CHILD_ID, V_RGBW);
bool initialValueSent = false;

void before() {

}

void setup() {

}

void presentation()
{
  sendSketchInfo(SN, SV);
  present(CHILD_ID, S_RGBW_LIGHT);
}


void loop()
{
  if (!initialValueSent) {
    Serial.println("Sending initial value");
    // Send initial values.
    send(lightMsg.set(currentLevel > 0 ? 1 : 0));
    send(dimmerMsg.set(currentLevel));
    send(rgbwMsg.set(rgbw));
    Serial.println("Requesting initial value from controller");
    request(CHILD_ID, V_STATUS);
    wait(2000, C_SET, V_STATUS);
  }
}

void receive(const MyMessage &message) {
  if (message.type == V_STATUS) {
    if (!initialValueSent) {
      Serial.println("Receiving initial value from controller");
      initialValueSent = true;
    }
  }

  if (message.type == V_RGBW) {
    // Retrieve the RGB value from the incoming message.
    // RGB LED not implemented, just a dummy print.
    String hexstring = message.getString();
    hexstring.toCharArray(rgbw, sizeof(rgbw));
    Serial.print("Changing color to ");
    Serial.println(rgbw);
    send(rgbwMsg.set(rgbw));
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
