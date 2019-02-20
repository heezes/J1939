#include "TP_J1939.h"

extern J1939_RX_MESSAGE_T J1939_rx_message;
extern J1939_TX_MESSAGE_T TxMsg;
static J1939_RX_STATE_MACHINE_T J1939_rx_state_machine;
static J1939_TX_STATE_MACHINE_T J1939_tx_state_machine;
static J1939_RX_PDU_T J1939_rx_pdu;

__weak void Can_DataCallback(J1939_RX_MESSAGE_T *msg)
{}

//==========================================================================================
// Transport Layer Interface Functions
//==========================================================================================
u8 GET_PACKET_NUMBER(u16 byte_count)
{
	u8 count = 0;
	while(byte_count > 7)
	{
		count++;
		byte_count -= 7;
	}
	return ++count;
}

J1939_RTYPE TL_init(void)
{
	J1939_rx_message.PGN = 0;
	J1939_rx_message.dest_addr = 0;
	J1939_rx_message.source_addr = 0;
	J1939_rx_message.byte_count = 0;

	J1939_rx_pdu.PGN = 0;
	J1939_rx_pdu.dest_addr = 0;
	J1939_rx_pdu.source_addr = 0;
	J1939_rx_pdu.byte_count = 0;

	J1939_rx_state_machine.PGN = 0;
	J1939_rx_state_machine.dest_addr = 0;
	J1939_rx_state_machine.source_addr = 0;
	J1939_rx_state_machine.packet_number = 0;
	J1939_rx_state_machine.timer_counter = 0;
	J1939_rx_state_machine.total_packet_number = 0;
	J1939_rx_state_machine.byte_count = 0;
	J1939_rx_state_machine.status = WAIT_FOR_MESSAGE;
	J1939_rx_state_machine.TP = TP_NONE;


	for (u8 i = 0; i < NUMBER_TRANS_RX_BUFFERS; i++)
	{
	  J1939_rx_message.data[i] = 0;
	}

	for (u8 i = 0; i < NUMBER_PDU_BUFFERS; i++)
	{
	  J1939_rx_pdu.data[i] = 0;
	}
	return Interface_Init();
}


void TL_process(void)
{
   J19139_PduTypeDef pdu;
   if(RetrieveFrame(&pdu) == J1939_OK)
   {
	   /*
	  print_string("\nDATA: ");
	  for(int i = 0; i<pdu.dlc; i++)
		print_string("%d ",pdu.data[i]);
	  print_string("\r\n");*/
      if ((pdu.PGN.pgn == TP_CM) || (pdu.PGN.pgn == TP_DT))
      {
         J1939_rx_pdu.PGN = pdu.PGN.pgn;
         J1939_rx_pdu.dest_addr = pdu.PGN.ps;
         J1939_rx_pdu.source_addr = pdu.sa;
         J1939_rx_pdu.byte_count = pdu.dlc;

         for (int i = 0; i < NUMBER_PDU_BUFFERS; i++)
         {
            J1939_rx_pdu.data[i] = pdu.data[i];
         }
      }
      else
      {
         J1939_RX_MESSAGE_T msg;
         msg.PGN = pdu.PGN.pgn;
         msg.dest_addr = pdu.PGN.ps;
         msg.source_addr = pdu.sa;
         msg.byte_count = pdu.dlc;

         for (int i = 0; i < NUMBER_PDU_BUFFERS; i++)
         {
            msg.data[i] = pdu.data[i];
         }
         // send message up the stack to the Network Management Layer
         Can_DataCallback(&msg);
      }
   }
}


