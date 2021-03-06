// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
#define MY_RF24_CHANNEL 80
// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

#include <SPI.h>
#include <MySensors.h>

#define SN "TextSensor"
#define SV "1.0"
#define CHILD_ID 1

MyMessage textMsg(CHILD_ID, V_TEXT);
bool initialValueSent = false;

void setup(void)
{
}

void presentation()
{
	sendSketchInfo(SN, SV);
	present(CHILD_ID, S_INFO, "TextSensor1");
}

void loop()
{
	if (!initialValueSent)
	{
		Serial.println("Sending initial value");
		// Send initial values.
		send(textMsg.set("-"));
		Serial.println("Requesting initial value from controller");
		request(CHILD_ID, V_TEXT);
		wait(2000, C_SET, V_TEXT);
	}
}

void receive(const MyMessage &message)
{
	if (message.type == V_TEXT)
	{
		if (!initialValueSent)
		{
			Serial.println("Receiving initial value from controller");
			initialValueSent = true;
		}
		// Dummy print
		Serial.print("Message: ");
		Serial.print(message.sensor);
		Serial.print(", Message: ");
		Serial.println(message.getString());
		// Send message to controller
		send(textMsg.set(message.getString()));
	}
}
