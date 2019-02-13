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
					for(int i = 0; i<NUMBER_PDU_BUFFERS; i++)
						StateMachine->buffer[i] 		= 	pdu.data[i];
					StateMachine->source_addr			=	pdu.sa;
					StateMachine->dest_addr				=	pdu.PGN.ps;
					if((pdu.PGN.pgn == (TP_CM_PGN)) && (StateMachine->source_addr == GLOBADDR|| StateMachine->dest_addr == NODEADDR))
					{

						StateMachine->t->interval			=   0;
						StateMachine->t->start				= 	0;
						if(!StateMachine->mode)
						{
							StateMachine->cts_count				=	0;
							StateMachine->packet_number			= 	0;
							StateMachine->PGN 					=	((pdu.data[6]<<8) | pdu.data[5]);
							StateMachine->byte_count 		  	= 	((StateMachine->buffer[2]<<8)|StateMachine->buffer[1]);
							StateMachine->total_packet_number 	= 	StateMachine->buffer[3];
							StateMachine.mode 					=   (StateMachine->buffer[0]);
							StateMachine->state					= 	CHECK_PACKET;
						}
						else
						{
							if(StateMachine->packet_number == StateMachine->buffer[0] -1)
							{
								StateMachine->packet_number			= 	StateMachine->buffer[0];
								StateMachine->state					= 	CHECK_TIMER;
							}
							else{
					    		memset(StateMachine, 0, sizeof(J1939_StateMachine));
					    		StateMachine->state = RECEIVE_PACKET;
					    		loop = FALSE;
							}
							StateMachine->mode					= 	TP_DT;
						}
					}
					else
					{
						/*Implement Callback*/
					}
				}
				else 
				{
					StateMachine->state = RECEIVE_PACKET;
					loop = FALSE;
				}
				break;
			}
			case CHECK_PACKET:
			{
				if(StateMachine->mode == TP_CM_RTS)
				{/*TODO:*/}
				else if(StateMachine->mode == TP_CM_CTS)
				{/*TODO:*/}
				else if(StateMachine->mode == TP_CM_EOM)
				{/*TODO:*/}
			    else if(StateMachine->mode == TP_CM_BAM)
			    {
			    	if(StateMachine->byte_count < MAX_PACKET_SIZE)
			    	{
			    		Timer_Set(StateMachine->t, MAX_BAM_TIMEOUT);
			    		StateMachine->state	= CHECK_TIMER;
			    	}
			    	else
			    	{
			    		memset(StateMachine, 0, sizeof(J1939_StateMachine));
			    		StateMachine->state = RECEIVE_PACKET;
			    	}
			    }
				else{
					/*TODO:
					 * Handle Transport Protocol Abort Here*/
				}
	    		loop = FALSE;
				break;
			}
			case WAIT_FOR_DATA:
			{
				if(StateMachine->mode == TP_CM_RTS)
				{/*TODO:*/}
			    else if(StateMachine->mode == TP_DT)
			    {
			    	StateMachine->state	 = CHECK_EOM;
			    }
				else{/*TODO:*/}
				break;
			}
			case CHECK_TIMER:
			{
				if(!Timer_Expired(StateMachine->t))
				{
					StateMachine->state	= SAVE_DATA;
				}
				else{
					memset(StateMachine, 0, sizeof(StateMachine));
					StateMachine->state	=	RECIEVE_PACKET;
				}
				loop = FALSE;
				break;
			}
			case RESET_STATE:
			{}
			case SAVE_DATA:
			{
				int i = 0;
				while((i < 7) && ((StateMachine->packet_number*7+i) <= StateMachine.byte_count))
				{
				   callback(&StateMachine->buffer[i+1],i);
				   //J1939_rx_message.data[J1939_state_machine.packet_number*7+i] = StateMachine.data[i+1];
				   StateMachine->buffer[i+1] = 0;
				   i++;
				}
				J1939_state_machine.packet_number++;
			}
			case CHECK_EOM:
			{
				if(StateMachine->packet_number == StateMachine->total_packet_number)
				{
					StateMachine =
				}
				else
				{

				}
				break;
			}
		}
	}
}
