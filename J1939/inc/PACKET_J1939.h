#ifndef __PACKET_J1939_H__
#define __PACKET_J1939_H__

#include "CAN_INTERFACE_J1939.h"

typedef uint32_t PGN_T;

/*PRIVATE DEFINES*/
#define CAN_BUFFER_SIZE 	400
#define MAX_CAN_FRAMES 		8
#define FRAME_HEADER		5

/*Mask Defines*/
#define PGN_MASK			0x3FFFF
#define PRIORITY_MASK		0x7
#define EDP_MASK			0x02
#define DP_MASK				0x01


typedef struct 
{
	u8 		edp_dp;			/*EDP & Data Page*/
	u8		pf;				/*PDU Format*/
	u8 		ps;				/*PDU Specific Address*/
	PGN_T	pgn;			/*PGN(PARAMETER GROUP NAME)*/
}PgnTypeDef;

typedef struct 
{
	PgnTypeDef PGN;
	u8 		   dlc;			/*DATA LENGTH*/
	u8 		   priority;	/*Priority*/
	u8 		   sa;			/*Source Address*/
	u8 		   data[MAX_CAN_FRAMES];
}J19139_PduTypeDef;

/*Function Decleration*/
J1939_RTYPE Interface_Init(J19139_PduTypeDef* pkt);
J1939_RTYPE Interface_DeInit(J19139_PduTypeDef* pkt);
J1939_RTYPE PackFrame(J19139_PduTypeDef* pkt);
J1939_RTYPE RetrieveFrame(J19139_PduTypeDef* pkt);

#endif
