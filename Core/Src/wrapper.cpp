/*
 * wrapper.cpp
 *
 *  Created on: May 9, 2023
 *      Author: taman
 */

//[注意]EMS PinはInputが想定されていそうですが、動作確認用にOutputとして設定してあります。用途に合わせて変更してください。

#include <stdint.h>
#include "can.h"
#include "main.h"

struct CanCallbackState
{
	uint8_t rxBuffer[8]{};
} canCallbackState{};

//CANを受信したときに呼ばれるコールバックです。割り込みです。
extern "C" void can_callback(CAN_RxHeaderTypeDef * RxHeader, const uint8_t * RxData){
	(void)RxHeader;

	HAL_GPIO_TogglePin(CAN_LED_GPIO_Port, CAN_LED_Pin);
	for(int i = 0; i < 8; i++){
		canCallbackState.rxBuffer[i] = RxData[i];
	}
}

/* Stew:
これ、TxHeaderとTxMailboxはsetupとloopを分けるためだけにグローバル変数になっている。
CRSPill_templateのユーザーに陽にメインループを書かせ、グローバルな変数を減らすべきでは。
これはArduinoではないし、なんなら同上の理由でArduinoのsetup/loopも邪悪だと思う。
(Arduinoのグローバル変数はスタックオーバーフローの検出に役立つかもしれないが、
STM32ではそこまでローカル変数を忌避する必要もないだろう。それ以前にやるべきことがあるはずだ)
*/
struct Global
{
	CAN_TxHeaderTypeDef TxHeader;
};

//起動時に１度だけ呼ばれます。初期化をします。
extern "C" Global setup(){
	//LEDをチカチカ
	for(int i = 0; i < 3; i++){
		HAL_GPIO_WritePin(CAN_LED_GPIO_Port, CAN_LED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EMS_LED_GPIO_Port, EMS_LED_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(CAN_LED_GPIO_Port, CAN_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EMS_LED_GPIO_Port, EMS_LED_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
	}
	canCallbackState.rxBuffer[0] = 0x11;
	canCallbackState.rxBuffer[1] = 0x22;
	canCallbackState.rxBuffer[2] = 0x33;
	canCallbackState.rxBuffer[3] = 0x44;
	canCallbackState.rxBuffer[4] = 0x55;
	canCallbackState.rxBuffer[5] = 0x66;
	canCallbackState.rxBuffer[6] = 0x77;
	canCallbackState.rxBuffer[7] = 0x88;

	CAN_TxHeaderTypeDef TxHeader;
	TxHeader.StdId = 0x0;  // CAN ID
	TxHeader.RTR = CAN_RTR_DATA;  // フレームタイプはデータフレーム
	TxHeader.IDE = CAN_ID_STD;  // 標準ID(11ﾋﾞｯﾄ)
	TxHeader.DLC = 8;  // データ長は8バイトに
	TxHeader.TransmitGlobalTime = DISABLE;

	//CANの初期化です。この後からCANの送受信が行われます。
	can_setup();

	return Global{TxHeader};
}


//ループです。実行中は呼ばれ続けます。
extern "C" void loop(Global global){
	//mailboxが一杯でなければCANを送信します。
	if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan)){
		global.TxHeader.StdId++;
		if(global.TxHeader.StdId > 10) global.TxHeader.StdId = 0x0;
		HAL_GPIO_TogglePin(EMS_LED_GPIO_Port,EMS_LED_Pin);
		uint32_t TxMailbox;
		HAL_CAN_AddTxMessage(&hcan, &global.TxHeader, canCallbackState.rxBuffer, &TxMailbox);
	}
	//ちょっと待ちます。
	HAL_Delay(10);
}
