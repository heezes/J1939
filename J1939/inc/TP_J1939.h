#ifndef __J1939_TL_H__
#define __J1939_TL_H__

#include "PACKET_J1939.h"


typedef struct
{
   PGN_T PGN;
   u8 	data[NUMBER_TRANS_TX_BUFFERS];
   u16 	byte_count;
   u8 	priority;
   u8 	dest_addr;
   s8 	status;
} J1939_TX_MESSAGE_T;

typedef struct
{
   PGN_T PGN;
   u8 	data[NUMBER_TRANS_RX_BUFFERS];
   u16 	byte_count;
   u8 	source_addr;
   u8 	dest_addr;
} J1939_RX_MESSAGE_T;

typedef struct
{
   PGN_T PGN;
   u8 	data[NUMBER_PDU_BUFFERS];
   u16 	byte_count;
   u8 	dest_addr;   
   u8 	source_addr;
} J1939_RX_PDU_T;

typedef struct
{
   PGN_T PGN;
   u8 	status;
   u8 	packet_number;
   u8 	total_packet_number;
   u16 	byte_count;
   u16 	timer_counter;
   u8 	source_addr;
   u8 	dest_addr;
   u8 	TP;
} J1939_RX_STATE_MACHINE_T;


//==========================================================================================
// Transport Layer Interface Functions
//==========================================================================================

J1939_RTYPE TL_init(void);
void TL_process(void);
void TL_periodic(void);
u8 Transmit_J1939_BAM(PGN_T pgn, u16 size, u8 total_packets);
u8 Transmit_J1939msg(J1939_TX_MESSAGE_T *msg);

#endif
