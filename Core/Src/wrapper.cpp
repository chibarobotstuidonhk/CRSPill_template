/*
 * wrapper.cpp
 *
 *  Created on: May 9, 2023
 *      Author: taman
 */

#include "can.h"
#include "main.h"
#include "wapper.hpp"

//[注意]EMS PinはInputが想定されていそうですが、動作確認用にOutputとして設定してあります。用途に合わせて変更してください。

uint8_t TxData[8];
CAN_TxHeaderTypeDef TxHeader;
uint32_t TxMailbox;

//CANを受信したときに呼ばれるコールバックです。割り込みです。
void can_callback(CAN_RxHeaderTypeDef *RxHeader, uint8_t RxData[]){
	HAL_GPIO_TogglePin(CAN_LED_GPIO_Port, CAN_LED_Pin);
	for(int i = 0;i<8;i++){
		TxData[i] = RxData[i];
	}
}

//起動時に１度だけ呼ばれます。初期化をします。
void setup(){
	//LEDをチカチカ
	for(int i = 0;i<3;i++){
		HAL_GPIO_WritePin(CAN_LED_GPIO_Port, CAN_LED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EMS_LED_GPIO_Port, EMS_LED_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(CAN_LED_GPIO_Port, CAN_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EMS_LED_GPIO_Port, EMS_LED_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
	}
    TxData[0] = 0x11;
    TxData[1] = 0x22;
    TxData[2] = 0x33;
    TxData[3] = 0x44;
    TxData[4] = 0x55;
    TxData[5] = 0x66;
    TxData[6] = 0x77;
    TxData[7] = 0x88;

    TxHeader.StdId = 0x0;                 // CAN ID
    TxHeader.RTR = CAN_RTR_DATA;            // フレームタイプはデータフレーム
    TxHeader.IDE = CAN_ID_STD;              // 標準ID(11ﾋﾞｯﾄ)
    TxHeader.DLC = 8;                       // データ長は8バイトに
    TxHeader.TransmitGlobalTime = DISABLE;

    //CANの初期化です。この後からCANの送受信が行われます。
	can_setup();
}


//ループです。実行中は呼ばれ続けます。
void loop(){
	//mailboxが一杯でなければCANを送信します。
	if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan)){
		TxHeader.StdId++;
		if(TxHeader.StdId > 10) TxHeader.StdId = 0x0;
		HAL_GPIO_TogglePin(EMS_LED_GPIO_Port,EMS_LED_Pin);
	    HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
	}
	//ちょっと待ちます。
	HAL_Delay(10);
}
