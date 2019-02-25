#include "StateMachine.h"
#include "string.h"

extern J1939_RX_MESSAGE_T J1939_rx_message;
extern J1939_TX_MESSAGE_T TxMsg;
static J1939_RX_STATE_MACHINE_T J1939_rx_state_machine;
static J1939_TX_STATE_MACHINE_T J1939_tx_state_machine;
static J1939_RX_PDU_T J1939_rx_pdu;

/**/
static u8 Transmit_J1939_CTS(J1939_RX_STATE_MACHINE_T* RxStateMachine);
static u8 Transmit_J1939_Abort(PGN_T pgn, u8 reason, u8 DestAddr);
static u8 Transmit_J1939_EOM(J1939_RX_STATE_MACHINE_T* RxStateMachine);

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
	//J1939_rx_state_machine.timer_counter = 0;
	Timer_Reset(&J1939_rx_state_machine.t);
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

	  print_string("\nDATA: ");
	  for(int i = 0; i<pdu.dlc; i++)
		print_string("%d ",pdu.data[i]);
	  print_string("\r\n");
      print_string("PGN: %d\r\n",pdu.PGN.pgn);
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
   else
	   return;
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
               print_string("WAIT_FOR_MESSAGE\r\n");
               if (J1939_rx_pdu.data[0] == TP_CM_RTS)
               {
            	   /*Last 8 bit not extracted because DA*/
                  J1939_rx_state_machine.PGN = J1939_rx_pdu.data[6];
                  J1939_rx_state_machine.PGN <<= 8;
                  J1939_rx_state_machine.PGN += J1939_rx_pdu.data[5];
                  J1939_rx_state_machine.dest_addr = J1939_rx_pdu.dest_addr;
                  J1939_rx_state_machine.source_addr = J1939_rx_pdu.source_addr;
                  J1939_rx_state_machine.packet_number = 0;
                  //J1939_rx_state_machine.timer_counter = 0;
                  Timer_Reset(&J1939_rx_state_machine.t);
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
                  //J1939_rx_state_machine.timer_counter = 0;
                  Timer_Reset(&J1939_rx_state_machine.t);
                  J1939_rx_state_machine.total_packet_number = J1939_rx_pdu.data[3];
                  J1939_rx_state_machine.byte_count = J1939_rx_pdu.data[2];
                  J1939_rx_state_machine.byte_count <<= 8;
                  J1939_rx_state_machine.byte_count += J1939_rx_pdu.data[1];
                  J1939_rx_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
                  J1939_rx_state_machine.TP = TP_CM_BAM;
               }
               else if(J1939_rx_pdu.data[0] == TP_CM_CONN_ABORT)
               {
            	   /*Implement Abort*/
            	   print_string("Abort the Connection\r\n");
            	   J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
            	   /*Last 8 bit not extracted because DA
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
                   */
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
         	print_string("INIT_REASSEMBLE_STRUCTURE\r\n");
            if (J1939_rx_state_machine.TP == TP_CM_RTS)
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
                    J1939_rx_state_machine.status = SEND_CTS;
                }
                else
                {
                    J1939_rx_state_machine.status= WAIT_FOR_MESSAGE;
                    Timer_Reset(&J1939_rx_state_machine.t);
//                    J1939_rx_state_machine.timer_counter = 0;
                    go_on = FALSE;

                }
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
                  Timer_Set(&J1939_rx_state_machine.t,RECEIVE_RESP_TIMEOUT);
               }
               else
               {
                  J1939_rx_state_machine.status= WAIT_FOR_MESSAGE;
                  Timer_Reset(&J1939_rx_state_machine.t);
                  //J1939_rx_state_machine.timer_counter = 0;
                  go_on = FALSE;
                  //break;
               }
            }
            else
            {
               J1939_rx_state_machine.status = WAIT_FOR_MESSAGE;
               //J1939_rx_state_machine.timer_counter = 0;
               Timer_Reset(&J1939_rx_state_machine.t);
               go_on = FALSE;
               //break;
            }
         break;

         case CHECK_PACKET:
         break;

         case SEND_ABORT:
         break;

         case WAIT_FOR_DATA:
         	print_string("WAIT_FOR_DATA\r\n");
            if (J1939_rx_pdu.PGN == TP_DT)
            {
               J1939_rx_pdu.PGN = 0;
               Timer_Reset(&J1939_rx_state_machine.t);
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
         	print_string("CHECK_TIMER\r\n");
/*            if (J1939_rx_state_machine.timer_counter > (750/TICK))   // 750ms
            {
               J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
               //J1939_rx_state_machine.timer_counter = 0;
               //break;
            }*/
            if(Timer_Expired(&J1939_rx_state_machine.t))
            {
            	print_string("Timer Expired\r\n");
                J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
            }
            else
            {
               J1939_rx_state_machine.status = WAIT_FOR_DATA;
               go_on = FALSE;//break;
            }
         break;

         case RESET_REASSEMBLY_STRUCTURE:
         	print_string("RESET_REASSEMBLY_STRUCTURE\r\n");
   		    memset(&J1939_rx_pdu, 0, sizeof(J1939_RX_PDU_T));
   		    memset(&J1939_rx_state_machine, 0,sizeof(J1939_RX_STATE_MACHINE_T));
   		    memset(&J1939_rx_message, 0, sizeof(J1939_RX_MESSAGE_T));
            Timer_Reset(&J1939_rx_state_machine.t);
            J1939_rx_state_machine.status = WAIT_FOR_MESSAGE;
            go_on = FALSE;
         break;

         case CHECK_DATA_PACKET:
         	print_string("CHECK_DATA_PACKET\r\n");
         	for(int ijk = 0; ijk<8; ijk++)
         		print_string("%d ",J1939_rx_pdu.data[ijk]);
         	print_string("\r\n");
               if (J1939_rx_pdu.source_addr == J1939_rx_state_machine.source_addr &&
                  J1939_rx_pdu.dest_addr == J1939_rx_state_machine.dest_addr)
               {
                  if (J1939_rx_state_machine.packet_number == J1939_rx_pdu.data[0]-1)
                  {
                     J1939_rx_state_machine.status = SAVE_DATA;
                     //J1939_rx_state_machine.timer_counter = 0;
                     Timer_Reset(&J1939_rx_state_machine.t);
                     J1939_rx_pdu.data[0] = 0;
                  }
                  else if(J1939_rx_pdu.data[0] != 0)
                  {
                     J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
                  }
                  else
                  {
                	  //print_string("%d\r\n",J1939_rx_pdu.data[0]);
                     J1939_rx_state_machine.status = WAIT_FOR_DATA;
                     go_on = FALSE;
                  }
               }
               else
               {
            	   //print_string("Not Accepted\r\n");
                  J1939_rx_state_machine.status = WAIT_FOR_DATA;
                  go_on = FALSE;
               }
         break;

         case SAVE_DATA:
         	print_string("SAVE_DATA\r\n");
            i=0;

            while ((i < 7) && ((J1939_rx_state_machine.packet_number*7+i) <= J1939_rx_state_machine.byte_count))
            {
               J1939_rx_message.data[J1939_rx_state_machine.packet_number*7+i] = J1939_rx_pdu.data[i+1];
               J1939_rx_pdu.data[i+1] = 0;
               i++;
            }
            J1939_rx_state_machine.packet_number++;
            Timer_Set(&J1939_rx_state_machine.t, SEND_RESP_TIMEOUT);
            if (J1939_rx_state_machine.packet_number == J1939_rx_state_machine.total_packet_number)
            {
                if(J1939_rx_state_machine.TP == TP_CM_RTS)
                {
                	J1939_rx_state_machine.status = SEND_EOM;
                }
                else
                	J1939_rx_state_machine.status = FILL_USER_MESSAGE;
            }
            else if((J1939_rx_state_machine.packet_number) == J1939_rx_state_machine.cts_count*TRANSPORT_PACKET_COUNT)
            {
            	J1939_rx_state_machine.status = SEND_CTS;
            }
            else
            {
               J1939_rx_state_machine.status = WAIT_FOR_DATA;
               go_on = FALSE;
            }
         break;

         case SEND_EOM:
         	 print_string("SEND_EOM\r\n");
        	 Transmit_J1939_EOM(&J1939_rx_state_machine);
        	 J1939_rx_state_machine.status = FILL_USER_MESSAGE;
        	 break;
         case SEND_CTS:
         	print_string("SEND_CTS\r\n");
        	 Transmit_J1939_CTS(&J1939_rx_state_machine);
        	 //Start Timer
        	 Timer_Set(&J1939_rx_state_machine.t, RECEIVE_RESP_TIMEOUT);
        	 J1939_rx_state_machine.cts_count++;
        	 J1939_rx_state_machine.status = CHECK_TIMER;
        	 //go_on = FALSE;
        	 break;

         case FILL_USER_MESSAGE:
         	print_string("FILL_USER_MESSAGE\r\n");
            J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
            //J1939_rx_state_machine.timer_counter = 0;
            Timer_Reset(&J1939_rx_state_machine.t);
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
		   case TX_WAIT_FOR_MESSAGE:
			   print_string("TX_WAIT_FOR_MESSAGE\r\n");
			   if((J1939_rx_pdu.PGN == TP_CM) || TxMsg.status == TP_CM_RTS)//&& (J1939_rx_pdu.dest_addr == GLOBADDR || J1939_rx_pdu.dest_addr == NODEADDR))
			   {
				   if(J1939_rx_pdu.data[0] == TP_CM_CTS)
				   {
					   print_string("CTS Received\r\n");
					   J1939_rx_pdu.PGN		= 0;
					   J1939_rx_pdu.data[0] 	= 0;
	                   J1939_tx_state_machine.cts_count	= J1939_rx_pdu.data[1];
	                   J1939_tx_state_machine.packet_number = J1939_rx_pdu.data[2];
	                   J1939_tx_state_machine.status = TX_CHECK_CM;
	                   TxMsg.status = TP_CM_CTS;
				   }
				   else if((J1939_rx_pdu.data[0] == TP_CM_END_OF_MSG_ACK) ||(J1939_rx_pdu.data[0] == TP_CM_CONN_ABORT))
				   {
					   print_string("%d Received\r\n", J1939_rx_pdu.data[0]);
					   J1939_tx_state_machine.status = TX_RESET_REASSEMBLY_STRUCTURE;
				   }
				   else
				   {
	                   J1939_tx_state_machine.status = TX_CHECK_TIMER;
				   }
			   }
			   else
			   {
                   J1939_tx_state_machine.status = TX_CHECK_TIMER;
				   go_on = FALSE;
			   }
			   break;
		   case TX_CHECK_CM:
			   print_string("TX_CHECK_CM\r\n");
			   if(J1939_tx_state_machine.cts_count)
			   {
				  J1939_tx_state_machine.status = TX_SEND_DT;
			   }
			   else
			   {
				 /*TODO SET TIMER
				  * Timeout Vaue Set to 1050 ms
				  * */
				   print_string("CTS with 0 Count\r\n");
				Timer_Set(&J1939_tx_state_machine.t, CTS_WAIT_TIMEOUT);
				J1939_tx_state_machine.status = TX_CHECK_TIMER;
				go_on = FALSE;
			   }
			   break;
		   case TX_CHECK_TIMER:
			   print_string("TX_CHECK_TIMER\r\n");
			   if(Timer_Expired(&J1939_tx_state_machine.t))
			   {
				   print_string("Timer Expired\r\n");
				   Timer_Reset(&J1939_tx_state_machine.t);
				   if(TxMsg.status == TP_CM_RTS)
				   {
					   J1939_tx_state_machine.status = TX_RESET_REASSEMBLY_STRUCTURE;
					   Transmit_J1939_Abort(TxMsg.PGN, ABORT_TIMEOUT, TxMsg.dest_addr);
					   TxMsg.status = TP_NONE;
				   }
			   }
			   else
				   J1939_tx_state_machine.status = TX_WAIT_FOR_MESSAGE;
			   go_on = FALSE;
			   break;
		   case TX_RESET_REASSEMBLY_STRUCTURE:
			   print_string("TX_RESET_REASSEMBLY_STRUCTURE\r\n");
	           memset(&TxMsg,0,sizeof(J1939_TX_MESSAGE_T));
			   memset(&J1939_tx_state_machine,0,sizeof(J1939_TX_STATE_MACHINE_T));
			   memset(&J1939_rx_pdu, 0, sizeof(J1939_RX_PDU_T));
			   J1939_tx_state_machine.status = TX_WAIT_FOR_MESSAGE;
			   go_on = FALSE;
			   break;
		   case TX_SEND_DT:
			   if(J1939_tx_state_machine.cts_count && ((TxMsg.byte_count+6) >= J1939_tx_state_machine.packet_number*7))
			   {
				   print_string("TX_SEND_DT\r\n");
				   Transmit_J1939msg(&TxMsg);
				   J1939_tx_state_machine.packet_number++;
				   J1939_tx_state_machine.cts_count--;
				   go_on = FALSE;
			   }
			   else
			   {
				 /*SET Timer
				  * Timeout Value Set to 1250 ms.
				  * */
				   print_string("Waiting for CTS\r\n");
				   Timer_Set(&J1939_tx_state_machine.t, RECEIVE_RESP_TIMEOUT);
				   J1939_tx_state_machine.status = TX_WAIT_FOR_MESSAGE;
			   }
			   break;
		   }
	   }
}

