/*
 * Service_J1939.h
 *
 *  Created on: 24-Apr-2019
 *      Author: Altamash Abdul Rahim
 */

#ifndef INC_SERVICE_J1939_H_
#define INC_SERVICE_J1939_H_

#include "StateMachine.h"

typedef struct
{
	J1939_TX_MESSAGE_T 			txMsg;
	J1939_RX_MESSAGE_T 			rxMsg;
	J1939_RX_PDU_T	   			rxPdu;
	J1939_RX_STATE_MACHINE_T    rxStateMachine;
	J1939_TX_STATE_MACHINE_T    txStateMachine;
}
CanHandleTypeDef;

#endif /* INC_SERVICE_J1939_H_ */
