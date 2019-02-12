/*
 * App_J1939.c
 *
 *  Created on: 09-Feb-2019
 *      Author: Altamash Abdul Rahim
 */
#include "App_J1939.h"
#include "stdio.h"

J19139_PduTypeDef J1939_PDU;

#define TRANSMITTER	0

void App_Init(void)
{
	if(Interface_Init(&J1939_PDU) == J1939_OK){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_SET);
		print_string("Interface Initialized\r\n");
	}
	else
		print_string("Interface Initialization Error\r\n");
}

void App_Process(void)
{
#if TRANSMITTER
	HAL_Delay(1000);
	J1939_PDU.PGN.pgn 		= 59392;
	J1939_PDU.PGN.edp_dp	= 0;
	J1939_PDU.PGN.pf 		= 220;
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
