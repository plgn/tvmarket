#ifndef REGISTERS_H_
#define REGISTERS_H_

#define GPIO_SIZE 16

#define REG_COIL_START				0x000
#define REG_COIL_NREGS				(GPIO_SIZE * 4 + 0)

#define RELAY_1						GPIO_SIZE * 3 + 0
#define RELAY_2						GPIO_SIZE * 3 + 1
#define RELAY_3						GPIO_SIZE * 3 + 2

#define REG_DISCRETE_START			0x100
#define REG_DISCRETE_NREGS			(GPIO_SIZE * 2 + 1)

#define REG_HOLDING_START			0x200
#define REG_HOLDING_NREGS			10

#define REG_INPUT_START				0x300
#define REG_INPUT_TEMP              0
#define REG_INPUT_HUMI              2
#define REG_INPUT_PRES              4
#define REF_INPUT_GAS               6
#define REG_INPUT_NREGS				8

void registers_set_temperature(int32_t temp);
void registers_set_humidity(uint32_t humi);
void registers_set_pressure(uint32_t pres);
void registers_set_gas(uint32_t gas);

#endif
