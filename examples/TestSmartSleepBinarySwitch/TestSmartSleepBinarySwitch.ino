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
 * Interrupt driven binary switch example with dual interrupts
 * Author: Patrick 'Anticimex' Fallberg
 * Connect one button or door/window reed switch between
 * digitial I/O pin 3 (BUTTON_PIN below) and GND and the other
 * one in similar fashion on digital I/O pin 2.
 * This example is designed to fit Arduino Nano/Pro Mini
 *
 */


// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
#define MY_RF24_CHANNEL	78

#include <SPI.h>
#include <MySensors.h>

#define SN "Binary Sensor"
#define SV "1.0"

#define CHILD_ID_1 1
#define CHILD_ID_2 2

#define BUTTON_PIN 3 // Arduino Digital I/O pin for button/reed switch
#define LED_PIN 5 // LED pin

MyMessage msgButton(CHILD_ID_1, V_TRIPPED);
MyMessage msgLed(CHILD_ID_2, V_STATUS);
bool state = false;

void setup()
{
  // Setup the buttons
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SN, SV);
  // Present sensor
  present(CHILD_ID_1, S_DOOR);
  present(CHILD_ID_2, S_BINARY);
  send(msgButton.set(0));
  send(msgLed.set(0));
}

// Loop will iterate on changes on the BUTTON_PINs
void loop()
{
  uint8_t value;
  static uint8_t sentValue = 2;

  // Short delay to allow buttons to properly settle
  sleep(5);

  value = digitalRead(BUTTON_PIN);

  if (value != sentValue) {
     // Value has changed from last transmission, send the updated value
     send(msgButton.set(value==LOW ? 1 : 0));
     if (value==LOW) {
       state = state ? false : true;
     }
     digitalWrite(LED_PIN, state ? HIGH : LOW);
     send(msgLed.set(state ? 1 : 0));
     sentValue = value;
     Serial.print("Value changed in loop for sensor:");
     Serial.print(", New status: ");
     Serial.println(value==LOW);
  }

  // Sleep until something happens with the sensor
  if (value==HIGH) {
    smartSleep(digitalPinToInterrupt(BUTTON_PIN), CHANGE, 20000);
  } else {
    sleep(digitalPinToInterrupt(BUTTON_PIN), CHANGE, 20000);
  }
}

void receive(const MyMessage &message) {
  if (message.type==V_STATUS && message.sensor==CHILD_ID_2) {
     // Change LED state
     digitalWrite(LED_PIN, message.getBool() ? HIGH : LOW);
     send(msgLed.set(message.getBool() ? 1 : 0));
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   }
}
