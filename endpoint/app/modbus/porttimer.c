#include <port.h>
#include <mbport.h>
#include <periph.h>

extern TIM_HandleTypeDef    htim4;
static uint16_t timeout     = 0;
static uint16_t downcounter = 0;

BOOL
xMBPortTimersInit(USHORT usTim1Timerout50us) {
	htim4.Instance               = MODBUS_TIMER;
	htim4.Init.Prescaler         = (HAL_RCC_GetPCLK1Freq() / 1000000) - 1;
	htim4.Init.CounterMode       = TIM_COUNTERMODE_UP;
	htim4.Init.Period            = 50 - 1;
	htim4.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	timeout = usTim1Timerout50us;

	if (!HAL_TIM_Base_Init(&htim4) == HAL_OK) {
		return FALSE;
	}

	HAL_NVIC_SetPriority(TIM4_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);

	return TRUE;
}

inline void
vMBPortTimersEnable()
{
	downcounter = timeout;
	HAL_TIM_Base_Start_IT(&htim4);
}

inline void
vMBPortTimersDisable() {
	HAL_TIM_Base_Stop_IT(&htim4);
}

extern uint32_t test_var;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance != MODBUS_TIMER) {
		return;
	}

	if (!--downcounter) {
		test_var++;
		pxMBPortCBTimerExpired();
	}
}

void TIM4_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim4);
}
