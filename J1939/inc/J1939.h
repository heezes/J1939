#ifndef __J1939_H__
#define __J1939_H__

#include "stm32f4xx_hal.h"
#include "gp_timer.h"
#include "main.h"

typedef uint32_t u32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef int32_t	s32;
typedef	int16_t s16;
typedef int8_t  s8;

//#define NUMBER_TRANS_TX_BUFFERS		   128
//#define	NUMBER_TRANS_RX_BUFFERS		   128
#define PGN_NUM                        23
#define TICK                           20        // 20ms
#define NUMBER_TRANS_RX_MACHINES
#define J1939_MAX_MESSAGE_LENGTH       1785
#define NUMBER_TRANS_RX_BUFFERS        64
#define NUMBER_TRANS_TX_BUFFERS        64
#define NUMBER_PDU_BUFFERS             8
#define BIG_E                          0         // 1:Big-Endian 0:Little-Endian
#define OUT_BUFFER_SIZE                21
#define IN_BUFFER_SIZE                 21
#define CAN_MAX_BYTE_COUNT             8
#define MIN_PDU2                       240
#define ADDR_NUM                       2
//#define J1939ParDaraNum              62

#define  J1939_FAULT_NUM               (NUMBER_TRANS_RX_BUFFERS-2)/4

#define NODEADDR                       0x23
#define GLOBADDR                       0xFF
#define NULLADDR                       0xFE

#define LOWEST_PRIORITY				   3
#define HIGHEST_PRIORITY			   7
#define CM_PRIORITY			  		   HIGHEST_PRIORITY
#define RESERVED_BYTE				   0xFF
#define TRANSPORT_PACKET_COUNT		   5

/*Abort Reason*/
#define ABORT_ALREADY_CONNECTED		   1
#define ABORT_RESOURCE_UNAVILABLE	   2
#define ABORT_TIMEOUT				   3

/*Timeout Define (ms)*/
#define SEND_TIMEOUT				   750
#define RECEIVE_RESP_TIMEOUT		   1250
#define SEND_RESP_TIMEOUT			   200
#define CTS_WAIT_TIMEOUT			   1050

/**/
#define J1939STACK_VERSION             130

// TODO - Command Request/Ack Parameter Group Numbers
#define REQUEST_PGN                    0x00EA00  // 0x00EA00=59904
#define REQUEST_PGN_NUM                15
#define ACK_PGN                        0x00E800  // 0x00E800=59392
#define ACK_PGN_ACK                    0
#define ACK_PGN_NACK                   1
#define ACK_PGN_ACCESS_DENIED          2
#define ACK_PGN_CANNOT_RESPOND         3

// TODO - Proprietary Parameter Group Numbers
#define PROPRIETARY_A                  0x00EF00  // 0x00EF00=61184
#define PROPRIETARY_A2                 0x01EF00  // 0x01EF00=126720 Note: requires 24 bit PGN capability
#define PROPRIETARY_B_START            0x00FF00  // 0x00FF00=65280
#define PROPRIETARY_B_END              0x00FFFF  // 0x00FFFF=65535

// Receive state machine
#define WAIT_FOR_MESSAGE               0         // State Zero: Waiting for a BAM or RTS data packet to arrive
#define INIT_REASSEMBLE_STRUCTURE      1         // State One: Message reassemble structure and message timer are initialized.
#define CHECK_PACKET                   2         // State Two: The incoming message is for this node.
#define SEND_ABORT                     3         // State Three:  No more buffers to reassemble incoming message, Abort is sent.
#define SEND_CTS_WITH_COUNT			               4         // State Four: Buffers are available to reassemble incoming message, send CTS back to sender to indicate number of packets between next CTS (TRANSPORT_PACKET_COUNT).
#define WAIT_FOR_DATA                  5         // State Five: Waiting for DATA type packet from source.
#define CHECK_TIMER                    6         // State Six:  Message timer is checked for expiration.
#define RESET_REASSEMBLY_STRUCTURE     7         // State Seven: Message reassemble structure is reset.
#define CHECK_DATA_PACKET              8         // State Eight: A DATA type packet has arrived and checked.
#define SAVE_DATA                      9         // State Nine: The data is saved in the message reassemble buffer
#define SEND_EOM                       10        // State Ten:  EOM message is sent.
#define SEND_CTS                       11        // State Eleven: CTS is sent to trigger more data from the sender.
#define FILL_USER_MESSAGE              12        // State Twelve: The J1939_RX_MESSAGE_T data structure is filled for user application.

// Transport Protocol Layer
#define TP_CM                          0x00EC00  // 0x00EC00=60416
#define TP_DT						   0x00EB00	 // 60160
#define TP_CM_RTS                      16
#define TP_CM_CTS                      17         
#define TP_CM_END_OF_MSG_ACK           19
#define TP_CM_CONN_ABORT               255
#define TP_CM_BAM                      32
#define TP_DT                          0x00EB00  // 0x00EB00=60160
#define TP_NONE                        0 

// TODO - Network Management Layer - PGNs
#define REQUEST_FOR_ADDRESS_CLAIMED    0x00EA00  // 0x00EA00=59904         
#define ADDRESS_CLAIMED                0x00EE00  // 0x00EE00=60928
#define CANNOT_CLAIM                   0x00EE00  // 0x00EE00=60928
#define COMMANDED_ADDRESS              0x00FED8  // 0x00FED8=65240

// Diagnostic Trouble Code (DTC)
#define J1939_DM1                      0x00FECA  // 0x00FECA=65226

#define PRIMED                         1
#define NOTPRIMED                      0 

#ifndef NULL
#define NULL                           0
#endif

#ifndef TRUE
#define TRUE                           1
#endif

#ifndef FALSE              
#define FALSE                          0
#endif


typedef enum 
{
	J1939_ERROR = -1,
	J1939_OK,
	J1939_TIMEOUT
}J1939_RTYPE;

#endif
