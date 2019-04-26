/*
 * Service_J1939.c
 *
 *  Created on: 24-Apr-2019
 *      Author: Altamash Abdul Rahim
 */

#include "Service_J1939.h"

uint8_t RequestDeviceSync(CanHandleTypeDef *can)
{
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
}

uint8_t AuthenticatePeer(CanHandleTypeDef *can)
{

}

