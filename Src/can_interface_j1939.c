/* can_interface_J1939.c
 *
 *  Created on: 24-Apr-2019
 *  Author: Altamash Abdul Rahim
 *  Description: This File contains MCAL abstraction for CAN communication. It is part of Interface layer.
 *
 * Vecmocon Technologies Private Limited [CONFIDENTIAL]
 * Unpublished Copyright (c) 2019 [Vecmocon Technologies Private Limited], All Rights Reserved.
 *
 *
 * NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
 * herein are proprietary to COMPANY and may be covered by Indian and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
 * from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed
 * Confidentiality and Non-disclosure agreements explicitly covering such access.
 *
 *
 * The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes
 * information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE,
 * OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE
 * LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS
 * TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.
 *
 * Change log:
 */
#include <can_interface_j1939.h>
#include "main.h"

extern CAN_HandleTypeDef hcan1;

#define g_canHandle		 hcan1

/*This function is implemented in PACKET_J1939.c*/
extern J1939_RTYPE_t CAN_StoreToQueuefromISR(u32 id, u8* data, u8 len);

/*Platform Specific Callbacks*/
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan) {

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CAN_RxHeaderTypeDef RxHeader;
	memset(&RxHeader, 0, sizeof(CAN_RxHeaderTypeDef));
	uint8_t data[8];
	if (HAL_CAN_GetRxMessage(&g_canHandle, CAN_FILTER_FIFO0, &RxHeader, data)
			== HAL_OK) {
		CAN_StoreToQueuefromISR(RxHeader.ExtId, data, (u8) RxHeader.DLC);
	}
}

/*Private FP*/
static void CAN_FilterConfig(void) {
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	sFilterConfig.FilterIdHigh = 0;
	sFilterConfig.FilterIdLow = 0;
	sFilterConfig.FilterMaskIdHigh = 0;
	sFilterConfig.FilterMaskIdLow = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	HAL_CAN_ConfigFilter(&g_canHandle, &sFilterConfig);
}

J1939_RTYPE_t CAN_Init(void) {
	J1939_RTYPE_t ret = J1939_OK;
	/*CAN Hardware Initialization Implementation*/
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 4;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_14TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_5TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&g_canHandle) != HAL_OK)
		ret = J1939_ERROR;
	if (HAL_CAN_Start(&g_canHandle) != HAL_OK)
		ret = J1939_ERROR;
	CAN_FilterConfig();
	if (HAL_CAN_ActivateNotification(&g_canHandle,
			(CAN_IT_RX_FIFO0_MSG_PENDING)) != HAL_OK)
		ret = J1939_ERROR;
	return ret;
}

J1939_RTYPE_t CAN_DeInit(void) {
	/*CAN Harware Initialization Implementation*/
	return J1939_OK;
}

J1939_RTYPE_t CAN_Transmit(u32 id, u8* data, u8 len) {
	CAN_TxHeaderTypeDef TxHeader;
	memset(&TxHeader, 0, sizeof(CAN_TxHeaderTypeDef));
	TxHeader.ExtId = id;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = len;
	if (id > 0x7FF)
		TxHeader.IDE = CAN_ID_EXT;
	else
		TxHeader.IDE = CAN_ID_STD;
	uint32_t pTxMailbox = 0;
	if (HAL_CAN_AddTxMessage(&g_canHandle, &TxHeader, data, &pTxMailbox)
			!= HAL_OK)
		return J1939_ERROR;
	return J1939_OK;
}