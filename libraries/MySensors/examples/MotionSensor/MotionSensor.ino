/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of
 * the network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list:
 * https://github.com/mysensors/Arduino/graphs/contributors
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
 *
 * MODIFIED BY Martin Hjelmare
 */

#include <MySensor.h>
#include <SPI.h>

// Make sleep time random between 1 sec and 2 min.
// Comment to turn off and sleep default 2 min.
// Use unconnected pin A0 to get a random start seed for the pseudorandom
// generator.
//#define RAND_RSEED 0
// Random number to randomize sleep time as a factor of a random percentage.
long randSleepFactor;
// Sleep time between reports (in milliseconds)
unsigned long SLEEP_TIME = 120000;
// The digital input you attached your motion sensor.
// (Only 2 and 3 generates interrupt!)
#define DIGITAL_INPUT_MOTION 3
#define DIGITAL_OUTPUT_LED 4
// Usually the interrupt = pin -2 (on uno/nano anyway)
#define INTERRUPT DIGITAL_INPUT_MOTION-2
// Id of the sensor child
#define CHILD_ID_MOTION 1
// Id of the sensor child
#define CHILD_ID_LED 2

// Define last tripped value.
boolean last_tripped = false;
// Initialize motion message
MyMessage msgMotion(CHILD_ID_MOTION, V_TRIPPED);
// Initialize led message
MyMessage msgLight(CHILD_ID_LED, V_LIGHT);

MySensor gw;

void setup()
{
  // Initialize library and add callback for incoming messages
  gw.begin(incomingMessage, AUTO, false);

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Motion & LED", "1.0");

  // Set the motion sensor digital pin as input.
  pinMode(DIGITAL_INPUT_MOTION, INPUT);
  // Set the LED pin as an output.
  pinMode(DIGITAL_OUTPUT_LED, OUTPUT);
#ifdef RAND_RSEED
  // Set the analog pin for random seed as input.
  pinMode(ANALOG_INPUT_RSEED, INPUT);
#endif

#ifdef RAND_RSEED
  // Seed the random generator.
  randomSeed(analogRead(RAND_RSEED));
#endif

  // Register all sensors to gw (they will be created as child devices).
  gw.present(CHILD_ID_MOTION, S_MOTION);
  // Register led as sensor to gw, created as a child device.
  gw.present(CHILD_ID_LED, S_LIGHT);

  // Wait for a second.
  delay(1000);
  // Turn the LED on by making the voltage HIGH.
  digitalWrite(DIGITAL_OUTPUT_LED, HIGH);
  // Wait for a second.
  delay(1000);
  // Turn the LED off by making the voltage LOW.
  digitalWrite(DIGITAL_OUTPUT_LED, LOW);
  // Send message to gateway, setting LED as off.
  gw.send(msgLight.set("0"));
}

void loop()
{
  // Alway process incoming messages whenever possible
  gw.process();

  // Read digital motion value
  boolean tripped = digitalRead(DIGITAL_INPUT_MOTION) == HIGH;

  if (tripped) {
    digitalWrite(DIGITAL_OUTPUT_LED, HIGH);
  } else {
    digitalWrite(DIGITAL_OUTPUT_LED, LOW);
  }

  if (tripped != last_tripped) {
    Serial.println(tripped);
    // Send tripped value to gateway.
    gw.send(msgMotion.set(tripped ? 1 : 0));
    last_tripped = tripped;
  }
#ifdef RAND_RSEED
  randSleepFactor = random(1, 101);
  SLEEP_TIME = SLEEP_TIME * randSleepFactor / 100;
#endif
  Serial.println("Sleep");
  // Sleep until interrupt by motion sensor or for SLEEP_TIME.
  gw.sleep(INTERRUPT,CHANGE, SLEEP_TIME);
  Serial.println("Wake");
}

void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller.
  // But we better check anyway.
  if (message.type==V_LIGHT) {
     // Change relay state
     digitalWrite(DIGITAL_OUTPUT_LED, message.getBool()?1:0);

     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   }
}
