/*
* Documentation: http://www.mysensors.org
* Support Forum: http://forum.mysensors.org
*/

#include <MySensor.h>
/*
* Include all the other Necessary code here.
* The example code is limited to message exchange for mysensors
* with the controller (ha).
*/

#define CHILD_ID_HVAC  0  // childId
MyMessage msgHVACSetPointC(CHILD_ID_HVAC, V_HVAC_SETPOINT_COOL);
MyMessage msgHVACSpeed(CHILD_ID_HVAC, V_HVAC_SPEED);
MyMessage msgHVACFlowState(CHILD_ID_HVAC, V_HVAC_FLOW_STATE);

/*
* Include all the other Necessary code here.
* The example code is limited to message exchange for mysensors
* with the controller (ha).
*/

void setup()
{
  // Startup and initialize MySensors library.
  // Set callback for incoming messages.
  gw.begin(incomingMessage);

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("HVAC", "0.1");

  gw.present(CHILD_ID_HVAC, S_HVAC, "Thermostat");
  gw.send(msgHVACFlowState.set("Off"));
  gw.send(msgHVACSetPointC.set(target_temp));
  gw.send(msgHVACSpeed.set("Max"));
}

void loop() {
  // Process incoming messages (like config from server)
  gw.process();
}

void incomingMessage(const MyMessage &message) {
  String recvData = message.data;
  recvData.trim();
  switch (message.type) {
    case V_HVAC_SPEED:
    if(recvData.equalsIgnoreCase("auto")) fan_speed = 0;
    else if(recvData.equalsIgnoreCase("min")) fan_speed = 1;
    else if(recvData.equalsIgnoreCase("normal")) fan_speed = 2;
    else if(recvData.equalsIgnoreCase("max")) fan_speed = 3;
    processHVAC();
    break;
    case V_HVAC_SETPOINT_COOL:
    target_temp = message.getFloat();
    processHVAC();
    break;
    case V_HVAC_FLOW_STATE:
    if(recvData.equalsIgnoreCase("coolon") && (!Present_Power_On )){
      togglePower();
    }
    else if(recvData.equalsIgnoreCase("off") && Present_Power_On ){
      togglePower();
    }
    break;
  }
}
