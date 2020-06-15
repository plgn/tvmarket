#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "modbus.h"
#include "registers.h"

extern UART_HandleTypeDef huart1;

// pin init helper
typedef enum {
    GpioModeInput,
    GpioModeOutput,
} GpioMode;

typedef struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} GpioPin;

typedef struct {
    GpioPin gpio;
    GpioPin pull;
} IoLine;

void app_gpio_init(GpioPin gpio, GpioMode mode) {
    if(gpio.pin != 0) {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.Pin = gpio.pin;
        GPIO_InitStruct.Pull = GPIO_NOPULL;

        switch(mode) {
            case GpioModeInput:
                GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            break;

            case GpioModeOutput: 
                GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
            break;
        }

        HAL_GPIO_Init(gpio.port, &GPIO_InitStruct);
    }
}

void app_gpio_write(GpioPin gpio, bool state) {
    if(gpio.pin != 0) {
        HAL_GPIO_WritePin(gpio.port, gpio.pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

bool app_gpio_read(GpioPin gpio) {
    return (HAL_GPIO_ReadPin(gpio.port, gpio.pin) == GPIO_PIN_SET);
}

IoLine BOARD_PINS[GPIO_SIZE + 1] = {
    // dummy IO0
    {.gpio = {.pin = 0, .port = (GPIO_TypeDef*)0}, .pull = {.pin = 0, .port = (GPIO_TypeDef*)0}},

    {.gpio = {.port = GPIOA, .pin = GPIO_PIN_0}, .pull = {.port = GPIOA, .pin = GPIO_PIN_1}},
    {.gpio = {.port = GPIOC, .pin = GPIO_PIN_2}, .pull = {.port = GPIOC, .pin = GPIO_PIN_3}},
    {.gpio = {.port = GPIOC, .pin = GPIO_PIN_0}, .pull = {.port = GPIOC, .pin = GPIO_PIN_1}},
    {.gpio = {.port = GPIOC, .pin = GPIO_PIN_14}, .pull = {.port = GPIOC, .pin = GPIO_PIN_15}},

    {.gpio = {.port = GPIOB, .pin = GPIO_PIN_9}, .pull = {.port = GPIOC, .pin = GPIO_PIN_13}},
    {.gpio = {.port = GPIOB, .pin = GPIO_PIN_7}, .pull = {.port = GPIOB, .pin = GPIO_PIN_8}},
    {.gpio = {.port = GPIOB, .pin = GPIO_PIN_5}, .pull = {.port = GPIOB, .pin = GPIO_PIN_6}},
    {.gpio = {.port = GPIOB, .pin = GPIO_PIN_3}, .pull = {.port = GPIOB, .pin = GPIO_PIN_4}},

    {.gpio = {.port = GPIOC, .pin = GPIO_PIN_12}, .pull = {.port = GPIOD, .pin = GPIO_PIN_2}},
    {.gpio = {.pin = 0, .port = (GPIO_TypeDef*)0}, .pull = {.pin = 0, .port = (GPIO_TypeDef*)0}},
    {.gpio = {.port = GPIOA, .pin = GPIO_PIN_11}, .pull = {.port = GPIOA, .pin = GPIO_PIN_15}},
    {.gpio = {.port = GPIOC, .pin = GPIO_PIN_9}, .pull = {.port = GPIOA, .pin = GPIO_PIN_8}},

    {.gpio = {.port = GPIOC, .pin = GPIO_PIN_7}, .pull = {.port = GPIOC, .pin = GPIO_PIN_8}},
    {.gpio = {.port = GPIOB, .pin = GPIO_PIN_15}, .pull = {.port = GPIOC, .pin = GPIO_PIN_6}},
    {.gpio = {.port = GPIOB, .pin = GPIO_PIN_13}, .pull = {.port = GPIOB, .pin = GPIO_PIN_14}},
    {.gpio = {.port = GPIOB, .pin = GPIO_PIN_1}, .pull = {.port = GPIOB, .pin = GPIO_PIN_12}},
};

void set_coil(uint8_t index, uint8_t state) {
    index -= 1;
    // printf("index %d\n", index);

    if(index < GPIO_SIZE) {
        printf("set mode %d to %d\n", index, state);
        app_gpio_init(BOARD_PINS[index + 1].gpio, state == 1 ? GpioModeOutput : GpioModeInput);
    }

    if(index >= GPIO_SIZE && index < GPIO_SIZE * 2) {
        printf("write %d to %d\n", (index - GPIO_SIZE), state);
        app_gpio_write(BOARD_PINS[(index - GPIO_SIZE) + 1].gpio, state == 1);
    }

    if(index >= GPIO_SIZE * 2 && index < GPIO_SIZE * 3) {
        uint8_t pin = (index - GPIO_SIZE * 2) + 1;
        printf("pull %d to %d\n", pin, state);
        app_gpio_write(BOARD_PINS[pin].pull, state == 1);
    }

    if(index == RELAY_1) {
        HAL_GPIO_WritePin(
            RELAY_C1_GPIO_Port, RELAY_C1_Pin,
            state == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET
        );
    }

    if(index == RELAY_2) {
        HAL_GPIO_WritePin(
            RELAY_C2_GPIO_Port, RELAY_C2_Pin,
            state == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET
        );
    }

    if(index == RELAY_3) {
        HAL_GPIO_WritePin(
            RELAY_C3_GPIO_Port, RELAY_C3_Pin,
            state == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET
        );
    }
}

static bool lock_en[GPIO_SIZE] = {false};
static bool lock_dis[GPIO_SIZE] = {false};

uint8_t get_discrete(uint8_t index) {
    uint8_t res = 0;

    index -= 1;

    if(index < GPIO_SIZE) {
        res = lock_en[index] ? 1 : 0;

        lock_en[index] = app_gpio_read(BOARD_PINS[index + 1].gpio);
    }

    if(index >= GPIO_SIZE && index < GPIO_SIZE * 2) {
        res = lock_dis[index - GPIO_SIZE] ? 1 : 0;

        lock_dis[index - GPIO_SIZE] = app_gpio_read(BOARD_PINS[(index - GPIO_SIZE) + 1].gpio);
    }

    printf("get %d = %d\n", index, res);

    return res;
}

void poll_inputs() {
    for(size_t i = 0; i < GPIO_SIZE; i++) {
        if(app_gpio_read(BOARD_PINS[i + 1].gpio)) {
            // printf("poll %d t\n", i + 1);
            lock_en[i] = true;
        } else {
            // printf("poll %d f\n", i + 1);
            lock_dis[i] = false;
        }
    }
}

osThreadId ledTaskHandle;
void led_task(void const * argument);

extern uint8_t receive_buf[1];

void app() {
    printf("=== Endpoint ASC B4CKSP4CE ===\n");

    for(size_t i = 1; i < sizeof(BOARD_PINS)/sizeof(BOARD_PINS[0]); i++) {
        app_gpio_init(BOARD_PINS[i].pull, GpioModeOutput);
        app_gpio_write(BOARD_PINS[i].pull, false);
    }

    // osThreadDef(ledTask, led_task, osPriorityNormal, 0, 128);
    // ledTaskHandle = osThreadCreate(osThread(ledTask), NULL);

    modbus_init();

    while(1) {
        HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
        modbus_poll();
        poll_inputs();

        osDelay(1);
    }
}

extern uint32_t test_var;

void led_task(void const* argument) {
    int32_t temp = 0;

    for(size_t i = 1; i < sizeof(BOARD_PINS)/sizeof(BOARD_PINS[0]); i++) {
        app_gpio_init(BOARD_PINS[i].gpio, GpioModeOutput);
    }

    while(1) {
        for(size_t i = 1; i < sizeof(BOARD_PINS)/sizeof(BOARD_PINS[0]); i++) {
            app_gpio_write(BOARD_PINS[i].gpio, true);
            osDelay(10);
        }

        for(size_t i = 1; i < sizeof(BOARD_PINS)/sizeof(BOARD_PINS[0]); i++) {
            app_gpio_write(BOARD_PINS[i].gpio, false);
            osDelay(10);
        }

        registers_set_temperature(temp++);

        // printf("test: %d\n", test_var);

        /*
        vMBPortSerialEnable(0, 1);
        xMBPortSerialPutByte('a');
        osDelay(2);
        vMBPortSerialEnable(1, 0);
        */
    }
}