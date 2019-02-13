#include "StateMachine.h"

void TP_StateMachine_Run(J1939_StateMachine* StateMachine)
{
	u8 loop = 1;
	while(loop)
	{
		switch(StateMachine->state)
		{
			default: break;
			case RECEIVE_PACKET:
			{
				J19139_PduTypeDef pdu;
				if(RetrieveFrame(&pdu) == J1939_OK)
				{
					if(pdu.PGN.pgn == (TP_CM_PGN))
					{
						StateMachine.PGN 			=	pdu.PGN.pgn;
						StateMachine.source_addr	=	pdu.sa;
						StateMachine.dest_addr		=	pdu.PGN.ps;
						StateMachine.mode 			=   
						for(int i = 0; i<NUMBER_PDU_BUFFERS; i++)
							StateMachine.data[i] = pdu.data[i];
					}
					else
					{

					}
				}
				else 
					StateMachine->state = RECEIVE_PACKET;
				loop = 0;
				break;
			}
			case CHECK_PACKET:
			{}
			case WAIT_FOR_DATA:
			{}
			case CHECK_TIMER:
			{}
			case RESET_STATE:
			{}
			case CHECK_SEQUENCE:
			{}
			case SAVE_DATA:
			{

			}
		}
	}
}