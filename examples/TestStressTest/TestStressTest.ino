/*
   PROJECT: MySensors - network stresstest
   PROGRAMMER: AWI
   DATE: 10 august 2015, update september 15, 2015
   FILE: AWI_Universal_Test_50.ino
   LICENSE: Public domain
   -in conjunction with MySensors		http://www.mysensors.org

   STATUS:                      never finished, MySensors 1.5 (developement S_ & V_ types)
   KNOWN ISSUES:                multiple initialisation of sensors (not a real issue)
   TBD:
   SUMMARY:
        multisensor for controller stress test
        1. Presents sensors to controller
        2. Sends (per sensor) values in with x iterations in y increments
        repeat

   Setup: sends values for al sensor specified in setup array

   Update:	20150904:       Changed structure and type definitions (makes is easier to read)
                                        20150915:       Changed the structure so that one sensor can
                                           be sent multiple variables and option for SET/ REQ

   Special:	Stores test data in progmem to save RAM space
                        verbose listing of S_ & V_ names (lookup tables)
 */

#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_NODE_ID 50 // fixed MySensors node ID

#include <MySensors.h>

// reference: typedef enum {P_STRING, P_BYTE, P_INT16, P_UINT16, P_LONG32, P_ULONG32, P_CUSTOM,
// P_FLOAT32 } mysensor_payload ; // MySensor types

typedef union {          // MySensor Payload types (only p_String used in this sketch)
  char     p_String[16]; // reduce characters to limit RAM size
  int16_t  p_Int16;
  uint16_t p_Uint16;
  int32_t  p_Int32;
  uint32_t p_Uint32;
  byte     p_Byte;
  float    p_Float32;
} payLoadType;

// data type for test vector
enum sActionT { vSet, vReq, vNone }; // sets the action to be performed (vNone does nothing)

typedef struct {
  byte             sensorNo;         // number of the sensor in the node (20150910)
  byte             s_type;           // S_ type of the sensor number (only for init)
  char             s_name[10];       // sensor name (max 9 char)
  byte             v_type;           // data V_ type to be sent
  mysensor_payload dataType;         // data type identifier (uses MySensors MyTypes definition)
  payLoadType      testStartValue;   // start value for test
  byte             testIterations;   // number of values sent
  float            testIncrement;    // test increment (different behaviour for each datatype)
  sActionT         sAction;          // sets the action to be performed (vNone does nothing)
  boolean          vAck;             // indicates if ack is to be sent (not implemented yet)
} testDataType;


// ***********************************************************************************************
// TESTDATA: testdata & initialization, change to adapt to your needs
// ***********************************************************************************************
const unsigned long SLEEP_PERIOD = 500;   // Period between individual tests in ms
const unsigned long loopDelay    = 300;   // Sleep period in ms)
const boolean setFlag            = true;  // global flag to enable "SET" for all testdata V_ types (
                                          // && with test flag in vector)
const boolean reqFlag            = false; // global flag to enable "REQ" for all V_ types in sensor
                                          // (controller dependent) (&& with test flag in vector)
const boolean testAck            = false; // global flag to enable tests to be acknowledged (&& with
                                          // test flag in vector)
