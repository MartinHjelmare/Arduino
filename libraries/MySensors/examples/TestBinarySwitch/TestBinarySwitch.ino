/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * Simple binary switch example
 * Connect button or door/window reed switch between
 * digitial I/O pin 3 (BUTTON_PIN below) and GND.
 * http://www.mysensors.org/build/binary
 */


#include <MySensor.h>
#include <SPI.h>

#define CHILD_ID1 1
#define CHILD_ID2 2
#define CHILD_ID3 3
#define CHILD_ID4 4

MySensor gw;
int oldValue = -1;
int value = -1;
// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg1(CHILD_ID1,V_TRIPPED);
MyMessage msg2(CHILD_ID2,V_TRIPPED);
MyMessage msg3(CHILD_ID3,V_TRIPPED);
MyMessage msg4(CHILD_ID4,V_TRIPPED);

void setup()
{
  gw.begin();
  gw.sendSketchInfo("TestBinarySwitch", "1.0");
  // Register binary input sensor to gw (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  gw.present(CHILD_ID1, S_DOOR);
  gw.present(CHILD_ID2, S_DOOR);
  gw.present(CHILD_ID3, S_DOOR);
  gw.present(CHILD_ID4, S_DOOR);
  gw.send(msg1.set("0"));
  gw.send(msg2.set("0"));
  gw.send(msg3.set("0"));
  gw.send(msg4.set("0"));
}


//  Check if digital input has changed and send in new value
void loop()
{
  gw.wait(5000);
  if (oldValue == -1) {
    value = 1;
  }
  if (value != oldValue) {
     // Send in the new value
     gw.send(msg1.set(value==1 ? 1 : 0));
     oldValue = value;
     value = value==1 ? 0 : 1;
  }
}
