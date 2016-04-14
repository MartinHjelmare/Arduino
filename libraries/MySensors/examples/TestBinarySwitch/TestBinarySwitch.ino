/**
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * http://www.mysensors.org/build/binary
 */


#include <MySensor.h>
#include <SPI.h>
#include <Bounce2.h>

#define SN "BinarySensor"
#define SV "1.0"
#define CHILD_ID 1
#define BUTTON_PIN 3  // Arduino Digital I/O pin for button/reed switch.

MySensor gw;
Bounce debouncer = Bounce();
MyMessage msg(CHILD_ID, V_TRIPPED);

void setup()
{
  gw.begin();
  gw.sendSketchInfo(SN, SV);
  // Setup the button.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // After setting up the button, setup debouncer.
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5);
  gw.present(CHILD_ID, S_DOOR);
  gw.send(msg.set(0));
}

void loop()
{
  if (debouncer.update()) {
    // Get the update value.
    int value = debouncer.read();
    // Send in the new value.
    gw.send(msg.set(value == LOW ? 1 : 0));
  }
}