// testvectors - Each line is one "testvector"
// Legend {<sensor no>), <S_ type>, "<sensor name>", <V_type>, <data type>, "<start value>",
// <iterations>, <increment>, <set/req>, <ack>},
const testDataType PROGMEM testDataP[] = {
  // size determined by number of lines
  // {S_no, S_ type, "Sname", V_type, dataType "startVal", iter, incr, Set/Req>, ack},
  { 0x01, S_DOOR,             "Door1",           V_TRIPPED,                  		 P_BYTE,
    "1",           5, 1, vSet, false },
  { 0x02, S_MOTION,           "Motion1",         V_TRIPPED,                      P_BYTE,
    "1",           5, 1, vSet, false },
  { 0x03, S_SMOKE,            "Smoke1",          V_TRIPPED,                      P_BYTE,
    "1",           5, 1, vSet, false },
  { 0x04, S_BINARY,           "Binar1",          V_STATUS,                       P_BYTE,
    "0",           5, 1, vSet, false },
  { 0x05, S_DIMMER,           "Dimmer1",         V_PERCENTAGE,                   P_BYTE,
    "60",          5, 1, vSet, false },
  { 0x06, S_COVER,            "Cover1",          V_PERCENTAGE,                   P_BYTE,
    "80",          5, 1, vSet, false },
  { 0x07, S_TEMP,             "Temp1",           V_TEMP,                         P_FLOAT32,
    "25",          5, 1, vSet, false },
  { 0x08, S_HUM,              "Hum1",            V_HUM,                          P_FLOAT32,
    "75",          5, 1, vSet, false },
  { 0x09, S_BARO,             "Baro1",           V_PRESSURE,                     P_FLOAT32,
    "1080",        5, 1, vSet, false },
  { 0x0A, S_WIND,             "Wind1",           V_WIND,                         P_BYTE,
    "4",           5, 1, vSet, false },
  { 0x0C, S_RAIN,             "Rain1",           V_RAIN,                         P_FLOAT32,
    "60",          5, 1, vSet, false },
  { 0x0D, S_UV,               "Uv1",             V_UV,                           P_FLOAT32,
    "80",          5, 1, vSet, false },
  { 0x0E, S_WEIGHT,           "Weight1",         V_WEIGHT,                       P_FLOAT32,
    "12",          5, 1, vSet, false },
  { 0x0F, S_POWER,            "Power1",          V_WATT,                         P_FLOAT32,
    "80",          5, 1, vSet, false },
  { 0x10, S_HEATER,           "Heater1",         V_HVAC_FLOW_STATE,              P_BYTE,
    "1",           5, 1, vSet, false },
  { 0x11, S_DISTANCE,         "Distance1",       V_DISTANCE,                     P_FLOAT32,
    "150",         5, 1, vSet, false },
  { 0x12, S_LIGHT_LEVEL,      "LightL1",         V_LEVEL,                        P_FLOAT32,
    "35",          5, 1, vSet, false },
  { 0x13, S_LOCK,             "Lock11",          V_LOCK_STATUS,                  P_BYTE,
    "1",           5, 1, vSet, false },
  { 0x14, S_IR,               "Ir1",             V_IR_RECEIVE,                   P_FLOAT32,
    "1234",        5, 1, vSet, false },
  { 0x15, S_WATER,            "Water1",          V_FLOW,                         P_FLOAT32,
    "80",          5, 1, vSet, false },
  { 0x16, S_AIR_QUALITY,      "AirQ1",           V_LEVEL,                        P_FLOAT32,
    "80",          5, 1, vSet, false },
  { 0x17, S_CUSTOM,           "Custom1",         V_VAR1,                         P_BYTE,
    "80",          5, 1, vSet, false },
  { 0x18, S_DUST,             "Dust1",           V_LEVEL,                        P_BYTE,
    "80",          5, 1, vSet, false },
  { 0x19, S_SCENE_CONTROLLER, "Scene1",          V_SCENE_ON,                     P_BYTE,
    "1",           5, 1, vSet, false },
  { 0x1A, S_RGB_LIGHT,        "RgbL1",           V_RGB,                          P_ULONG32,
    "808080",      5, 1, vSet, false },
  { 0x1B, S_RGBW_LIGHT,       "RgbwL1",          V_RGBW,                         P_ULONG32,
    "80808080",      5, 1, vSet, false },
  { 0x1C, S_COLOR_SENSOR,     "Color1",          V_RGB,                          P_ULONG32,
    "807060",      5, 1, vSet, false },
  { 0x1D, S_HVAC,             "Hvac1",           V_HVAC_FLOW_STATE,              P_STRING,
    "HeatOn",          5, 1, vSet, false },
  { 0x1E, S_MULTIMETER,       "Multi1",          V_VOLTAGE,                      P_FLOAT32,
    "80",          5, 1, vSet, false },
  { 0x1E, S_MULTIMETER,       "Multi1",          V_CURRENT,                      P_FLOAT32,
    "80",          5, 1, vSet, false },
  { 0x1E, S_MULTIMETER,       "Multi1",          V_IMPEDANCE,                    P_FLOAT32,
    "80",          5, 1, vSet, false },
  { 0x1F, S_SPRINKLER,        "Sprink1",         V_STATUS,                       P_BYTE,
    "1",           5, 1, vSet, false },
  { 0x20, S_WATER_LEAK,       "WaterL1",         V_TRIPPED,                      P_BYTE,
    "1",           5, 1, vSet, false },
  { 0x21, S_SOUND,            "Sound1",          V_LEVEL,                        P_BYTE,
    "80",          5, 1, vSet, false },
  { 0x22, S_VIBRATION,        "Vibr1",           V_LEVEL,                        P_BYTE,
    "80",          5, 1, vSet, false },
  { 0x23, S_MOISTURE,         "Moist1",          V_LEVEL,                        P_BYTE,
    "80",          5, 1, vSet, false },
  { 0x24, S_INFO,             "Info1",           V_TEXT,                         P_STRING,
    "ABC",         5, 1, vSet, false },
  { 0x25, S_GAS,              "Gas1",            V_VOLUME,                       P_BYTE,
    "80",          5, 1, vSet, false },
};