void TL_periodic(void)
{
   u8 i;
   u8 go_on;

   go_on = TRUE;
   J1939_rx_state_machine.timer_counter++;

   while (go_on)
   {
      switch (J1939_rx_state_machine.status)
      {
         case  WAIT_FOR_MESSAGE:
            if ((J1939_rx_pdu.PGN == TP_CM) )//&& (J1939_rx_pdu.dest_addr == GLOBADDR || J1939_rx_pdu.dest_addr == NODEADDR))
            {
               if (J1939_rx_pdu.data[0] == TP_CM_RTS)
               {
            	   /*Last 8 bit not extracted because DA*/
                  J1939_rx_state_machine.PGN = J1939_rx_pdu.data[6];
                  J1939_rx_state_machine.PGN <<= 8;
                  J1939_rx_state_machine.PGN += J1939_rx_pdu.data[5];
                  J1939_rx_state_machine.dest_addr = J1939_rx_pdu.dest_addr;
                  J1939_rx_state_machine.source_addr = J1939_rx_pdu.source_addr;
                  J1939_rx_state_machine.packet_number = 0;
                  J1939_rx_state_machine.timer_counter = 0;
                  J1939_rx_state_machine.total_packet_number = J1939_rx_pdu.data[3];
                  J1939_rx_state_machine.byte_count = J1939_rx_pdu.data[2];
                  J1939_rx_state_machine.byte_count <<= 8;
                  J1939_rx_state_machine.byte_count += J1939_rx_pdu.data[1];
                  J1939_rx_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
                  J1939_rx_state_machine.TP = TP_CM_RTS;
               }
               else if (J1939_rx_pdu.data[0] == TP_CM_BAM)
               {
            	   /*Last 8 bit not extracted because DA*/
                  J1939_rx_state_machine.PGN = J1939_rx_pdu.data[6];
                  J1939_rx_state_machine.PGN <<= 8;
                  J1939_rx_state_machine.PGN += J1939_rx_pdu.data[5];
                  J1939_rx_state_machine.dest_addr = J1939_rx_pdu.dest_addr;
                  J1939_rx_state_machine.source_addr = J1939_rx_pdu.source_addr;
                  J1939_rx_state_machine.packet_number = 0;
                  J1939_rx_state_machine.timer_counter = 0;
                  J1939_rx_state_machine.total_packet_number = J1939_rx_pdu.data[3];
                  J1939_rx_state_machine.byte_count = J1939_rx_pdu.data[2];
                  J1939_rx_state_machine.byte_count <<= 8;
                  J1939_rx_state_machine.byte_count += J1939_rx_pdu.data[1];
                  J1939_rx_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
                  J1939_rx_state_machine.TP = TP_CM_BAM;
               }
               else if(J1939_rx_pdu.data[0] == TP_CM_CTS)
               {
            	   /*Last 8 bit not extracted because DA*/
                   J1939_rx_state_machine.PGN = J1939_rx_pdu.data[6];
                   J1939_rx_state_machine.PGN <<= 8;
                   J1939_rx_state_machine.PGN += J1939_rx_pdu.data[5];
                   J1939_rx_state_machine.dest_addr = J1939_rx_pdu.dest_addr;
                   J1939_rx_state_machine.source_addr = J1939_rx_pdu.source_addr;
                   J1939_rx_state_machine.cts_count	= J1939_rx_pdu.data[1];
                   J1939_rx_state_machine.packet_number = J1939_rx_pdu.data[2];
                   J1939_rx_state_machine.timer_counter = 0;

                   J1939_rx_state_machine.total_packet_number = GET_PACKET_NUMBER(TxMsg->byte_count);
                   J1939_rx_state_machine.byte_count = TxMsg->byte_count;

                   J1939_rx_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
                   J1939_rx_state_machine.TP = TP_CM_CTS;
               }
               else if (J1939_rx_pdu.data[0] == TP_CM_END_OF_MSG_ACK)
               {
            	   /*Last 8 bit not extracted because DA*/
                  J1939_rx_state_machine.PGN = J1939_rx_pdu.data[6];
                  J1939_rx_state_machine.PGN <<= 8;
                  J1939_rx_state_machine.PGN += J1939_rx_pdu.data[5];
                  J1939_rx_state_machine.dest_addr = J1939_rx_pdu.dest_addr;
                  J1939_rx_state_machine.source_addr = J1939_rx_pdu.source_addr;
                  J1939_rx_state_machine.packet_number = 0;
                  J1939_rx_state_machine.timer_counter = 0;
                  J1939_rx_state_machine.total_packet_number = J1939_rx_pdu.data[3];
                  J1939_rx_state_machine.byte_count = J1939_rx_pdu.data[2];
                  J1939_rx_state_machine.byte_count <<= 8;
                  J1939_rx_state_machine.byte_count += J1939_rx_pdu.data[1];
                  J1939_rx_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
                  J1939_rx_state_machine.TP = TP_CM_END_OF_MSG_ACK;
               }
               else
               {
                  go_on = FALSE;//break;
               }
            }
            else
            {
               go_on = FALSE;//break;
            }
         break;

         case INIT_REASSEMBLE_STRUCTURE:
            if (J1939_rx_state_machine.TP == TP_CM_RTS)
            {
            }
            else if(J1939_rx_state_machine.TP == TP_CM_CTS)
            {
            	if(J1939_rx_state_machine.cts_count)
            	{
            		J1939_rx_state_machine.status = SEND_DT;
            	}
            	else
            	{
            		J1939_rx_state_machine.status = CHECK_TIMER;
            	}
            }
            else if(J1939_rx_state_machine.TP = TP_CM_END_OF_MSG_ACK)
            {
            	J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
            }
            else if (J1939_rx_state_machine.TP == TP_CM_BAM)
            {
               if (J1939_rx_state_machine.byte_count < NUMBER_TRANS_RX_BUFFERS)
               {
                  J1939_rx_message.PGN = J1939_rx_state_machine.PGN;
                  J1939_rx_message.dest_addr = J1939_rx_state_machine.dest_addr;
                  J1939_rx_message.source_addr = J1939_rx_state_machine.source_addr;
                  J1939_rx_message.byte_count = J1939_rx_state_machine.byte_count;

                  for (i = 0; i < NUMBER_TRANS_RX_BUFFERS; i++)
                  {
                     J1939_rx_message.data[i] = 0;
                  }
                  J1939_rx_state_machine.status = WAIT_FOR_DATA;
               }
               else
               {
                  J1939_rx_state_machine.status= WAIT_FOR_MESSAGE;
                  J1939_rx_state_machine.timer_counter = 0;
                  go_on = FALSE;
                  //break;
               }
            }
            else
            {
               J1939_rx_state_machine.status = WAIT_FOR_MESSAGE;
               J1939_rx_state_machine.timer_counter = 0;
               go_on = FALSE;
               //break;
            }
         break;

         case CHECK_PACKET:
         break;

         case SEND_ABORT:
         break;

         case SEND_DT:
        	 if(J1939_rx_state_machine.cts_count--)
        	 {
        		Transmit_J1939msg(&TxMsg);
        	 }
        	 else
        	 {
        		 /* TODO : Change State and Update Timer*/
                 J1939_rx_state_machine.status = CHECK_TIMER;
                 Timer_Set(&J1939_rx_state_machine.t, 1250);
                 //J1939_rx_state_machine.timer_counter = 0;
                 go_on = FALSE;
        	 }
         break;

         case WAIT_FOR_DATA:
            if (J1939_rx_pdu.PGN == TP_DT)
            {
               J1939_rx_state_machine.status = CHECK_DATA_PACKET;
               //break;
            }
            else
            {
               J1939_rx_state_machine.status = CHECK_TIMER;
               //break;
            }
         break;

         case CHECK_TIMER:
/*            if (J1939_rx_state_machine.timer_counter > (750/TICK))   // 750ms
            {
               J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
               //J1939_rx_state_machine.timer_counter = 0;
               //break;
            }*/
            if(Timer_Expired(&J1939_rx_state_machine.t))
            {
                J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
            }
            else
            {
               J1939_rx_state_machine.status = WAIT_FOR_DATA;
               go_on = FALSE;//break;
            }
         break;

         case RESET_REASSEMBLY_STRUCTURE:
            J1939_rx_state_machine.PGN = 0;
            J1939_rx_state_machine.dest_addr = 0;
            J1939_rx_state_machine.source_addr = 0;
            J1939_rx_state_machine.packet_number = 0;
            J1939_rx_state_machine.timer_counter = 0;
            J1939_rx_state_machine.total_packet_number = 0;
            J1939_rx_state_machine.byte_count = 0;
            J1939_rx_message.PGN = 0;
            J1939_rx_message.dest_addr = 0;
            J1939_rx_message.source_addr = 0;
            J1939_rx_message.byte_count = 0;

            for (i = 0; i < NUMBER_TRANS_RX_BUFFERS; i++)
            {
               J1939_rx_message.data[i] = 0;
            }

            J1939_rx_state_machine.status = WAIT_FOR_MESSAGE;
            go_on = FALSE;
         break;

         case CHECK_DATA_PACKET:
            if (J1939_rx_state_machine.TP == TP_CM_BAM)
            {
               if (J1939_rx_pdu.source_addr == J1939_rx_state_machine.source_addr &&
                  J1939_rx_pdu.dest_addr == J1939_rx_state_machine.dest_addr)
               {
                  if (J1939_rx_state_machine.packet_number == J1939_rx_pdu.data[0]-1)
                  {
                     J1939_rx_state_machine.status = SAVE_DATA;
                     J1939_rx_state_machine.timer_counter = 0;
                     J1939_rx_pdu.data[0] = 0;
                  }
                  else if(J1939_rx_pdu.data[0] != 0)
                  {
                     J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
                  }
                  else
                  {
                     J1939_rx_state_machine.status = WAIT_FOR_DATA;
                     go_on = FALSE;
                  }
               }
               else
               {
                  J1939_rx_state_machine.status = WAIT_FOR_DATA;
                  go_on = FALSE;
               }
            }
            else if (J1939_rx_state_machine.TP == TP_CM_RTS)
            {}
            else
            {}
         break;

         case SAVE_DATA:
            i=0;

            while ((i < 7) && ((J1939_rx_state_machine.packet_number*7+i) <= J1939_rx_state_machine.byte_count))
            {
               J1939_rx_message.data[J1939_rx_state_machine.packet_number*7+i] = J1939_rx_pdu.data[i+1];
               J1939_rx_pdu.data[i+1] = 0;
               i++;
            }
            J1939_rx_state_machine.packet_number++;

            if (J1939_rx_state_machine.packet_number == J1939_rx_state_machine.total_packet_number)
            {
               J1939_rx_state_machine.status = FILL_USER_MESSAGE;
            }
            else
            {
               J1939_rx_state_machine.status = WAIT_FOR_DATA;
               go_on = FALSE;
            }
         break;

         case SEND_EOM:
        	 break;
         case SEND_CTS:

         case FILL_USER_MESSAGE:
            J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
            J1939_rx_state_machine.timer_counter = 0;

            // send message up the stack to the Network Management Layer
            Can_DataCallback(&J1939_rx_message);

            //go_on = FALSE;
         break;


         default:
            go_on = FALSE;
         break;

      } // end switch
   } // end while
}

