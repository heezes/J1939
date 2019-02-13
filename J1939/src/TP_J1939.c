#include "TP_J1939.h"

#define TRUE 	1
#define FALSE	!TRUE

static J1939_RX_MESSAGE_T J1939_rx_message;
static J1939_STATE_MACHINE_T J1939_state_machine;
//static J1939_TX_MESSAGE_T J1939_tx_message;
static J19139_PduTypeDef J1939Pdu;


void TP_Init(void)
{
   U08 i;
  
   J1939_rx_message.PGN = 0;
   J1939_rx_message.dest_addr = 0;
   J1939_rx_message.source_addr = 0;
   J1939_rx_message.byte_count = 0;
   /*
   J1939_rx_pdu.PGN = 0;
   J1939_rx_pdu.dest_addr = 0;
   J1939_rx_pdu.source_addr = 0;
   J1939_rx_pdu.byte_count = 0;
  */
   J1939_state_machine.PGN = 0;
   J1939_state_machine.dest_addr = 0;
   J1939_state_machine.source_addr = 0;
   J1939_state_machine.packet_number = 0;
   J1939_state_machine.timer_counter = 0;
   J1939_state_machine.total_packet_number = 0;
   J1939_state_machine.byte_count = 0;
   J1939_state_machine.status = WAIT_FOR_MESSAGE;
   J1939_state_machine.TP = TP_NONE;
   

   for (i = 0; i < NUMBER_TRANS_RX_BUFFERS; i++)
   {
      J1939_rx_message.data[i] = 0;
   }
}

__weak void DataReceiveCpltCallback(J1939_RX_MESSAGE_T *rxMsg)
{

}

void TP_Process(void)
{
	J1939_RTYPE ret = J1939_OK;
	ret = RetrieveFrame(&J1939Pdu);
	if(ret == J1939_OK)
	{
		   if((J1939Pdu.PGN.pgn != TP_CM) || (J1939Pdu.PGN.pgn != TP_DT))
		   {
			  J1939_rx_message.PGN = J1939Pdu.PGN.pgn;
			  J1939_rx_message.dest_addr = J1939Pdu.PGN.ps;
			  J1939_rx_message.source_addr = J1939Pdu.sa;
			  J1939_rx_message.byte_count = J1939Pdu.dlc;

			  for (i = 0; i < NUMBER_PDU_BUFFERS; i++)
			  {
				  J1939_rx_message.data[i] = J1939Pdu.data[i];
		      }
		      // send message up the stack
			  DataReceiveCpltCallback(&J1939_rx_message);
		   }
	}
	else
		print_string("Frame Read Timeout\r\n");
	return;
}

