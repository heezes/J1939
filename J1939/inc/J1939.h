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

typedef enum 
{
	J1939_ERROR = -1,
	J1939_OK,
	J1939_TIMEOUT
}J1939_RTYPE;

#endif