void TP_tx_Process(void)
{
	   u8 go_on;
	   go_on = TRUE;
	   while(go_on)
	   {
		   switch(J1939_tx_state_machine.status)
		   {
		   case WAIT_FOR_MESSAGE:
			   if((J1939_rx_pdu.PGN == TP_CM) )//&& (J1939_rx_pdu.dest_addr == GLOBADDR || J1939_rx_pdu.dest_addr == NODEADDR))
			   {
				   if(J1939_rx_pdu.data[0] == TP_CM_CTS)
				   {
	                   J1939_tx_state_machine.cts_count	= J1939_rx_pdu.data[1];
	                   J1939_tx_state_machine.packet_number = J1939_rx_pdu.data[2];
	                   J1939_tx_state_machine.status = CHECK_CM;
				   }
				   else if((J1939_rx_pdu.data[0] == TP_CM_END_OF_MSG_ACK) ||(J1939_rx_pdu.data[0] == TP_CM_CONN_ABORT))
				   {
					   J1939_tx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
				   }
				   else
					   go_on = FALSE;
			   }
			   else
			   {
                   J1939_tx_state_machine.status = CHECK_TIMER;
			   }
			   break;
		   case CHECK_CM:
			   if(J1939_tx_state_machine.cts_count)
			   {
				  J1939_tx_state_machine.status = SEND_DT;
			   }
			   else
			   {
				 /*TODO SET TIMER
				  * Timeout Vaue Set to 1050 ms
				  * */
				Timer_Set(&J1939_tx_state_machine.t, CTS_WAIT_TIMEOUT);
				J1939_tx_state_machine.status = CHECK_TIMER;
				go_on = FALSE;
			   }
			   break;
		   case CHECK_TIMER:
			   if(Timer_Expired(&J1939_tx_state_machine.t))
			   {
				   J1939_tx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
				   Transmit_J1939_Abort(&TxMsg);
			   }
			   else
			   {
				   J1939_tx_state_machine.status = WAIT_FOR_MESSAGE;
				   go_on = False;
			   }
			   break;
		   case RESET_REASSEMBLY_STRUCTURE:
	           memset(TxMsg,0,sizeof(J1939_TX_MESSAGE_T));
			   memset(J1939_tx_state_machine,0,sizeof(J1939_TX_STATE_MACHINE_T));
			   J1939_tx_state_machine.status = WAIT_FOR_MESSAGE;
			   go_on = FALSE;
			   break;
		   case SEND_DT:
			   if(J1939_tx_state_machine.cts_count)
			   {
				   Transmit_J1939msg(&TxMsg);
				   J1939_rx_state_machine.packet_number++;
				   J1939_tx_state_machine.cts_count--;
				   //HAL_Delay(10);
			   }
			   else
			   {
				 /*SET Timer
				  * Timeout Value Set to 1250 ms.
				  * */
				  Set_Timer(&J1939_tx_state_machine.t, RECEIVE_RESP_TIMEOUT);
				  J1939_tx_state_machine.status = WAIT_FOR_MESSAGE;
			   }
			   go_on = FALSE;
			   break;
		   }
	   }
}

