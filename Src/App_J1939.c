/*
 * App_J1939.c
 *
 *  Created on: 09-Feb-2019
 *      Author: Altamash Abdul Rahim
 */
#include "App_J1939.h"
#include "stdio.h"
#include "string.h"


#define BMS	0
/*Global structure definitions*/
RSA_params g_RsaParam;
__IO CanHandleTypeDef canJ1939;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	CAN_J1939StackTick((CanHandleTypeDef*)&canJ1939);
}
void App_Init(void)
{
	if(CAN_TransportLayerInit() == J1939_OK)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, GPIO_PIN_SET);
		print_string("Transport Layer Initialized\r\n");
	}
	else
		print_string("Transport Layer Initialization Error\r\n");
	CryptoInit(&g_RsaParam, PRIME_NO_1, PRIME_NO_2);
#if BMS
	print_string("RSA Parameters:\nn: %d\ne: %d\nd: %d\nphi:%d\n", g_RsaParam.n, g_RsaParam.e, g_RsaParam.d, g_RsaParam.phi);
	if(CAN_AuthenticatePeer(encrypt(&g_RsaParam, 1235)) == 0)
		print_string("Authentication Started\n");
#endif
}


void App_Process(void)
{

}
