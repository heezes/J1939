#include "TP_J1939.h"

extern J1939_RX_MESSAGE_T J1939_rx_message;
static J1939_RX_STATE_MACHINE_T J1939_rx_state_machine;
static J1939_RX_PDU_T J1939_rx_pdu;

__weak void Can_DataCallback(J1939_RX_MESSAGE_T *msg)
{}

//==========================================================================================
// Transport Layer Interface Functions
//==========================================================================================

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
      
         case SEND_CTS_WITH_COUNT:
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

u8 Transmit_J1939_BAM(PGN_T pgn, u16 size, u8 total_packets)
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
    pdu.data[0] = TP_CM_BAM;
    pdu.data[1] = (uint8_t) size ;
    pdu.data[2] = (uint8_t) ((size)>>8);
    pdu.data[3] = total_packets;
    pdu.data[4] = RESERVED_BYTE;
    pdu.data[7] = (uint8_t)((pgn>>16) & 0xff);  /* MSB */
    pdu.data[6] = (uint8_t)(pgn>>8 & 0xff);
    pdu.data[5] = (uint8_t)(pgn & 0xff);   /* LSB */

    if(PackFrame(&pdu) == J1939_ERROR)
       return FALSE;
    return TRUE;
}

u8 Transmit_J1939msg(J1939_TX_MESSAGE_T *msg)
{
   if (msg->byte_count > 8)
   {
#if NOT_YET
      // Transport layer for transmission of J1939 messages > 8 bytes not implemented yet
#endif
   }
   else
   {
      //Build_CANpkt(msg_ptr, 0);
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
