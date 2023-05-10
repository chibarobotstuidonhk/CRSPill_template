#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    void setup(void);
    void loop(void);

    // callback from can interrupt.
    void can_callback(CAN_RxHeaderTypeDef *RxHeader, uint8_t RxData[]);

#ifdef __cplusplus
}
#endif