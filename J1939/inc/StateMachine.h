#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include "PACKET_J1939.h"

/**/
#define TRUE	1
#define FALSE	!TRUE

/*FSM STATES*/
#define  RECEIVE_PACKET    1
#define  CHECK_PACKET      2
#define  WAIT_FOR_DATA     3
#define  CHECK_TIMER       4
#define  RESET_STATE       5
#define  CHECK_EOM    	   6
#define  SAVE_DATA         7

/*Connection Management PGN*/
#define TP_CM_PGN          0x00EC00

/*Transport Protocol Mode*/
#define BAM_DT         0
#define TP_DT 	       1
/*Control Bytes*/
#define	TP_CM_DT		   1
#define TP_CM_RTS          16
#define TP_CM_CTS          17
#define TP_CM_BAM          32
#define TP_CM_EOM          19
#define TP_CM_ABORT        255

/**/
#define NODEADDR                       0x23
#define GLOBADDR                       0xFF
#define NULLADDR                       0xFE
#define NUMBER_PDU_BUFFERS 			   8
#define MAX_BAM_TIMEOUT				   750

typedef struct
{
   PGN_T PGN;
   u8    state;
   u8    packet_number;
   u8    total_packet_number;
   u8 	 cts_count;
   u16   byte_count;
   //u16   timer_counter;
   u8    source_addr;
   u8    dest_addr;
   u8    mode;
   u8    buffer[NUMBER_PDU_BUFFERS];
   struct timer *t;
}J1939_StateMachine;

#endif
