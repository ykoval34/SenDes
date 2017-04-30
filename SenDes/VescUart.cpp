#include "VescUart.h"
#include "buffer.h"
#include "crc.h"
#include "datatypes.h"
#include "config.h"

bool process_read_package(uint8_t* message, mc_values& values, int len);
bool unpack_payload(uint8_t *message, int lenMes, uint8_t *payload, int lenPay);

int process_received_msg(uint8_t *payloadReceived) {

	//Messages <= 255 start with 2. 2nd byte is length
	//Messages >255 start with 3. 2nd and 3rd byte is length combined with 1st >>8 and then &0xFF

	int counter = 0;
	int endMessage = 256;
	bool messageRead = false;
	uint8_t messageReceived[256];
	int lenPayload = 0;

	while (SERIALIO.available()) {

		messageReceived[counter++] = SERIALIO.read();

		if (counter == 2) {//case if state of 'counter' with last read 1

			switch (messageReceived[0])
			{
			case 2:
				endMessage = messageReceived[1] + 5; //Payload size + 2 for sice + 3 for SRC and End.
				lenPayload = messageReceived[1];
				break;
			case 3:
				//ToDo: Add Message Handling > 255 (starting with 3)
				break;
			default:
				break;
			}

		}
		if (counter >= sizeof(messageReceived))
		{
			break;
		}

		if (counter == endMessage && messageReceived[endMessage - 1] == 3) {//+1: Because of counter++ state of 'counter' with last read = "endMessage"
			messageReceived[endMessage] = 0;

			messageRead = true;
			break; //Exit if end of message is reached, even if there is still more data in buffer.
		}
	}
	bool unpacked = false;
	if (messageRead) {
		unpacked = unpack_payload(messageReceived, endMessage, payloadReceived, messageReceived[1]);
	}
	if (unpacked)
	{
		return lenPayload; //Message was read

	}
	else {
		return 0; //No Message Read
	}
}

bool unpack_payload(uint8_t *message, int lenMes, uint8_t *payload, int lenPay) {
	uint16_t crcMessage = 0;
	uint16_t crcPayload = 0;
	//Rebuild src:
	crcMessage = message[lenMes - 3] << 8;
	crcMessage &= 0xFF00;
	crcMessage += message[lenMes - 2];

	//Extract payload:
	memcpy(payload, &message[2], message[1]);

	crcPayload = crc16(payload, message[1]);

	if (crcPayload == crcMessage)
	{

		return true;
	}
	else
	{
		return false;
	}
}

int send_payload(uint8_t* payload, int lenPay) {
	uint16_t crcPayload = crc16(payload, lenPay);
	int count = 0;
	uint8_t messageSend[256];

	if (lenPay <= 256)
	{
		messageSend[count++] = 2;
		messageSend[count++] = lenPay;
	}
	else
	{
		messageSend[count++] = 3;
		messageSend[count++] = (uint8_t)(lenPay >> 8);
		messageSend[count++] = (uint8_t)(lenPay & 0xFF);
	}
	memcpy(&messageSend[count], payload, lenPay);

	count += lenPay;
	messageSend[count++] = (uint8_t)(crcPayload >> 8);
	messageSend[count++] = (uint8_t)(crcPayload & 0xFF);
	messageSend[count++] = 3;
	messageSend[count] = NULL;
	//Sending package
	SERIALIO.write(messageSend, count);

	//Returns number of send bytes
	return count;
}


bool process_read_package(uint8_t* message, mc_values& values, int len) {
	COMM_PACKET_ID packetId;
	int32_t ind = 0;

	packetId = (COMM_PACKET_ID)message[0];
	message++;//Eliminates the message id
	len--;

	switch (packetId)
	{
	case COMM_GET_VALUES:
		ind = 0;
		values.temp_mos1 = buffer_get_float16(message, 10.0, &ind);
		values.temp_mos2 = buffer_get_float16(message, 10.0, &ind);
		values.temp_mos3 = buffer_get_float16(message, 10.0, &ind);
		values.temp_mos4 = buffer_get_float16(message, 10.0, &ind);
		values.temp_mos5 = buffer_get_float16(message, 10.0, &ind);
		values.temp_mos6 = buffer_get_float16(message, 10.0, &ind);
		values.temp_pcb = buffer_get_float16(message, 10.0, &ind);

		values.current_motor = buffer_get_float32(message, 100.0, &ind);
		values.current_in = buffer_get_float32(message, 100.0, &ind);
		values.duty_now = buffer_get_float16(message, 1000.0, &ind);
		values.rpm = buffer_get_int32(message, &ind);
		values.v_in = buffer_get_float16(message, 10.0, &ind);
		values.amp_hours = buffer_get_float32(message, 10000.0, &ind);
		values.amp_hours_charged = buffer_get_float32(message, 10000.0, &ind);
		ind += 8; //Skip 9 bit
		values.tachometer = buffer_get_int32(message, &ind);
		values.tachometer_abs = buffer_get_int32(message, &ind);
		values.fault_code = (mc_fault_code)message[ind++];

		return true;
		break;

	default:
		return false;
		break;
	}

}

bool vesc_get_values(mc_values& values) {
	uint8_t command[1] = { COMM_GET_VALUES };
	uint8_t payload[256];
	send_payload(command, 1);
	//delay(15); //needed, otherwise data is not read
	int lenPayload = process_received_msg(payload);
	if (lenPayload > 0) {
		bool read = process_read_package(payload, values, lenPayload); //returns true if sucessfull
		return read;
	}
	else
	{
		return false;
	}
}


