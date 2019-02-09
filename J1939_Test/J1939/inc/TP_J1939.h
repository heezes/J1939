#ifndef __TP_1939_H__
#define __TP_1939_H__

#include "PACKET_J1939.h"

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
#define SEND_CTS_WITH_COUNT            4         // State Four: Buffers are available to reassemble incoming message, send CTS back to sender to indicate number of packets between next CTS (TRANSPORT_PACKET_COUNT). 
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


#define NUMBER_PDU_BUFFERS             8
   

typedef struct
{
   PGN_T PGN;
   u8    data[NUMBER_TRANS_TX_BUFFERS];
   u16   byte_count;
   u8    priority;
   u8    dest_addr;
   s8    status;
} J1939_TX_MESSAGE_T;

typedef struct
{
   PGN_T PGN;
   u8 data[NUMBER_TRANS_RX_BUFFERS];
   u16 byte_count;
   u8 source_addr;
   u8 dest_addr;
} J1939_RX_MESSAGE_T;

typedef struct
{
   PGN_T PGN;
   u8    status;
   u8    packet_number;
   u8    total_packet_number;
   u16   byte_count;
   u16   timer_counter;
   u8    source_addr;
   u8    dest_addr;
   u8    TP;
} J1939_RX_STATE_MACHINE_T;

#endif