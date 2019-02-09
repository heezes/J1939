#ifndef __CAN_INTERFACE_J1939_H__
#define __CAN_INTERFACE_J1939_H__

#include "J1939.h"


typedef struct 
{
	u32 ExtId;
	u8 	dlc;
}CanHandleTypedef;

/*Function Declaration*/
J1939_RTYPE CAN_Init(void);
J1939_RTYPE CAN_DeInit(void);
J1939_RTYPE CAN_Transmit(u32 id, u8* data, u8 len);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
#endif
