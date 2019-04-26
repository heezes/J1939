/*
This File is used in J1939 based communication

Queue Data Frame:
----------------------------------------------------------------------
|	|		|	    											     |
|DLC|EXT ID	|<-----------------------DATA--------------------------->|			
----------------------------------------------------------------------
 <1b><-4b--><------------------------8b------------------------------>
Extended Identity Frame: 29 Bits
---------------------------------------------------------------------
| P |  EDP  |  DP  |		PF		|		PS		|		SA		|
|3b |  1b   |  1b  |		8b		|		8b		|		8b		|
---------------------------------------------------------------------
*/
#include "PACKET_J1939.h"
#include "queue_aar.h"

/*Private Variable*/
Queue CAN_QUEUE;
static u8 CAN_BUFFER[CAN_BUFFER_SIZE];

J1939_RTYPE Interface_Init(void)
{
	J1939_RTYPE ret = J1939_OK;
	create_queue(&CAN_QUEUE,&CAN_BUFFER, sizeof(CAN_BUFFER)/sizeof(u8), CHAR);
	ret = CAN_Init();
	return ret;
}

J1939_RTYPE Interface_DeInit(J19139_PduTypeDef* pkt)
{
	return J1939_OK;
}
/*
TODO: Explore Possibility of Using Enqueue
*/
J1939_RTYPE PackFrame(J19139_PduTypeDef* pkt)
{
	J1939_RTYPE ret = J1939_OK;
	/*Calculate ID and add data to MAILBOX*/
	u32 id = 0;
	if(pkt->PGN.pf < 240)
		id	|= ((pkt->priority << 26) | ((pkt->PGN.pgn | pkt->PGN.ps)<<8)|pkt->sa);
	else
		id |= (u32)(pkt->priority << 26) | ((pkt->PGN.pgn<<8)|pkt->sa);
	ret = CAN_Transmit(id, (u8*)&pkt->data, pkt->dlc);
	return ret;
}
/*
TODO: Add frame Identification 
*/
J1939_RTYPE RetrieveFrame(J19139_PduTypeDef* pkt)
{
	u8 len		= 0;
	u32 id 		= 0;
	while(1)
	{
		if(!isEmpty(&CAN_QUEUE))
		{
			u8 temp;
			if(len==0)
			{
				temp = *(u8*)dequeue(&CAN_QUEUE);
				pkt->dlc = temp;
			}
			else if((len>=1) && len<5)
			{
				temp = (*(u8*)dequeue(&CAN_QUEUE));
				id 	 |= (temp<<((len-1)*8) & 0xFFFFFFFF);
			}
			else if((len-FRAME_HEADER)==pkt->dlc)
				break;/*Frame Extracted*/
			else
			{
				temp =	*(u8*)dequeue(&CAN_QUEUE);
				pkt->data[len-FRAME_HEADER] = temp;
			}
			len++;
		}
		else
			break;
	}
	if((len-FRAME_HEADER) != pkt->dlc)
		return J1939_TIMEOUT;/*Timeout Error*/
	pkt->priority 	= (id >> 26) & PRIORITY_MASK;
	pkt->sa 		= (u8)(id);
	pkt->PGN.pf 	= (u8)(id >> 16);
	if(pkt->PGN.pf > 240)
	{
		pkt->PGN.pgn 	= (PGN_T)((id >> 8) & 0x3FFFF);
		pkt->PGN.ps 	= (u8)(pkt->PGN.pgn >> 0);
	}
	else
	{
		pkt->PGN.pgn 	= (PGN_T)((id >> 8) & 0x3FF00);
		pkt->PGN.ps 	= (u8)(id >> 8);
	}
	pkt->PGN.edp_dp = (u8)(pkt->PGN.pgn >> 16) & (EDP_MASK|DP_MASK);
	pkt->PGN.pf 	= (u8)(pkt->PGN.pgn >> 8);
	return J1939_OK;
}
