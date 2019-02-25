#include "CAN_INTERFACE_J1939.h"
#include "queue_aar.h"
#include "main.h"

extern Queue CAN_QUEUE;
CAN_HandleTypeDef hcan1;

/*Forward Decleration*/
static J1939_RTYPE CAN_Receive(u32 id, u8* data, u8 len);

/*Callbacks*/
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef RxHeader;
	memset(&RxHeader, 0, sizeof(CAN_RxHeaderTypeDef));
	uint8_t data[8];
	if(HAL_CAN_GetRxMessage(&hcan1, CAN_FILTER_FIFO0, &RxHeader, data) == HAL_OK)
	{
		CAN_Receive(RxHeader.ExtId, data, (u8)RxHeader.DLC);
	}
}

/*Private FP*/
static void CanFilterConfig(void)
{
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	sFilterConfig.FilterIdHigh = 0;
	sFilterConfig.FilterIdLow = 0;
	sFilterConfig.FilterMaskIdHigh = 0;
	sFilterConfig.FilterMaskIdLow = 0 ;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
}

J1939_RTYPE CAN_Init(void)
{
	J1939_RTYPE ret = J1939_OK;
	/*CAN Harware Initialization Implementation*/
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 4;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_14TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_6TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan1) != HAL_OK)
		ret = J1939_ERROR;
	if(HAL_CAN_Start(&hcan1)!=HAL_OK)
		ret = J1939_ERROR;
	CanFilterConfig();
	if(HAL_CAN_ActivateNotification(&hcan1, (CAN_IT_RX_FIFO0_MSG_PENDING)) != HAL_OK)
		ret = J1939_ERROR;
	return ret;
}

J1939_RTYPE CAN_DeInit(void)
{
	/*CAN Harware Initialization Implementation*/
	return J1939_OK;
}

static J1939_RTYPE CAN_Receive(u32 id, u8* data, u8 len)
{
/*	  print_string("\nDATA: ");
	  for(int i = 0; i<len; i++)
		print_string("%d ",data[i]);
	  print_string("\r\n");*/
	u8 space = (CAN_QUEUE.capacity - CAN_QUEUE.size);
	if(space >= 13)
	{
		enqueue(&CAN_QUEUE, &len);
		for(int i = 1; i<5; i++)
		{
			u8 temp = ((id>>((i-1)*8)) & 0xFF);
			enqueue(&CAN_QUEUE, &temp);
		}
		for(int i = 0; i<8; i++)
		{
			enqueue(&CAN_QUEUE, &data[i]);
		}
	}
	else
		return J1939_ERROR;
	return J1939_OK;
}

J1939_RTYPE CAN_Transmit(u32 id, u8* data, u8 len)
{
	CAN_TxHeaderTypeDef TxHeader;
	memset(&TxHeader, 0, sizeof(CAN_TxHeaderTypeDef));
	TxHeader.ExtId	 = id;
	TxHeader.RTR 	 = CAN_RTR_DATA;
	TxHeader.DLC 	 = len;
	if(id > 0x7FF)
		TxHeader.IDE = CAN_ID_EXT;
	else
		TxHeader.IDE = CAN_ID_STD;
	uint32_t pTxMailbox = 0;
	if(HAL_CAN_AddTxMessage(&hcan1, &TxHeader, data, &pTxMailbox)!= HAL_OK)
		return J1939_ERROR;
	return J1939_OK;
}
