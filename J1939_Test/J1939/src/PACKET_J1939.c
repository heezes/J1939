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
#include "gp_timer.h"

/*Private Variable*/
Queue CAN_QUEUE;
static u8 CAN_BUFFER[CAN_BUFFER_SIZE];

J1939_RTYPE Interface_Init(J19139_PduTypeDef* pkt)
{
	J1939_RTYPE ret = J1939_OK;
	pkt->PGN.edp_dp = 0; pkt->PGN.pf = 0; pkt->PGN.ps = 0; pkt->PGN.pgn = 0;
	pkt->dlc 		= 0;
	pkt->priority 	= 0;
	pkt->sa 		= 0;
	memset(pkt->data,0,sizeof(pkt->data));
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
	/*Enqueue*/
	u32 id;
	id  = (u32)(pkt->priority << 25) | ((pkt->PGN.pgn<<8) | pkt->sa);
	ret = CAN_Transmit(id, (u8*)&pkt->data, pkt->dlc);
	return ret;
}
/*
TODO: Add frame Identification 
*/
J1939_RTYPE RetrieveFrame(J19139_PduTypeDef* pkt)
{
	struct timer t;
	t.interval = 0;
	t.start 	= 0;
	u8 len		= 0;
	u32 id 		= 0;
	Timer_Set(&t, 10);
	while(!Timer_Expired(&t))
	{
		if(!isEmpty(&CAN_QUEUE))
		{
			if(len==0)
				pkt->dlc = *(u8*)dequeue(&CAN_QUEUE);
			else if((len>=1) && len<5)
				id |= (*(u8*)dequeue(&CAN_QUEUE))<<((len-1)*8);
			else if((len-FRAME_HEADER)==pkt->dlc)
				break;/*Frame Extracted*/
			else
				pkt->data[len-FRAME_HEADER] = *(u8*)dequeue(&CAN_QUEUE);
			len++;
		}
	}
	if((len-FRAME_HEADER) != pkt->dlc)
		return J1939_TIMEOUT;/*Timeout Error*/
	pkt->priority 	= (id >> 24) & PRIORITY_MASK;
	pkt->sa 		= (u8)((id >> 0) &  0xFF);
	pkt->PGN.pgn 	= (u8)(id >> 8) &  PGN_MASK;
	pkt->PGN.edp_dp = (u8)(pkt->PGN.pgn >> 16) & (EDP_MASK|DP_MASK);
	pkt->PGN.pf 	= (u8)(pkt->PGN.pgn >> 8) & (0xFF);
	pkt->PGN.ps 	= (u8)(pkt->PGN.pgn >> 0) & (0xFF);
	return J1939_OK;
}