// END TESTDATA
testDataType testData; // storage for current testdata (to copy from progmem)

// Initialize messages (only generic message here)
MyMessage Msg;         // instantiate message to fill "on the fly"

// Global variables to number of test elements
int noSensors;         // number of active sensors in node
int noTests;           // noTests == noSensors for now

void setup() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("AWI Universal test 50", "1.2");
  noTests   = sizeof(testDataP) / sizeof(testData); // determine number of tests from data array
  noSensors = noTests;                              //  == noTests for now (needs to be calculated
                                                    // or read)
  Serial.println("\n  Stress test for Mysensors network (20170207).\n");
  Serial.print("Number of Sensors: "); Serial.println(noSensors);
  Serial.print("Number of Tests:   "); Serial.println(noTests);

  for (int i = 0; i < noSensors; i++) {
    // Register al sensors (they will be created as child devices)
    // present all sensors in list (sensor_no, sensor_type, sensor_name). Multiple sensors are
    // presented multiple times (just to be aware).
    memcpy_P(&testData, &testDataP[i], sizeof(testData)); // copy testdata from Progmem
    present(testData.sensorNo, testData.s_type, testData.s_name);
    wait(loopDelay);
  }
}

void loop() {
  for (int i = 0; i < noTests; i++) {
    memcpy_P(&testData, &testDataP[i], sizeof(testData)); // copy testdata from Progmem
    printTest();
    Msg.setSensor(testData.sensorNo);                     // set the sensor number
    Msg.setType(testData.v_type);                         // set the sensor type
    boolean setAck = testAck && testData.vAck;            // Ack depends on global testAck and ack
                                                          // for test

    if (setFlag && (testData.sAction == vSet)) {          // if global Set is activated and
                                                          // testvalue for type = Set
      if (testData.dataType == P_BYTE) {
        uint8_t testPayload = atoi(testData.testStartValue.p_String);

        for (byte j = 0; j < testData.testIterations; j++) {
          send(Msg.set(testPayload), setAck);
          wait(SLEEP_PERIOD);
          testPayload += testData.testIncrement;
        }
      } else if (testData.dataType == P_INT16) {
        int testPayload = atoi(testData.testStartValue.p_String);

        for (byte j = 0; j < testData.testIterations; j++) {
          send(Msg.set(testPayload), setAck);
          wait(SLEEP_PERIOD);
          testPayload += testData.testIncrement;
        }
      } else if (testData.dataType == P_UINT16) {
        uint16_t testPayload = atoi(testData.testStartValue.p_String);

        for (byte j = 0; j < testData.testIterations; j++) {
          send(Msg.set(testPayload), setAck);
          wait(SLEEP_PERIOD);
          testPayload += testData.testIncrement;
        }
      } else if (testData.dataType == P_LONG32) {
        uint32_t testPayload = atol(testData.testStartValue.p_String);

        for (byte j = 0; j < testData.testIterations; j++) {
          send(Msg.set(testPayload), setAck);
          wait(SLEEP_PERIOD);
          testPayload += testData.testIncrement;
        }
      } else if (testData.dataType == P_ULONG32) {
        uint32_t testPayload = atol(testData.testStartValue.p_String);

        for (byte j = 0; j < testData.testIterations; j++) {
          send(Msg.set(testPayload), setAck);
          wait(SLEEP_PERIOD);
          testPayload += testData.testIncrement;
        }
      } else if (testData.dataType == P_FLOAT32) {
        float testPayload = atof(testData.testStartValue.p_String);

        for (byte j = 0; j < testData.testIterations; j++) {
          send(Msg.set(testPayload, 4), setAck);
          wait(SLEEP_PERIOD);
          testPayload += testData.testIncrement;
        }
      } else if (testData.dataType == P_STRING) {
        for (byte j = 0; j < testData.testIterations; j++) {
          send(Msg.set(testData.testStartValue.p_String), setAck); // just send the string
          wait(SLEEP_PERIOD);

          // testPayload += testData.testIncrement ; //string is not incremented
        }
      }
    } else if (reqFlag && (testData.sAction == vReq)) {
      // if global Req is activated and testvalue for type = Req
      request(i, testData.v_type);
    }

    // else do nothing (set or req)
    wait(loopDelay);
  }
}

