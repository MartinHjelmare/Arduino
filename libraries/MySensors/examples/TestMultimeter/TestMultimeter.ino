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

MySensor gw;
int oldValue = -1;
int value = -1;
// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msgImp(CHILD_ID1, V_IMPEDANCE);
MyMessage msgVolt(CHILD_ID1, V_VOLTAGE);
MyMessage msgPrefix(CHILD_ID2, V_UNIT_PREFIX);
MyMessage msgDist(CHILD_ID2, V_DISTANCE);
MyMessage msgTemp(CHILD_ID3, V_TEMP);

void setup()
{
  gw.begin();
  gw.sendSketchInfo("TestMultimeter", "1.0");
  // Register binary input sensor to gw (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  gw.present(CHILD_ID1, S_MULTIMETER);
  gw.present(CHILD_ID2, S_DISTANCE);
  gw.present(CHILD_ID3, S_TEMP);
  gw.send(msgImp.set("10"));
  gw.send(msgVolt.set("3.0"));
  gw.send(msgPrefix.set("cm"));
  gw.send(msgDist.set("15"));
  gw.send(msgTemp.set("68"));
}


//  Check if digital input has changed and send in new value
void loop()
{
  gw.wait(20000);
  if (oldValue == -1) {
    value = 68;
  }
  if (value != oldValue) {
     // Send in the new value
     gw.send(msgTemp.set(value==68 ? 68 : 72));
     oldValue = value;
     value = value==68 ? 72 : 68;
  }
}
