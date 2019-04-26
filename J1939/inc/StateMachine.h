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
   u8 	cts_count;
   u16 	byte_count;
   u16 	timer_counter;
   u8 	source_addr;
   u8 	dest_addr;
   u8 	TP;
   struct timer t;
} J1939_RX_STATE_MACHINE_T;

typedef struct
{
   u8 	status;
   u8 	packet_number;
   u8 	cts_count;
   struct timer t;
} J1939_TX_STATE_MACHINE_T;


//==========================================================================================
// Transport Layer Interface Functions
//==========================================================================================
u8 GET_PACKET_NUMBER(u16 byte_count);
J1939_RTYPE TL_init(void);
void TL_process(J1939_RX_PDU_T *J1939_rx_pdu);
void TL_periodic(J1939_RX_STATE_MACHINE_T *J1939_rx_state_machine, J1939_RX_PDU_T *J1939_rx_pdu, J1939_RX_MESSAGE_T *J1939_rx_message);
void TP_tx_Process(J1939_TX_STATE_MACHINE_T *J1939_tx_state_machine, J1939_RX_PDU_T *J1939_rx_pdu, J1939_TX_MESSAGE_T *TxMsg);
u8 Transmit_J1939_BAM(J1939_TX_MESSAGE_T *msg, J1939_TX_STATE_MACHINE_T *J1939_tx_state_machine);
u8 Transmit_J1939_RTS(J1939_TX_MESSAGE_T *msg, J1939_TX_STATE_MACHINE_T *J1939_tx_state_machine);
u8 Transmit_J1939msg(J1939_TX_MESSAGE_T *msg, J1939_TX_STATE_MACHINE_T *J1939_tx_state_machine);

#endif