void receive(const MyMessage& message) {
  Serial.print("Incoming Message");
  Serial.print("Sensor(hex):");
  Serial.print(message.sensor, HEX);
  Serial.print(", V_type: ");
  Serial.print(message.type);
  Serial.print(", payload: ");
  Serial.println(message.getString());
}

// Lookuptables for S_ & V_ type names
const char PROGMEM sTypes[][20] = {
  "S_DOOR",             // Door sensor, V_TRIPPED, V_ARMED
  "S_MOTION",           // Motion sensor, V_TRIPPED, V_ARMED
  "S_SMOKE",            // Smoke sensor, V_TRIPPED, V_ARMED
  "S_BINARY",           // Binary light or relay, V_STATUS (or V_LIGHT), V_WATT (same as "S_LIGHT)
  "S_DIMMER",           // Dimmable light or fan device, V_STATUS (on/off), V_DIMMER (dimmer level
                        // 0-100), V_WATT
  "S_COVER",            // Blinds or window cover, V_UP, V_DOWN, V_STOP, V_DIMMER (open/close to a
                        // percentage)
  "S_TEMP",             // Temperature sensor, V_TEMP
  "S_HUM",              // Humidity sensor, V_HUM
  "S_BARO",             // Barometer sensor, V_PRESSURE, V_FORECAST
  "S_WIND",             // Wind sensor, V_WIND, V_GUST
  "S_RAIN",             // Rain sensor, V_RAIN, V_RAINRATE
  "S_UV",               // Uv sensor, V_UV
  "S_WEIGHT",           // Personal scale sensor, V_WEIGHT, V_IMPEDANCE
  "S_POWER",            // Power meter, V_WATT, V_KWH
  "S_HEATER",           // Header device, V_HVAC_SETPOINT_HEAT, V_HVAC_FLOW_STATE
  "S_DISTANCE",         // Distance sensor, V_DISTANCE
  "S_LIGHT_LEVEL",      // Light level sensor, V_LIGHT_LEVEL (uncalibrated in percentage),  V_LEVEL
                        // (light level in lux)
  "S_ARDUINO_NODE",     // Used (internally) for presenting a non-repeating Arduino node
  "S_ARDUINO_REP_NODE", // Used (internally) for presenting a repeating Arduino node
  "S_LOCK",             // Lock device, V_LOCK_STATUS
  "S_IR",               // Ir device, V_IR_SEND, V_IR_RECEIVE
  "S_WATER",            // Water meter, V_FLOW, V_VOLUME
  "S_AIR_QUALITY",      // Air quality sensor, V_LEVEL
  "S_CUSTOM",           // Custom sensor
  "S_DUST",             // Dust sensor, V_LEVEL
  "S_SCENE_CONTROLLER", // Scene controller device, V_SCENE_ON, V_SCENE_OFF.
  "S_RGB_LIGHT",        // RGB light. Send color component data using V_RGB. Also supports V_WATT
  "S_RGBW_LIGHT",       // RGB light with an additional White component. Send data using V_RGBW.
                        // Also supports V_WATT
  "S_COLOR_SENSOR",     // Color sensor, send color information using V_RGB
  "S_HVAC",             // Thermostat/HVAC device. V_HVAC_SETPOINT_HEAT, V_HVAC_SETPOINT_COLD,
                        // V_HVAC_FLOW_STATE, V_HVAC_FLOW_MODE
  "S_MULTIMETER",       // Multimeter device, V_VOLTAGE, V_CURRENT, V_IMPEDANCE
  "S_SPRINKLER",        // Sprinkler, V_STATUS (turn on/off), V_TRIPPED (if fire detecting device)
  "S_WATER_LEAK",       // Water leak sensor, V_TRIPPED, V_ARMED
  "S_SOUND",            // Sound sensor, V_TRIPPED, V_ARMED, V_LEVEL (sound level in dB)
  "S_VIBRATION",        // Vibration sensor, V_TRIPPED, V_ARMED, V_LEVEL (vibration in Hz)
  "S_MOISTURE",         // Moisture sensor, V_TRIPPED, V_ARMED, V_LEVEL (water content or moisture
                        // in percentage?)
  "S_INFO",             // LCD text device / Simple information device on controller, V_TEXT
  "S_GAS",              // Gas meter, V_FLOW, V_VOLUME  (development)
};

