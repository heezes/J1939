/*
 * App_J1939.c
 *
 *  Created on: 09-Feb-2019
 *      Author: Altamash Abdul Rahim
 */
#include "App_J1939.h"
#include "stdio.h"
#include "string.h"

unsigned int tick = 0, lastTick = 0;
uint8_t blinkLed = 0;

#define TRANSMITTER	0

__IO J1939_RX_MESSAGE_T J1939_rx_message = {0};
__IO J1939_TX_MESSAGE_T TxMsg = {0};
__IO static J1939_RX_STATE_MACHINE_T J1939_rx_state_machine = {0};
__IO static J1939_TX_STATE_MACHINE_T J1939_tx_state_machine = {0};
__IO static J1939_RX_PDU_T J1939_rx_pdu = {0};

void Can_DataCallback(J1939_RX_MESSAGE_T *msg)
{
	print_string("PGN: %d\r\n", msg->PGN);
	print_string("Data Size: %d Byte\r\n", msg->byte_count);
	print_string("Source Address: %d\r\n", msg->source_addr);
	print_string("Destination Address: %d\r\n", msg->dest_addr);
	print_string("Data Recevived\r\n");
	for(int i = 0; i< msg->byte_count; i++)
		print_string("%d ",msg->data[i]);
	print_string("\r\n");
	if(msg->PGN == 60928)
	{
#if !TRANSMITTER
		J19139_PduTypeDef pdu;
		memset(&pdu, 0, sizeof(J19139_PduTypeDef));
		pdu.PGN.pgn 		= msg->PGN;
		pdu.PGN.ps  		= 0x30;
		pdu.PGN.edp_dp		= 0;
		pdu.PGN.pf			= (u8)(pdu.PGN.pgn>>8);
		pdu.priority		= LOWEST_PRIORITY;
		pdu.sa      		= NODEADDR;
		pdu.dlc     		= msg->byte_count;
		tick   				= HAL_GetTick();
		for (int i = 0; i < NUMBER_PDU_BUFFERS-4; i++)
		{
			pdu.data[i] = (uint8_t)(tick>>(i*8));
		}
		if(PackFrame(&pdu) == J1939_ERROR)
			print_string("Sync Ack Send Error\n");
#else
		tick = ((msg->data[3]<<24) & 0xFFFFFFFF) | ((msg->data[2]<<16) & 0xFFFFFF) | ((msg->data[1]<<8) & 0xFFFF) | ((msg->data[0]<<0) & 0xFF);
#endif
	    lastTick = tick;
		blinkLed = 1;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	TL_process((J1939_RX_PDU_T *)&J1939_rx_pdu);
	TP_tx_Process((J1939_TX_STATE_MACHINE_T *)&J1939_tx_state_machine,(J1939_RX_PDU_T *)&J1939_rx_pdu,(J1939_TX_MESSAGE_T*)&TxMsg);
	TL_periodic((J1939_RX_STATE_MACHINE_T *)&J1939_rx_state_machine, (J1939_RX_PDU_T *)&J1939_rx_pdu, (J1939_RX_MESSAGE_T*)&J1939_rx_message);
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
	J19139_PduTypeDef pdu;
	pdu.PGN.pgn 		= 60928;
	pdu.PGN.ps  		= 0x30;
	pdu.PGN.edp_dp		= 0;
	pdu.PGN.pf			= (u8)(pdu.PGN.pgn>>8);
	pdu.priority		= LOWEST_PRIORITY;
	pdu.sa      		= NODEADDR;
	pdu.dlc     		= 8;
	for (int i = 0; i < NUMBER_PDU_BUFFERS; i++)
	{
		pdu.data[i] = i;
	}
	if(PackFrame(&pdu) == J1939_ERROR)
		print_string("Sync Ack Send Error\n");
#if 0
	memset(&TxMsg, 0, sizeof(J1939_TX_MESSAGE_T));
	TxMsg.PGN 			= 60928;
	u8 count			= 50;
	TxMsg.byte_count	= count;
	TxMsg.dest_addr		= GLOBADDR;
	TxMsg.priority		= CM_PRIORITY;
	for(int i = 0; i < TxMsg.byte_count; i++)
		TxMsg.data[i]	= i;
	if(Transmit_J1939_RTS(&TxMsg,&J1939_tx_state_machine) == TRUE)
		print_string("CAN Frame SENT\r\n");
	else
		print_string("CAN Frame ERROR\r\n");
#endif
#endif
}

#define TX_DATA_PGN		60928

void App_Process(void)
{
	uint32_t currentTick = 0;
	currentTick = HAL_GetTick();
#if TRANSMITTER
	currentTick += tick;
#endif
	if(blinkLed)
	{
		if(currentTick - lastTick >= 2000)
		{
			print_string("Tick %u\n", (unsigned int)currentTick);
			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_12);
			lastTick = currentTick;
		}
	}
}
