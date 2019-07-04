/* App_J1939.c
 *
 *  Created on: 09-Feb-2019
 *  Author: Altamash Abdul Rahim
 *  Description: Application File for J1939 based communication.
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

#include "App_J1939.h"
#include "stdio.h"
#include "string.h"

extern TIM_HandleTypeDef htim7;

/*Global structure definitions*/
extern volatile RSA_params_t g_rsaParam;
extern volatile uint32_t uwTick;
volatile CanJ1939HandleTypeDef_t g_canJ1939_t;
volatile uint8_t Authenticated = 0; /*Used as Authentication Flag*/


void SetTick(uint32_t tickVal)
{uwTick = tickVal;}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	CAN_J1939StackTick((CanJ1939HandleTypeDef_t*)&g_canJ1939_t);
}

/*This Function is Called from lower layer CAN data callback which informs if authentication was successfull or not.
 * Called within Timer ISR*/
void CAN_AuthenticationCallback(uint8_t data)
{
 if(data)
	 Authenticated = 1;
 HAL_TIM_Base_Stop_IT(&htim7);
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
	RSA_CryptoInit((RSA_params_t*)&g_rsaParam, PRIME_NO_1, PRIME_NO_2);
#if BMS
	CAN_DeviceSync();
	HAL_Delay(10);
	print_string("RSA Parameters:\nN: %d\nE: %d\nD: %d\nPHI:%d\n", g_rsaParam.n, g_rsaParam.e, g_rsaParam.d, g_rsaParam.phi);
	if(CAN_AuthenticatePeer(RSA_encryptData((RSA_params_t*)&g_rsaParam, INITIAL_AUTH_TOKEN)) == 0)
		print_string("Authentication Started\n");

#endif
}


void App_Process(void)
{
/*  print_string("Tick: %d\n", HAL_GetTick());
  HAL_Delay(500);*/
  if(Authenticated)
  {
	  static uint32_t lastTick = 0;
	  uint32_t NowTick = HAL_GetTick();
	  if(NowTick - lastTick >= 2000)
	  {
		  print_string("Tick: %d\n", HAL_GetTick());
		  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
		  lastTick = NowTick;
	  }
  }
}
