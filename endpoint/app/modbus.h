#ifndef MODBUS_H_
#define MODBUS_H_

#include <stdbool.h>
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

bool modbus_init();
void modbus_poll();

#endif
