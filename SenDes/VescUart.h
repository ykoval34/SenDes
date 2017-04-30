#ifndef VESC_UART_H
#define VESC_UART_H
#include "config.h"

//#ifndef _CONFIG_h

/*TThis library was created on an Adruinio 2560 with different serial ports to have a better possibility
to debug. The serial ports are define with #define:
#define SERIALIO Serial1  		for the UART port to VESC
#define DEBUGSERIAL Serial		for debuging over USB
So you need here to define the right serial port for your arduino.
If you want to use debug, uncomment DEBUGSERIAL and define a port.*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "datatypes.h"


bool unpack_payload(uint8_t *message, int lenMes, uint8_t *payload, int lenPa);
bool process_read_package(uint8_t *message, mc_values &values, int len);

///PackSendPayload Packs the payload and sends it over Serial.
///Define in a Config.h a SERIAL with the Serial in Arduino Style you want to you
///@param: payload as the payload [unit8_t Array] with length of int lenPayload
///@return the number of bytes send
int send_payload(uint8_t* payload, int lenPay);

///Sends a command to VESC and stores the returned data
///@param bldcMeasure struct with received data
//@return true if sucess
bool vesc_get_values(struct mc_values &values);

///ReceiveUartMessage receives the a message over Serial
///Define in a Config.h a SERIAL with the Serial in Arduino Style you want to you
///@parm the payload as the payload [unit8_t Array]
///@return the number of bytes receeived within the payload

int process_received_msg(uint8_t* payloadReceived);

#endif //VESC_UART_H
