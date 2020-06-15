#include <port.h>
#include <mbport.h>
#include <periph.h>

extern UART_HandleTypeDef huart1;
uint8_t receive_buf[1];

uint32_t test_var = 0;

BOOL
xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity) {
    /*
    (void)ucPORT;

    huart1.Instance                    = MODBUS_USART;
    huart1.Init.BaudRate               = ulBaudRate;

    switch (ucDataBits) {
        case 8:  huart1.Init.WordLength = UART_WORDLENGTH_8B; break;
        case 9:  huart1.Init.WordLength = UART_WORDLENGTH_9B; break;
        default: huart1.Init.WordLength = UART_WORDLENGTH_8B; break;
    }

    huart1.Init.StopBits               = UART_STOPBITS_1;

    switch (eParity) {
        case MB_PAR_NONE: huart1.Init.Parity = UART_PARITY_NONE; break;
        case MB_PAR_EVEN: huart1.Init.Parity = UART_PARITY_EVEN; break;
        case MB_PAR_ODD:  huart1.Init.Parity = UART_PARITY_ODD;  break;
        default:          huart1.Init.Parity = UART_PARITY_NONE; break;
    }

    huart1.Init.Mode                   = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_RS485Ex_Init(&huart1, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK) {
        return FALSE;
    }

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    */

    return TRUE;
}

void
vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable) {
    // test_var = 1;
    if(xRxEnable) {
        // test_var += 2;
        HAL_UART_Receive_IT(&huart1, receive_buf, 1);
    }

    if (xRxEnable && !xTxEnable) {
        /*
        while (!__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE));
        __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
        while (!__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC));

        __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
        */
        HAL_GPIO_WritePin(MODBUS_DE_PORT, MODBUS_DE_PIN, GPIO_PIN_RESET);
    }

    if (xTxEnable && !xRxEnable) {
        // __HAL_UART_DISABLE_IT(&huart1, RT_IT_RXNE);
        HAL_GPIO_WritePin(MODBUS_DE_PORT, MODBUS_DE_PIN, GPIO_PIN_SET);
        // __HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
    }

    /*
    if (!xRxEnable && !xTxEnable) {
        while (!__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE));
        __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
        while (!__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC));

        __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
    }
    */
}

BOOL
xMBPortSerialPutByte( CHAR ucByte ) {
    uint8_t buffer[] = {(uint8_t)ucByte};
    HAL_UART_Transmit_IT(&huart1, buffer, 1);

    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte ) {
    *pucByte = (CHAR) receive_buf[0];
    return TRUE;
}

/*
void USART1_IRQHandler(void) {
    if((__HAL_UART_GET_IT(&huart1, UART_IT_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_RXNE) != RESET)) {
        pxMBFrameCBByteReceived();
        __HAL_UART_SEND_REQ(&huart1, UART_RXDATA_FLUSH_REQUEST);
        return;
    }
    if((__HAL_UART_GET_IT(&huart1, UART_IT_TXE) != RESET) &&(__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_TXE) != RESET)) {
        pxMBFrameCBTransmitterEmpty();
        return;
    }

    HAL_UART_IRQHandler(&huart1);
}
*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    if (huart == &huart1) {
        // printf("%02X", receive_buf[0]);
        HAL_UART_Receive_IT(&huart1, receive_buf, 1);
        pxMBFrameCBByteReceived();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {

}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
    if (huart == &huart1) {
        pxMBFrameCBTransmitterEmpty();
    }
}
