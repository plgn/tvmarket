#include <mb.h>
#include "registers.h"
#include <string.h>
#include <stdio.h>

static unsigned short usRegCoilStart = REG_COIL_START;
static unsigned char  usRegCoilBuf[REG_COIL_NREGS];

static unsigned short usRegDiscreteStart = REG_DISCRETE_START;

static unsigned short usRegHoldingStart = REG_HOLDING_START;
static unsigned short usRegHoldingBuf[REG_HOLDING_NREGS];

static unsigned short usRegInputStart = REG_INPUT_START;
static unsigned short usRegInputBuf[REG_INPUT_NREGS];

void registers_set_temperature(int32_t temp) {
	memcpy(&usRegInputBuf[REG_INPUT_TEMP], &temp, 4);
}

void registers_set_humidity(uint32_t humi) {
	memcpy(&usRegInputBuf[REG_INPUT_HUMI], &humi, 4);
}

void registers_set_pressure(uint32_t pres) {
	memcpy(&usRegInputBuf[REG_INPUT_PRES], &pres, 4);
}

void registers_set_gas(uint32_t gas) {
	memcpy(&usRegInputBuf[REF_INPUT_GAS], &gas, 4);
}

void set_coil(uint8_t index, uint8_t state);
uint8_t get_discrete(uint8_t index);

eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    printf("set coil %d\n", usAddress);

    if ((usAddress >= REG_COIL_START)
        && (usAddress + usNCoils <= REG_COIL_START + REG_COIL_NREGS))
    {
        iRegIndex = (int)(usAddress - usRegCoilStart);

        size_t shift = 0;

        while	(usNCoils > 0)
        {
            // printf("coil %d [%02X]\n", shift, pucRegBuffer[shift / 8]);
        	if (eMode == MB_REG_READ) {
        		pucRegBuffer[shift / 8] |= (unsigned char)(usRegCoilBuf[iRegIndex]) << (shift % 8);
        	} else {
        		usRegCoilBuf[iRegIndex] = (pucRegBuffer[shift / 8] & (1 << (shift % 8)) ? 1 : 0);
        	}

            set_coil(iRegIndex, usRegCoilBuf[iRegIndex]);

            iRegIndex++;
            usNCoils--;
            shift++;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if ((usAddress >= REG_DISCRETE_START)
        && (usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_NREGS))
    {
        iRegIndex = (int)(usAddress - usRegDiscreteStart);

        size_t shift = 0;

        for(size_t i = 0; i < usNDiscrete; i++) {
            pucRegBuffer[i] = 0;
        }

        while	(usNDiscrete > 0)
        {
            pucRegBuffer[shift / 8] |= (unsigned char)get_discrete(iRegIndex) << (shift % 8);
            printf("[%d] <- %02X\n", shift / 8, pucRegBuffer[shift / 8]);
            
            iRegIndex++;
            usNDiscrete--;
            shift++;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if ((usAddress >= REG_HOLDING_START)
        && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        iRegIndex = (int)(usAddress - usRegHoldingStart);

        while (usNRegs > 0)
        {
        	if (eMode == MB_REG_READ) {
                *pucRegBuffer++ = (unsigned char)(usRegHoldingBuf[iRegIndex] >> 8);
                *pucRegBuffer++ = (unsigned char)(usRegHoldingBuf[iRegIndex] & 0xFF);
        	} else {
        		usRegHoldingBuf[iRegIndex] = 0;
        		usRegHoldingBuf[iRegIndex] |= (*pucRegBuffer++ << 8);
        		usRegHoldingBuf[iRegIndex] |= (*pucRegBuffer++ & 0xFF);
        	}

            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( (usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = (unsigned char)(usRegInputBuf[iRegIndex] >> 8);
            *pucRegBuffer++ = (unsigned char)(usRegInputBuf[iRegIndex] & 0xFF);
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}