void TP_StateMachine_Run(void)
{      
   u8 i;
   u8 go_on;

   go_on = TRUE;
   J1939_state_machine.timer_counter++;
	 /*
	  * TODO: Add DA and SA Check for Rejecting Conn
	  * TODO: Buffer Allocation for Different Concurrent Connection(BAM or TP)
	  * */
   while (go_on)
   {
	  switch (J1939_state_machine.status)
	  {
		 case  WAIT_FOR_MESSAGE:
			if ((J1939Pdu.PGN.pgn == TP_CM) && (J1939Pdu.PGN.ps == GLOBADDR || J1939Pdu.PGN.ps == NODEADDR))
			{
			   if (J1939Pdu.data[0] == TP_CM_RTS)
			   {
				  J1939_state_machine.PGN = J1939Pdu.data[6];
				  J1939_state_machine.PGN <<= 8;
				  J1939_state_machine.PGN += J1939Pdu.data[5];
				  J1939_state_machine.dest_addr = J1939Pdu.PGN.ps;
				  J1939_state_machine.source_addr = J1939Pdu.sa;
				  J1939_state_machine.packet_number = 0;
				  J1939_state_machine.timer_counter = 0;
				  J1939_state_machine.total_packet_number = J1939Pdu.data[3];
				  J1939_state_machine.byte_count = J1939Pdu.data[2];
				  J1939_state_machine.byte_count <<= 8;
				  J1939_state_machine.byte_count += J1939Pdu.data[1];
				  J1939_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
				  J1939_state_machine.TP = TP_RX;
			   }
			   else if (J1939Pdu.data[0] == TP_CM_BAM)
			   {
				  J1939_state_machine.PGN = J1939Pdu.data[6];
				  J1939_state_machine.PGN <<= 8;
				  J1939_state_machine.PGN += J1939Pdu.data[5];
				  J1939_state_machine.dest_addr = J1939Pdu.PGN.ps;
				  J1939_state_machine.source_addr = J1939Pdu.sa;
				  J1939_state_machine.packet_number = 0;
				  J1939_state_machine.timer_counter = 0;
				  J1939_state_machine.total_packet_number = J1939Pdu.data[3];
				  J1939_state_machine.byte_count = J1939Pdu.data[2];
				  J1939_state_machine.byte_count <<= 8;
				  J1939_state_machine.byte_count += J1939Pdu.data[1];
				  J1939_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
				  J1939_state_machine.TP = TP_CM_BAM;
			   }
			   else if(J1939Pdu.data[0] == TP_CM_CTS)
			   {
				  J1939_state_machine.PGN = J1939Pdu.data[6];
				  J1939_state_machine.PGN <<= 8;
				  J1939_state_machine.PGN += J1939Pdu.data[5];
				  J1939_state_machine.dest_addr = J1939Pdu.PGN.ps;
				  J1939_state_machine.source_addr = J1939Pdu.sa;
				  J1939_state_machine.packet_number = J1939Pdu.data[0];
				  J1939_state_machine.timer_counter = 0;
				  /*J1939_state_machine.total_packet_number = J1939Pdu.data[3];
				  J1939_state_machine.byte_count = J1939Pdu.data[2];
				  J1939_state_machine.byte_count <<= 8;
				  J1939_state_machine.byte_count += J1939Pdu.data[1];*/
				  J1939_state_machine.cts_count	  = J1939.data[1];
				  J1939_state_machine.status = INIT_REASSEMBLE_STRUCTURE;
				  J1939_state_machine.TP = TP_TX;
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
			if (J1939_state_machine.TP == TP_CM_RTS)
			{
				if(J1939_state_machine.byte_count < NUMBER_TRANS_RX_BUFFERS)
				{
					J1939_state_machine.status = SEND_DT;
					J1939_state_machine.timer_counter = 0;
				}
			   else
			   {
				  J1939_state_machine.status = SEND_ABORT;
				  J1939_state_machine.timer_counter = 0;
				  go_on = FALSE;
			   }
			}
			else if (J1939_state_machine.TP == TP_CM_BAM)
			{
			   if (J1939_state_machine.byte_count < NUMBER_TRANS_RX_BUFFERS)
			   {
				   J1939_rx_message.PGN = J1939_state_machine.PGN;
				   J1939_rx_message.dest_addr = J1939_state_machine.dest_addr;
				   J1939_rx_message.source_addr = J1939_state_machine.source_addr;
				   J1939_rx_message.byte_count = J1939_state_machine.byte_count;

				  for (i = 0; i < NUMBER_TRANS_RX_BUFFERS; i++)
				  {
					  J1939_rx_message.data[i] = 0;
				  }
				  J1939_state_machine.status = WAIT_FOR_DATA;
			   }
			   else
			   {
				  J1939_state_machine.status= WAIT_FOR_MESSAGE;
				  J1939_state_machine.timer_counter = 0;
				  go_on = FALSE;
			   }
			}
			else
			{
			   J1939_state_machine.status = WAIT_FOR_MESSAGE;
			   J1939_state_machine.timer_counter = 0;
			   go_on = FALSE;
			}
		 break;

		 case SEND_DT:
		 break;

		 case SEND_ABORT:
		 break;
	  
		 case WAIT_FOR_CTS:
			 /*SEND CLEAR TO SEND*/
		 break;

		 case WAIT_FOR_DATA:
			if (J1939Pdu.PGN.pgn == TP_DT)
			{
			   J1939_state_machine.status = CHECK_DATA_PACKET;
			   //break;
			}
			else
			{
			   J1939_state_machine.status = CHECK_TIMER;
			   //break;
			}
		 break;

		 case CHECK_TIMER:
			if (J1939_state_machine.timer_counter > (750/TICK))   // 750ms
			{
			   /*Timeout*/
			   J1939_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
			}
			else
			{
			   J1939_state_machine.status = WAIT_FOR_DATA;
			   go_on = FALSE;
			}
		 break;

		 case RESET_REASSEMBLY_STRUCTURE:
			memset(J1939_state_machine, 0, sizeof(J1939_state_machine));
			memset(J1939Pdu, 0, sizeof(J1939Pdu));
			/*J1939_state_machine.PGN = 0;
			J1939_state_machine.dest_addr = 0;
			J1939_state_machine.source_addr = 0;
			J1939_state_machine.packet_number = 0;
			J1939_state_machine.timer_counter = 0;
			J1939_state_machine.total_packet_number = 0;
			J1939_state_machine.byte_count = 0;
			J1939Pdu.PGN.pgn = 0;
			J1939Pdu.PGN.ps = 0;
			J1939Pdu.sa = 0;
			J1939Pdu.dlc = 0;

			for (i = 0; i < NUMBER_TRANS_RX_BUFFERS; i++)
			{
				J1939_rx_message.data[i] = 0;
			}*/
			J1939_state_machine.status = WAIT_FOR_MESSAGE;
			go_on = FALSE;
		 break;

		 case CHECK_DATA_PACKET:
			if (J1939_state_machine.TP == (TP_CM_BAM|TP_CM_RTS|TP_CM_CTS))
			{
			   if (J1939Pdu.sa == J1939_state_machine.source_addr &&
				  J1939Pdu.PGN.ps == J1939_state_machine.dest_addr)
			   {
				  if (J1939_state_machine.packet_number == J1939Pdu.data[0]-1)
				  {
					 J1939_state_machine.status = SAVE_DATA;
					 J1939_state_machine.timer_counter = 0;
					 J1939Pdu.data[0] = 0;
				  }
				  else if(J1939Pdu.data[0] != 0)
				  {
					 J1939_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
				  }
				  else
				  {
					 J1939_state_machine.status = WAIT_FOR_DATA;
					 go_on = FALSE;
				  }
			   }
			   else
			   {
				  J1939_state_machine.status = WAIT_FOR_DATA;
				  go_on = FALSE;
			   }
			}
			else if(J1939Pdu.PGN.pgn == TP_CM_CTS)
			{}
			else
			{}
		 break;

		 case SAVE_DATA:
			i=0;

			while((i < 7) && ((J1939_state_machine.packet_number*7+i) <= J1939_state_machine.byte_count))
			{
			   J1939_rx_message.data[J1939_state_machine.packet_number*7+i] = J1939Pdu.data[i+1];
			   J1939Pdu.data[i+1] = 0;
			   i++;
			}
			J1939_state_machine.packet_number++;
			if (J1939_state_machine.packet_number == J1939_state_machine.total_packet_number)
			{
			   J1939_state_machine.status = FILL_USER_MESSAGE;
			}
			else if (((J1939_state_machine.packet_number/TRANSPORT_PACKET_COUNT) == J1939_state_machine.cts_count+1) && J1939_state_machine.TP == TP_CM_RTS)
			{
				J1939_state_machine.status 		  = SEND_CTS;
				J1939_state_machine.timer_counter = 0;
			}
			else
			{
			   J1939_state_machine.status = WAIT_FOR_DATA;
			   go_on = FALSE;
			}
		 break;

		 case SEND_EOM:
			 /*SEND OF EOM*/
			J1939_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
			J1939_state_machine.timer_counter = 0;
			// send message up the stack to the Network Management Layer
			DataReceiveCpltCallback(&J1939_rx_message);
/*************************************************************************************************************************************************************/
		 case SEND_CTS:
			 /*SEND CLEAR TO SEND*/

		 case FILL_USER_MESSAGE:
			J1939_state_machine.status = RESET_REASSEMBLY_STRUCTURE;
			J1939_state_machine.timer_counter = 0;
			// send message up the stack to the Network Management Layer
		 break;


		 default:
			go_on = FALSE;
		 break;
	  } // end switch
   } // end while
}


J1939_RTYPE Transmit_J1939msg(J1939_TX_MESSAGE_T *msg_ptr)
{
	J1939_RTYPE ret = J1939_OK;
   if (msg_ptr->byte_count > 8)
   {
	   J1939_CM_RTS(msg_ptr->PGN, msg_ptr->dest_addr, msg_ptr->byte_count);
   }
   else
   {
	  J19139_PduTypeDef pkt;
	  memset(pkt, 0, sizeof(J19139_PduTypeDef));
	  pkt.PGN.pgn 	=	msg_ptr->PGN;
	  pkt.dlc 		= 	msg_ptr->byte_count;
	  pkt.priority	=  	msg_ptr->priority;
	  pkt.PGN.ps    = 	msg_ptr->dest_addr;
      ret = PackFrame(&pkt);
   }

   return ret;
}


void J1939_CM_RTS(PGN_T pgn, u8 destination_address, u16 size)
{
	/*
	 * Byte: 1 Control byte = 16, Destination Specific Request_To_Send (RTS)
		2,3 Total message size, number of bytes
		4 Total number of packets
		5 Maximum number of packets that can be sent in response to one CTS. FF16 indicates that
		no limit exists for the originator.
		6-8 Parameter Group Number of the packeted message
		Byte 6 Parameter Group Number of requested information
		(8 LSB of parameter group number, bit 8 most significant)
		Byte 7 Parameter Group Number of requested information
		(2nd byte of parameter group number, bit 8 most significant)
		Byte 8 Parameter Group Number of requested information
		(8 MSBs of parameter group number, bit 8 most significant)
	 */
    J19139_PduTypeDef J1939Pdu;
    J1939Pdu.PGN.pgn	=	TP_CM;
    J1939Pdu.PGN.edp_dp	=	0;
    J1939Pdu.PGN.pf		=	TP_CM_PF;
    J1939Pdu.PGN.ps		= 	destination_address;
    J1939Pdu.dlc		=	NUMBER_PDU_BUFFERS;
    J1939Pdu.priority	=	TP_CM_PRIORITY;
    J1939Pdu.sa			=	DEVICE_ADDRESS;
	/* initial RTS data */
    J1939Pdu.data[0] = TP_CM_RTS;
    J1939Pdu.data[1] = (u8) size ;
    J1939Pdu.data[2] = (u8) ((size)>>8);
    J1939Pdu.data[3] = tx_msg.total_packets;
    J1939Pdu.data[4] = RESERVED_BYTE;/*(0xFF)*/
    J1939Pdu.data[7] = (u8)((pgn>>16) & 0xff);  /* MSB */
    J1939Pdu.data[6] = (u8)(pgn>>8 & 0xff);
    J1939Pdu.data[5] = (u8)(pgn & 0xff);   /* LSB */

    if(PackFrame(&J19139Pdu) == J1939_OK)
    {
		/* reset the connection timer and wait CTS message */
    	J1939_tx_message.state = TP_CM_TIMER;
		tx_msg.state = ENUM_J1939_TX_CM_WAIT;
    }
}

void J1939_CM_ABORT(PGN_T pgn, u8 destination_address, u8 code)
{
    J19139_PduTypeDef J1939Pdu;
    J1939Pdu.PGN.pgn	=	TP_CM;
    J1939Pdu.PGN.edp_dp	=	0;
    J1939Pdu.PGN.pf		=	TP_CM_PF;
    J1939Pdu.PGN.ps		= 	destination_address;
    J1939Pdu.dlc		=	NUMBER_PDU_BUFFERS;
    J1939Pdu.priority	=	TP_CM_PRIORITY;
    J1939Pdu.sa			=	DEVICE_ADDRESS;
	/* initial RTS data */
    J1939Pdu.data[0] = TP_CM_CONN_ABORT;
    J1939Pdu.data[1] = code;
    J1939Pdu.data[2] = RESERVED_BYTE;
    J1939Pdu.data[3] = RESERVED_BYTE;
    J1939Pdu.data[4] = RESERVED_BYTE;
    J1939Pdu.data[7] = (uint8_t)((pgn_num>>16) & 0xff);  /* MSB */
    J1939Pdu.data[6] = (uint8_t)(pgn_num>>8 & 0xff);
    J1939Pdu.data[5] = (uint8_t)(pgn_num & 0xff);   /* LSB */
    if(PackFrame(&J19139Pdu) == J1939_OK)
    {
		/* reset the connection timer and wait CTS message */

    }
}

void J1939_CM_CTS(PGN_T pgn, u8 destination_address, u8 packet_number)
{
    J19139_PduTypeDef J1939Pdu;
    J1939Pdu.PGN.pgn	=	TP_CM;
    J1939Pdu.PGN.edp_dp	=	0;
    J1939Pdu.PGN.pf		=	TP_CM_PF;
    J1939Pdu.PGN.ps		= 	destination_address;
    J1939Pdu.dlc		=	NUMBER_PDU_BUFFERS;
    J1939Pdu.priority	=	TP_CM_PRIORITY;
    J1939Pdu.sa			=	DEVICE_ADDRESS;
	/* initial RTS data */
    J1939Pdu.data[0] = TP_CM_CTS;
    J1939Pdu.data[1] = (u8)TRANSPORT_PACKET_COUNT;
    J1939Pdu.data[2] = packet_number;
    J1939Pdu.data[3] = RESERVED_BYTE;/*(0xFF)*/
    J1939Pdu.data[4] = RESERVED_BYTE;/*(0xFF)*/
    J1939Pdu.data[7] = (u8)((pgn>>16) & 0xff);  /* MSB */
    J1939Pdu.data[6] = (u8)(pgn>>8 & 0xff);
    J1939Pdu.data[5] = (u8)(pgn & 0xff);   /* LSB */

    if(PackFrame(&J19139Pdu) == J1939_OK)
    {
		/* reset the connection timer and wait CTS message */

    }
}

void J1939_CM_EOM(PGN_T pgn, u8 destination_address, u16 size, u8 total_packet)
{
	/*End of Message Acknowledgment (TP.CM_EndOfMsgACK): Destination Specific
	Byte: 1 Control byte = 19, End_of_Message Acknowledge
	2,3 Total message size, number of bytes
	4 Total number of packets
	5 Reserved for assignment by SAE, this byte should be filled with FF16
	6-8 Parameter Group Number of the packeted message*/
    J19139_PduTypeDef J1939Pdu;
    J1939Pdu.PGN.pgn	=	TP_CM;
    J1939Pdu.PGN.edp_dp	=	0;
    J1939Pdu.PGN.pf		=	TP_CM_PF;
    J1939Pdu.PGN.ps		= 	destination_address;
    J1939Pdu.dlc		=	NUMBER_PDU_BUFFERS;
    J1939Pdu.priority	=	TP_CM_PRIORITY;
    J1939Pdu.sa			=	DEVICE_ADDRESS;
	/* initial RTS data */
    J1939Pdu.data[0] = TP_CM_END_OF_MSG_ACK;
    J1939Pdu.data[1] = (u8) size ;
    J1939Pdu.data[2] = packet_number;
    J1939Pdu.data[3] = total_packet;/*(0xFF)*/
    J1939Pdu.data[4] = RESERVED_BYTE;/*(0xFF)*/
    J1939Pdu.data[7] = (u8)((pgn>>16) & 0xff);  /* MSB */
    J1939Pdu.data[6] = (u8)(pgn>>8 & 0xff);
    J1939Pdu.data[5] = (u8)(pgn & 0xff);   /* LSB */

    if(PackFrame(&J19139Pdu) == J1939_OK)
    {
		/* reset the connection timer and wait CTS message */

    }
}

void J1939_CM_BAM(PGN_T pgn, u8 destination_address, u16 size, u8 total_packet)
{
	/*Broadcast Announce Message (TP.CM_BAM): Global Destination
	Byte: 1 Control byte = 32, Broadcast Announce Message
	2,3 Total message size, number of bytes
	4 Total number of packets
	5 Reserved for assignment by SAE, this byte should be filled with FF16
	6-8 Parameter Group Number of the packeted message*/
    J19139_PduTypeDef J1939Pdu;
    J1939Pdu.PGN.pgn	=	TP_CM;
    J1939Pdu.PGN.edp_dp	=	0;
    J1939Pdu.PGN.pf		=	TP_CM_PF;
    J1939Pdu.PGN.ps		= 	destination_address;
    J1939Pdu.dlc		=	NUMBER_PDU_BUFFERS;
    J1939Pdu.priority	=	TP_CM_PRIORITY;
    J1939Pdu.sa			=	DEVICE_ADDRESS;
	/* initial RTS data */
    J1939Pdu.data[0] = TP_CM_BAM;
    J1939Pdu.data[1] = (u8) size ;
    J1939Pdu.data[2] = packet_number;
    J1939Pdu.data[3] = total_packet;/*(0xFF)*/
    J1939Pdu.data[4] = RESERVED_BYTE;/*(0xFF)*/
    J1939Pdu.data[7] = (u8)((pgn>>16) & 0xff);  /* MSB */
    J1939Pdu.data[6] = (u8)(pgn>>8 & 0xff);
    J1939Pdu.data[5] = (u8)(pgn & 0xff);   /* LSB */

    if(PackFrame(&J19139Pdu) == J1939_OK)
    {
		/* reset the connection timer and wait CTS message */

    }
}

void J1939_TP_DT(u8 packet_number, u8* data, len)
{
    J19139_PduTypeDef J1939Pdu;
    J1939Pdu.PGN.pgn	=	TP_DT;
    J1939Pdu.PGN.edp_dp	=	0;
    J1939Pdu.PGN.pf		=	TP_DT_PF;
    J1939Pdu.PGN.ps		= 	destination_address;
    J1939Pdu.dlc		=	NUMBER_PDU_BUFFERS;
    J1939Pdu.priority	=	TP_DT_PRIORITY;
    J1939Pdu.sa			=	DEVICE_ADDRESS;
	/* initial RTS data */
    J1939Pdu.data[0] = packet_number;
    for(int i = 0;i<len; i++)
    	J1939Pdu.data[i] = data[i];
    for(int i=0; i<7-len;i++)
    	J1939Pdu.data[i+len] = 0xFF;
    if(PackFrame(&J19139Pdu) == J1939_OK)
    {
		/* reset the connection timer and wait CTS message */
    }
}
