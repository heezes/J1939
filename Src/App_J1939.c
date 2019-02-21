/*
 * App_J1939.c
 *
 *  Created on: 09-Feb-2019
 *      Author: Altamash Abdul Rahim
 */
#include "App_J1939.h"
#include "stdio.h"
#include "string.h"

#define TRANSMITTER	1

J1939_RX_MESSAGE_T J1939_rx_message;
J1939_TX_MESSAGE_T TxMsg;

void Can_DataCallback(J1939_RX_MESSAGE_T *msg)
{
	print_string("PGN: %d\r\n", msg->PGN);
	print_string("Data Size: %d Byte\r\n", msg->byte_count);
	print_string("Source Address: %d\r\n", msg->source_addr);
	print_string("Destination Address: %d\r\n", msg->dest_addr);
	print_string("Data Recevived: \r\n");
	for(int i = 0; i< msg->byte_count; i++)
		print_string("%d ",msg->data[i]);
	print_string("\r\n");
}

void App_Init(void)
{
	if(TL_init() == J1939_OK)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_SET);
		print_string("Transport Layer Initialized\r\n");
	}
	else
		print_string("Transport Layer Initialization Error\r\n");
#if TRANSMITTER
	memset(&TxMsg, 0, sizeof(J1939_TX_MESSAGE_T));
	TxMsg.PGN 			= 60928;
	u8 count			= 8;
	TxMsg.byte_count	= count;
	TxMsg.dest_addr		= GLOBADDR;
	TxMsg.priority		= CM_PRIORITY;
	for(int i = 0; i < TxMsg.byte_count; i++)
		TxMsg.data[i]	= i+1;
	if(Transmit_J1939_RTS(&TxMsg) == TRUE)
		print_string("CAN Frame SENT\r\n");
	else
		print_string("CAN Frame ERROR\r\n");
#endif
}

#define TX_DATA_PGN		60928

void App_Process(void)
{
	TL_process();
#if TRANSMITTER
	TP_tx_Process();
#else
	TL_process();
#endif


#if 0
#if TRANSMITTER
	HAL_Delay(1000);
	J1939_TX_MESSAGE_T TxMsg;
	memset(&TxMsg, 0, sizeof(J1939_TX_MESSAGE_T));
	TxMsg.PGN 			= 60928;
	u8 count			= 8;
	TxMsg.byte_count	= count;
	TxMsg.dest_addr		= GLOBADDR;
	TxMsg.priority		= CM_PRIORITY;
	for(int i = 0; i < TxMsg.byte_count; i++)
		TxMsg.data[i]	= i+1;
	//Transmit_J1939_BAM(TX_DATA_PGN, TxMsg.byte_count, GET_PACKET_NUMBER(TxMsg.byte_count));
	if(Transmit_J1939msg(&TxMsg))
		print_string("CAN Frame SENT\r\n");
	else
		print_string("CAN Frame ERROR\r\n");
	HAL_Delay(1000);
#else
	TL_process();
	TL_process();
#endif
#endif
}
