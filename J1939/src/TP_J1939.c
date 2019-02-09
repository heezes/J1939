#include "TP_J1939.h"

#define TRUE 	1
#define FALSE	!TRUE

static J1939_RX_MESSAGE_T J1939_rx_message;
static J1939_RX_STATE_MACHINE_T J1939_rx_state_machine;
//static J1939_RX_PDU_T J1939_rx_pdu;


void TP_Init(void)
{

}

#if 0
static void TP_PreProcess(J19139_PacketTypeDef *pkt)
{
   U08 i;
   
   if ((pdu_ptr->PGN == TP_CM) || (pdu_ptr->PGN == TP_DT))
   {
      J1939_rx_pdu.PGN = pkt->PGN;
      J1939_rx_pdu.dest_addr = pkt->PGN.ps;
      J1939_rx_pdu.source_addr = pkt->PGN.sa;
      J1939_rx_pdu.byte_count = pkt->dlc;
      
      for (i = 0; i < NUMBER_PDU_BUFFERS; i++)
      {
         J1939_rx_pdu.data[i] = pdu_ptr->data[i];
      }    
   }
   else
   {
      J1939_RX_MESSAGE_T msg;

	  msg.PGN = pdu_ptr->PGN;
      msg.dest_addr = pdu_ptr->dest_addr;
      msg.source_addr = pdu_ptr->source_addr;
      msg.byte_count = pdu_ptr->byte_count;
      
      //for (i = 0; i < NUMBER_TRANS_RX_BUFFERS; i++)
	  for (i = 0; i < NUMBER_PDU_BUFFERS; i++)
	  {
         msg.data[i] = pdu_ptr->data[i];
      }      
      // send message up the stack to the Network Management Layer
      NML_process(&msg);
   }
   
   return;
}
#endif

void TP_process(void)
{      
   u8 i;
   u8 go_on;
   
   J19139_PduTypeDef J1939Pdu;
   if(RetrieveFrame(&J1939Pdu) == J1939_OK)
   {
	   go_on = TRUE;
	   J1939_rx_state_machine.timer_counter++;

	   while (go_on)
	   {
	      switch (J1939_rx_state_machine.status)
	      {
	         case  WAIT_FOR_MESSAGE:
	            if ((J1939Pdu.PGN.pgn == TP_CM) && (J1939Pdu.PGN.ps == GLOBADDR || J1939Pdu.PGN.ps == NODEADDR))
	            {
	               if (J1939Pdu.data[0] == TP_CM_RTS)
	               {
	                  J1939_rx_state_machine.PGN = J1939Pdu.data[6];
	                  J1939_rx_state_machine.PGN <<= 8;
	                  J1939_rx_state_machine.PGN += J1939Pdu.data[5];
	                  J1939_rx_state_machine.dest_addr = J1939Pdu.PGN.ps;
	                  J1939_rx_state_machine.source_addr = J1939Pdu.sa;
	                  J1939_rx_state_machine.packet_number = 0;
	                  J1939_rx_state_machine.timer_counter = 0;
	                  J1939_rx_state_machine.total_packet_number = J1939Pdu.data[3];
	                  J1939_rx_state_machine.byte_count = J1939Pdu.data[2];
	                  J1939_rx_state_machine.byte_count <<= 8;
	                  J1939_rx_state_machine.byte_count += J1939Pdu.data[1];
	                  J1939_rx_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
	                  J1939_rx_state_machine.TP = TP_CM_RTS;
	               }
	               else if (J1939Pdu.data[0] == TP_CM_BAM)
	               {
	                  J1939_rx_state_machine.PGN = J1939Pdu.data[6];
	                  J1939_rx_state_machine.PGN <<= 8;
	                  J1939_rx_state_machine.PGN += J1939Pdu.data[5];
	                  J1939_rx_state_machine.dest_addr = J1939Pdu.PGN.ps;
	                  J1939_rx_state_machine.source_addr = J1939Pdu.sa;
	                  J1939_rx_state_machine.packet_number = 0;
	                  J1939_rx_state_machine.timer_counter = 0;
	                  J1939_rx_state_machine.total_packet_number = J1939Pdu.data[3];
	                  J1939_rx_state_machine.byte_count = J1939Pdu.data[2];
	                  J1939_rx_state_machine.byte_count <<= 8;
	                  J1939_rx_state_machine.byte_count += J1939Pdu.data[1];
	                  J1939_rx_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
	                  J1939_rx_state_machine.TP = TP_CM_BAM;          
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
	            else if (J1939_rx_state_machine.TP == TP_CM_BAM)
	            {
	               if (J1939_rx_state_machine.byte_count < NUMBER_TRANS_RX_BUFFERS)
	               {                  
	                  J1939Pdu.PGN.pgn = J1939_rx_state_machine.PGN;
	                  J1939Pdu.PGN.ps = J1939_rx_state_machine.dest_addr;
	                  J1939Pdu.sa = J1939_rx_state_machine.source_addr;
	                  J1939Pdu.dlc = J1939_rx_state_machine.byte_count;
	               
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
	      
	         case SEND_CTS_WITH_COUNT:
	         break;
	         
	         case WAIT_FOR_DATA:
	            if (J1939Pdu.PGN.pgn == TP_DT)
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
	            if (J1939_rx_state_machine.timer_counter > (750/TICK))   // 750ms
	            {
	               J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
	               //J1939_rx_state_machine.timer_counter = 0;
	               //break;
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
	            J1939Pdu.PGN.pgn = 0;
	            J1939Pdu.PGN.ps = 0;
	            J1939Pdu.sa = 0;
	            J1939Pdu.dlc = 0;

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
	               if (J1939Pdu.sa == J1939_rx_state_machine.source_addr &&
	                  J1939Pdu.PGN.ps == J1939_rx_state_machine.dest_addr)
	               {
	                  if (J1939_rx_state_machine.packet_number == J1939Pdu.data[0]-1)
	                  {
	                     J1939_rx_state_machine.status = SAVE_DATA;
	                     J1939_rx_state_machine.timer_counter = 0;
	                     J1939Pdu.data[0] = 0;               
	                  }
	                  else if(J1939Pdu.data[0] != 0)
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
	               J1939_rx_message.data[J1939_rx_state_machine.packet_number*7+i] = J1939Pdu.data[i+1];
	               J1939Pdu.data[i+1] = 0;
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
	      
	         case SEND_CTS:
	      
	         case FILL_USER_MESSAGE:
	            J1939_rx_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
	            J1939_rx_state_machine.timer_counter = 0;

	            // send message up the stack to the Network Management Layer
	            
	            //NML_process(&J1939_rx_message);

	            //go_on = FALSE;
	         break;
	         

	         default:
	            go_on = FALSE;
	         break;
	                                                
	      } // end switch   
	   } // end while   
   } // end If(retreiveFrame)
}


J1939_RTYPE Transmit_J1939msg(J1939_TX_MESSAGE_T *msg_ptr)
{
	J1939_RTYPE ret = J1939_OK;
   if (msg_ptr->byte_count > 8)
   {
#if NOT_YET
      // Transport layer for transmission of J1939 messages > 8 bytes not implemented yet
#endif
   }
   else
   {
	   J19139_PduTypeDef pkt;
      ret = PackFrame(&pkt);
   }

   return ret;
}
