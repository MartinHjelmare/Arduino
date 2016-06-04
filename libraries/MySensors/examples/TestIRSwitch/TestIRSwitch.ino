/*
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * http://www.mysensors.org/build/ir
 */

#include <MySensor.h>
#include <SPI.h>
#include <IRLib.h>

#define SN "IR Sensor"
#define SV "1.0"
#define CHILD_ID 1

MySensor gw;

char code[10] = "abcd01234";
char oldCode[10] = "abcd01234";
MyMessage msgCodeRec(CHILD_ID, V_IR_RECEIVE);
MyMessage msgCode(CHILD_ID, V_IR_SEND);
MyMessage msgSendCode(CHILD_ID, V_LIGHT);

void setup()
{
  gw.begin(incomingMessage);
  gw.sendSketchInfo(SN, SV);
  gw.present(CHILD_ID, S_IR);
  // Send initial values.
  gw.send(msgCodeRec.set(code));
  gw.send(msgCode.set(code));
  gw.send(msgSendCode.set(0));
}

void loop()
{
  gw.process();
  // IR receiver not implemented, just a dummy report of code when it changes
  if (String(code) != String(oldCode)) {
    Serial.print("Code received ");
    Serial.println(code);
    gw.send(msgCodeRec.set(code));
    strcpy(oldCode, code);
  }
}

void incomingMessage(const MyMessage &message) {
  if (message.type==V_LIGHT) {
    // IR sender not implemented, just a dummy print.
    if (message.getBool()) {
      Serial.print("Sending code ");
      Serial.println(code);
    }
    gw.send(msgSendCode.set(message.getBool() ? 1 : 0));
    // Always turn off device
    gw.wait(100);
    gw.send(msgSendCode.set(0));
  }
  if (message.type == V_IR_SEND) {
    // Retrieve the IR code value from the incoming message.
    String codestring = message.getString();
    codestring.toCharArray(code, sizeof(code));
    Serial.print("Changing code to ");
    Serial.println(code);
    gw.send(msgCode.set(code));
  }
}