u8 Transmit_J1939msg(J1939_TX_MESSAGE_T *msg)
{
   if (msg->byte_count > 8)
   {
	   if(msg->status = TP_CM_BAM)
	   {
			J19139_PduTypeDef pdu;
			pdu.PGN.pgn 		= TP_DT;
			pdu.PGN.ps  		= msg->dest_addr;
			pdu.PGN.edp_dp		= 0;
			pdu.PGN.pf			= (u8)(pdu.PGN.pgn>>8);
			pdu.priority		= CM_PRIORITY;
			pdu.sa      		= NODEADDR;
			pdu.dlc     		= NUMBER_PDU_BUFFERS;
			u8 total_packets 	= GET_PACKET_NUMBER(msg->byte_count);
			for(int i = 0; i < total_packets; i++)
			{
				pdu.data[0]		= i+1;
				for(int j = 1; j < NUMBER_PDU_BUFFERS; j++)
				{
					pdu.data[j]	= msg->data[(j-1)+(7*i)];
				}
				if(PackFrame(&pdu) == J1939_ERROR)
					return FALSE;
				struct Timer t;
				Timer_Set(&t, 10);
				while(!Timer_Expired(&t));
			}
	   }
	   else
	   {
			J19139_PduTypeDef pdu;
			pdu.PGN.pgn 		= TP_DT;
			pdu.PGN.ps  		= msg->dest_addr;
			pdu.PGN.edp_dp		= 0;
			pdu.PGN.pf			= (u8)(pdu.PGN.pgn>>8);
			pdu.priority		= CM_PRIORITY;
			pdu.sa      		= NODEADDR;
			pdu.dlc     		= NUMBER_PDU_BUFFERS;
			pdu.data[0]			= J1939_rx_state_machine.packet_number;
			for(int i = 1; i < (NUMBER_PDU_BUFFERS-1); i++)
			{
				pdu.data[i]	= msg->data[J1939_rx_state_machine.packet_number*7+i-1];
			}
			if(PackFrame(&pdu) == J1939_ERROR)
				return FALSE;
			struct Timer t;
			Timer_Set(&t, 10);
			while(!Timer_Expired(&t));
	   }
   }
   else
   {
      J19139_PduTypeDef pdu;
      pdu.PGN.pgn 		= msg->PGN;
      pdu.PGN.ps  		= msg->dest_addr;
      pdu.PGN.edp_dp	= 0;
      pdu.PGN.pf		= (u8)(pdu.PGN.pgn>>8);
      pdu.priority		= LOWEST_PRIORITY;
      pdu.sa      		= NODEADDR;
      pdu.dlc     		= msg->byte_count;
      for (int i = 0; i < NUMBER_PDU_BUFFERS; i++)
      {
         pdu.data[i] = msg->data[i];
      }
      if(PackFrame(&pdu) == J1939_ERROR)
         return FALSE;
   }
   return TRUE;
}