// Type of sensor data (for set/req/ack messages)
const char PROGMEM vTypes[][22] {
  "V_TEMP",            // S_TEMP. Temperature S_TEMP, S_HEATER, S_HVAC
  "V_HUM",             // S_HUM. Humidity
  "V_STATUS",          //  S_LIGHT, S_DIMMER, S_SPRINKLER, S_HVAC, S_HEATER. Used for
                       // setting/reporting binary (on/off) status. 1=on, 0=off
  "V_PERC/DIM",        // S_DIMMER. Used for sending a percentage value 0-100 (%).
  "V_PRESSURE",        // S_BARO. Atmospheric Pressure
  "V_FORECAST",        // S_BARO. Whether forecast. string of "stable", "sunny", "cloudy",
                       // "unstable", "thunderstorm" or "unknown"
  "V_RAIN",            // S_RAIN. Amount of rain
  "V_RAINRATE",        // S_RAIN. Rate of rain
  "V_WIND",            // S_WIND. Wind speed
  "V_GUST",            // S_WIND. Gust
  "V_DIRECTION",       // S_WIND. Wind direction 0-360 (degrees)
  "V_UV",              // S_UV. UV light level
  "V_WEIGHT",          // S_WEIGHT. Weight(for scales etc)
  "V_DISTANCE",        // S_DISTANCE. Distance
  "V_IMPEDANCE",       // S_MULTIMETER, S_WEIGHT. Impedance value
  "V_ARMED",           // S_DOOR, S_MOTION, S_SMOKE, S_SPRINKLER. Armed status of a security sensor.
                       // 1 = Armed, 0 = Bypassed
  "V_TRIPPED",         // S_DOOR, S_MOTION, S_SMOKE, S_SPRINKLER, S_WATER_LEAK, S_SOUND,
                       // S_VIBRATION, S_MOISTURE. Tripped status of a security sensor. 1 = Tripped,
                       // 0
  "V_WATT",            // S_POWER, S_LIGHT, S_DIMMER, S_RGB, S_RGBW. Watt value for power meters
  "V_KWH",             // S_POWER. Accumulated number of KWH for a power meter
  "V_SCENE_ON",        // S_SCENE_CONTROLLER. Turn on a scene
  "V_SCENE_OFF",       // S_SCENE_CONTROLLER. Turn of a scene
  "V_HVAC_FLOW_STATE", // S_HEATER, S_HVAC. HVAC flow state ("Off", "HeatOn", "CoolOn", or
                       // "AutoChangeOver")
  "V_HVAC_SPEED",      // S_HVAC, S_HEATER. HVAC/Heater fan speed ("Min", "Normal", "Max", "Auto")
  "V_LIGHT_LEVEL",     // S_LIGHT_LEVEL. Uncalibrated light level. 0-100%. Use "V_LEVEL for light
                       // level in lux
  "V_VAR1", "V_VAR2", "V_VAR3", "V_VAR4", "V_VAR5",
  "V_UP",              // S_COVER. Window covering. Up
  "V_DOWN",            // S_COVER. Window covering. Down
  "V_STOP",            // S_COVER. Window covering. Stop
  "V_IR_SEND",         // S_IR. Send out an IR-command
  "V_IR_RECEIVE",      // S_IR. This message contains a received IR-command
  "V_FLOW",            // S_WATER. Flow of water (in meter)
  "V_VOLUME",          // S_WATER. Water volume
  "V_LOCK_STATUS",     // S_LOCK. Set or get lock status. 1=Locked, 0=Unlocked
  "V_LEVEL",           // S_DUST, S_AIR_QUALITY, S_SOUND (dB), S_VIBRATION (hz), S_LIGHT_LEVEL (lux)
  "V_VOLTAGE",         // S_MULTIMETER
  "V_CURRENT",         // S_MULTIMETER
  "V_RGB",             // S_RGB_LIGHT, S_COLOR_SENSOR.
  // Used for sending color information for multi color LED lighting or color sensors.
  // Sent as ASCII hex: RRGGBB (RR=red, GG=green, BB=blue component)
  "V_RGBW",            // S_RGBW_LIGHT
  // Used for sending color information to multi color LED lighting.
  // Sent as ASCII hex: RRGGBBWW (WW=white component)
  "V_ID",              // S_TEMP
  // Used for sending in sensors hardware ids (i.e. OneWire DS1820b).
  "V_UNIT_PREFIX",     // S_DUST, S_AIR_QUALITY
  // Allows sensors to send in a string representing the
  // unit prefix to be displayed in GUI, not parsed by controller! E.g. cm, m, km, inch.
  // Can be used for S_DISTANCE or gas concentration
  "V_HVAC_SETPOINT_COOL", // S_HVAC. HVAC cool setpoint (Integer between 0-100)
  "V_HVAC_SETPOINT_HEAT", // S_HEATER, S_HVAC. HVAC/Heater setpoint (Integer between 0-100)
  "V_HVAC_FLOW_MODE",     // S_HVAC. Flow mode for HVAC ("Auto", "ContinuousOn", "PeriodicOn")
  "V_TEXT",               // S_INFO. Text message to display on LCD or controller device
};

void printTest() {
  char TypeTmp[20];                           // temporary char array

  Serial.print("Sensor(hex): "); Serial.print(testData.sensorNo, HEX);
  strcpy_P(TypeTmp, sTypes[testData.s_type]); // copy string from Progmem
  Serial.print(", "); Serial.print(TypeTmp);  // print string
  strcpy_P(TypeTmp, vTypes[testData.v_type]);
  Serial.print(", sName: "); Serial.print(testData.s_name);
  Serial.print(", "); Serial.print(TypeTmp);  // print string
  Serial.print(", start: "); Serial.println(testData.testStartValue.p_String);
}

/* for reference only
   // sensor data types
   // typedef enum {P_STRING, P_BYTE, P_INT16, P_UINT16, P_LONG32, P_ULONG32, P_CUSTOM, P_FLOAT32 }
      MyTypes ;
 */
