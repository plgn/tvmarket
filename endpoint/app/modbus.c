#include "modbus.h"

#include <mb.h>
#include "periph.h"

bool modbus_init() {
    if (eMBInit(MB_RTU, MODBUS_ADDRESS, 0, 115200, MB_PAR_NONE) != MB_ENOERR) {
        return false;
    }

    if(eMBEnable() != MB_ENOERR) {
        return false;
    }

    return true;
}

void modbus_poll() {
    eMBPoll();
}

static void _modbus_gpio_init() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin   = MODBUS_DE_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MODBUS_DE_PORT, &GPIO_InitStruct);
}
