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
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * Example sketch showing how to control physical relays.
 * This example will remember relay state after power failure.
 * http://www.mysensors.org/build/relay
 */

#include <MySigningNone.h>
#include <MySigningAtsha204Soft.h>
#include <MySigningAtsha204.h>
#include <MyTransportNRF24.h>
#include <MyTransportRFM69.h>
#include <MyHwATMega328.h>
#include <MySensor.h>
#include <SPI.h>

#define led 3

// NRFRF24L01 radio driver (set low transmit power by default)
MyTransportNRF24 transport(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL);
//MyTransportRFM69 radio;
// Message signing driver (none default)
//MySigningNone signer;
MySigningAtsha204Soft signer(true);
// Select AtMega328 hardware profile
MyHwATMega328 hw;
// Construct MySensors library
//MySensor gw(radio, hw);
MySensor gw(transport, hw, signer);
MyMessage msgLight(1, V_STATUS);

void setup()
{
  // Initialize library and add callback for incoming messages
  gw.begin(incomingMessage, AUTO, false);
  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Relay", "1.0");

  // Register led as sensor to gw, created as a child device.
  gw.present(1, S_BINARY);
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  delay(1000);
  digitalWrite(led, HIGH); // turn the LED on by making the voltage HIGH
  delay(1000); // wait for a second
  digitalWrite(led, LOW); // turn the LED off by making the voltage LOW
  gw.send(msgLight.set("0"));
}


void loop()
{
  // Alway process incoming messages whenever possible
  gw.process();
}

void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_STATUS) {
     // Change relay state
     digitalWrite(message.sensor-1+led, message.getBool()?1:0);

     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   }
}