u8 Transmit_J1939_BAM(J1939_TX_MESSAGE_T *msg)
{
    J19139_PduTypeDef pdu;
    pdu.PGN.pgn 		= TP_CM;
    pdu.PGN.ps  		= GLOBADDR;
    pdu.PGN.edp_dp		= 0;
    pdu.PGN.pf			= (u8)(pdu.PGN.pgn>>8);
    pdu.priority		= CM_PRIORITY;
    pdu.sa      		= NODEADDR;
    pdu.dlc     		= NUMBER_PDU_BUFFERS;

	/* initial BAM data */
    pdu.data[0] = TP_CM_BAM;
    pdu.data[1] = (uint8_t) msg->byte_count ;
    pdu.data[2] = (uint8_t) ((msg->byte_count)>>8);
    pdu.data[3] = GET_PACKET_NUMBER(msg->byte_count);
    pdu.data[4] = RESERVED_BYTE;
    pdu.data[7] = (uint8_t)((msg->PGN>>16) & 0xff);  /* MSB */
    pdu.data[6] = (uint8_t)(msg->PGN>>8 & 0xff);
    /*PDU Format < 240*/
    pdu.data[5] = (uint8_t)(msg->PGN & 0xff);   /* LSB */

    if(PackFrame(&pdu) == J1939_ERROR)
       return FALSE;
    return Transmit_J1939msg(msg);
}