u8 Transmit_J1939msg(J1939_TX_MESSAGE_T *msg)
{
   if (msg->byte_count > 8)
   {
	   if(msg->status == TP_CM_BAM)
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
				struct timer t;
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
			pdu.data[0]			= J1939_tx_state_machine.packet_number;
			for(int i = 1; i < (NUMBER_PDU_BUFFERS); i++)
			{
				pdu.data[i]	= msg->data[(J1939_tx_state_machine.packet_number-1)*7+i-1];
				print_string("%d ", pdu.data[i]);
			}
			print_string("\r\n");
			if(PackFrame(&pdu) == J1939_ERROR)
				return FALSE;
			struct timer t;
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
    pdu.PGN.ps  		= msg->dest_addr;
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
    msg->status = TP_CM_RTS;
    Timer_Set(&J1939_tx_state_machine.t,RECEIVE_RESP_TIMEOUT);
    //J1939_
    return TRUE;
}

static u8 Transmit_J1939_CTS(J1939_RX_STATE_MACHINE_T* RxStateMachine)
{
    J19139_PduTypeDef pdu;
    pdu.PGN.pgn 		= TP_CM;
    pdu.PGN.ps  		= RxStateMachine->dest_addr;
    pdu.PGN.edp_dp		= 0;
    pdu.PGN.pf			= (u8)(pdu.PGN.pgn>>8);
    pdu.priority		= CM_PRIORITY;
    pdu.sa      		= NODEADDR;
    pdu.dlc     		= NUMBER_PDU_BUFFERS;

	/* initial RTS data */
    pdu.data[0] = TP_CM_CTS;
    pdu.data[1] = (uint8_t) TRANSPORT_PACKET_COUNT;
    pdu.data[2] = (uint8_t) RxStateMachine->packet_number+1;
    pdu.data[3] = RESERVED_BYTE;
    pdu.data[4] = RESERVED_BYTE;
    pdu.data[7] = (uint8_t)((RxStateMachine->PGN>>16) & 0xff);  /* MSB */
    pdu.data[6] = (uint8_t)(RxStateMachine->PGN>>8 & 0xff);
    /*PDU Format < 240*/
    pdu.data[5] = (uint8_t)(RxStateMachine->PGN & 0x00);   /* LSB */

    if(PackFrame(&pdu) == J1939_ERROR)
       return FALSE;
    Timer_Set(&J1939_rx_state_machine.t,RECEIVE_RESP_TIMEOUT);
    return TRUE;

}

static u8 Transmit_J1939_Abort(PGN_T pgn, u8 reason, u8 DestAddr)
{
    J19139_PduTypeDef pdu;
    pdu.PGN.pgn 		= TP_CM;
    pdu.PGN.ps  		= DestAddr;
    pdu.PGN.edp_dp		= 0;
    pdu.PGN.pf			= (u8)(pdu.PGN.pgn>>8);
    pdu.priority		= CM_PRIORITY;
    pdu.sa      		= NODEADDR;
    pdu.dlc     		= NUMBER_PDU_BUFFERS;

	/* initial RTS data */
    pdu.data[0] = TP_CM_CONN_ABORT;
    pdu.data[1] = (uint8_t) reason;
    pdu.data[2] = RESERVED_BYTE;
    pdu.data[3] = RESERVED_BYTE;
    pdu.data[4] = RESERVED_BYTE;
    pdu.data[7] = (uint8_t)((pgn>>16) & 0xff);  /* MSB */
    pdu.data[6] = (uint8_t)(pgn>>8 & 0xff);
    /*PDU Format < 240*/
    pdu.data[5] = (uint8_t)(pgn & 0x00);   /* LSB */

    if(PackFrame(&pdu) == J1939_ERROR)
       return FALSE;
    return TRUE;
}

static u8 Transmit_J1939_EOM(J1939_RX_STATE_MACHINE_T* RxStateMachine)
{
    J19139_PduTypeDef pdu;
    pdu.PGN.pgn 		= TP_CM;
    pdu.PGN.ps  		= RxStateMachine->dest_addr;
    pdu.PGN.edp_dp		= 0;
    pdu.PGN.pf			= (u8)(pdu.PGN.pgn>>8);
    pdu.priority		= CM_PRIORITY;
    pdu.sa      		= NODEADDR;
    pdu.dlc     		= NUMBER_PDU_BUFFERS;

	/* initial RTS data */
    pdu.data[0] = TP_CM_END_OF_MSG_ACK;
    pdu.data[1] = (uint8_t) RxStateMachine->byte_count ;
    pdu.data[2] = (uint8_t) ((RxStateMachine->byte_count)>>8);
    pdu.data[3] = RxStateMachine->total_packet_number;
    pdu.data[4] = RESERVED_BYTE;
    pdu.data[7] = (uint8_t)((RxStateMachine->PGN>>16) & 0xff);  /* MSB */
    pdu.data[6] = (uint8_t)(RxStateMachine->PGN>>8 & 0xff);
    /*PDU Format < 240*/
    pdu.data[5] = (uint8_t)(RxStateMachine->PGN & 0x00);   /* LSB */

    if(PackFrame(&pdu) == J1939_ERROR)
       return FALSE;
    return TRUE;
}
