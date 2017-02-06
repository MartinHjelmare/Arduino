/**
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * http://www.mysensors.org/build/gps
 */

// Enable debug prints to serial monitor
#define MY_DEBUG
// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
#define MY_RF24_CHANNEL	78

#include <MySensors.h>

#define SN "GPS Sensor"
#define SV "1.0"

// GPS position send interval (in millisectonds)
#define GPS_SEND_INTERVAL 30000
// The child id used for the gps sensor
#define CHILD_ID_GPS 1

MyMessage msg(CHILD_ID_GPS, V_POSITION);

// Last time GPS position was sent to controller
unsigned long lastGPSSent = -31000;

// Some buffers
char latBuf[11];
char lngBuf[11];
char altBuf[6];
char payload[30];

// Dummy values. Implementation of real GPS device is not done.
float gpsLocationLat = 40.741895;
float gpsLocationLng = -73.989308;
float gpsAltitudeMeters = 12.0;

void setup() {

}

void presentation() {
  sendSketchInfo(SN, SV);
  present(CHILD_ID_GPS, S_GPS);
}

void loop()
{
  unsigned long currentTime = millis();

  // Evaluate if it is time to send a new position
  bool timeToSend = currentTime - lastGPSSent > GPS_SEND_INTERVAL;

  if (timeToSend) {
    // Send current gps location
    // Build position and altitude string to send
    dtostrf(gpsLocationLat, 1, 6, latBuf);
    dtostrf(gpsLocationLng, 1, 6, lngBuf);
    dtostrf(gpsAltitudeMeters, 1, 0, altBuf);
    sprintf(payload, "%s,%s,%s", latBuf, lngBuf, altBuf);

    Serial.print(F("Position: "));
    Serial.println(payload);

    send(msg.set(payload));
    lastGPSSent = currentTime;
  }
}