u8 Transmit_J1939_RTS(J1939_TX_MESSAGE_T *msg)
{
    J19139_PduTypeDef pdu;
    pdu.PGN.pgn 		= TP_CM;
    pdu.PGN.ps  		= GLOBADDR;
    pdu.PGN.edp_dp		= 0;
    pdu.PGN.pf			= (u8)(pdu.PGN.pgn>>8);
    pdu.priority		= CM_PRIORITY;
    pdu.sa      		= NODEADDR;
    pdu.dlc     		= NUMBER_PDU_BUFFERS;

	/* initial RTS data */
    pdu.data[0] = TP_CM_RTS;
    pdu.data[1] = (uint8_t) msg->byte_count ;
    pdu.data[2] = (uint8_t) ((msg->byte_count)>>8);
    pdu.data[3] = GET_PACKET_NUMBER(msg->byte_count);
    pdu.data[4] = TRANSPORT_PACKET_COUNT;
    pdu.data[7] = (uint8_t)((msg->PGN>>16) & 0xff);  /* MSB */
    pdu.data[6] = (uint8_t)(msg->PGN>>8 & 0xff);
    /*PDU Format < 240*/
    pdu.data[5] = (uint8_t)(msg->PGN & 0x00);   /* LSB */

    if(PackFrame(&pdu) == J1939_ERROR)
       return FALSE;
    Timer_Set(&J1939_rx_state_machine.t,RECEIVE_RESP_TIMEOUT);
    //J1939_
    return TRUE;
}
