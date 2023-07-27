#pragma once

#include <stdint.h>
#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void setup(void);
    void loop(void);

    // callback from can interrupt.
    void can_callback(CAN_RxHeaderTypeDef * RxHeader, const uint8_t * RxData);

#ifdef __cplusplus
}
#endif