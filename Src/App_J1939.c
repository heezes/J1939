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
}

#define TX_DATA_PGN		60928

void App_Process(void)
{
#if TRANSMITTER
	HAL_Delay(1000);
	J1939_TX_MESSAGE_T TxMsg;
	memset(&TxMsg, 0, sizeof(J1939_TX_MESSAGE_T));
	TxMsg.PGN 			= TP_CM;
	u8 count			= 48;
	TxMsg.byte_count	= count;
	TxMsg.dest_addr		= GLOBADDR;
	TxMsg.priority		= CM_PRIORITY;
	for(int i = 0; i < TxMsg.byte_count; i++)
		TxMsg.data[i]	= i+1;
	Transmit_J1939_BAM(TX_DATA_PGN, TxMsg.byte_count, GET_PACKET_NUMBER(TxMsg.byte_count));
	if(Transmit_J1939msg(&TxMsg))
		print_string("CAN Frame SENT\r\n");
	else
		print_string("CAN Frame ERROR\r\n");
	HAL_Delay(500);
#else
	TL_process();
	TL_periodic();
#endif
}

#if 0
void App_Process(void)
{
#if TRANSMITTER
	HAL_Delay(1000);
	J1939_PDU.PGN.pgn 		= 60928;
	J1939_PDU.PGN.edp_dp	= 0;
	J1939_PDU.PGN.pf 		= 238;
	J1939_PDU.PGN.ps		= 20;
	J1939_PDU.dlc 			= 8;
	J1939_PDU.priority		= 7;
	J1939_PDU.sa			= 19;
	for(int i = 0; i<J1939_PDU.dlc; i++)
		J1939_PDU.data[i]	= i+2;
	if(PackFrame(&J1939_PDU) == J1939_ERROR)
		print_string("Can Transmission Error\r\n");
	else
	{
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_12);
		print_string("Can Data Transmitted\r\n");
	}
#else
	HAL_Delay(100);
	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_12);
	J1939_RTYPE ret;
	ret = RetrieveFrame(&J1939_PDU);
	if(ret == J1939_ERROR)
		print_string("Can Receive Error\r\n");
	else if(ret == J1939_TIMEOUT)
	{
		print_string("Timeout\r\n");
	}
	else
	{
		print_string("PDU PGN: %d\r\n",J1939_PDU.PGN.pgn);
		print_string("PDU EDP: %d, DP: %d\r\n",(J1939_PDU.PGN.edp_dp & EDP_MASK), (J1939_PDU.PGN.edp_dp & DP_MASK));
		print_string("PDU Format: %d\r\n",J1939_PDU.PGN.pf);
		print_string("PDU Specific: %d\r\n");
		print_string("PDU DLC: %d\r\n",J1939_PDU.dlc);
		print_string("PDU Priority: %d\r\n",J1939_PDU.priority);
		print_string("PDU Source Adress: %d\r\n", J1939_PDU.sa);
		print_string("PDU Data: ");
		for(int i = 0; i<J1939_PDU.dlc; i++)
			print_string("%d ", J1939_PDU.data[i]);
		print_string("\r\n\n-----------------------------------------------------\n\r\n");

	}
#endif
}
#endif
