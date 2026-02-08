#include "driver_i2c.h"
#include "driver_clock.h" 

uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
uint16_t APB1_PreScaler[8] = {2, 4, 8, 16};

/*********************************************************************
 * Private Helper Functions (Internal Use Only)
 *********************************************************************/

static void I2C_ClearADDRFlag(I2C_RegDef_t *pI2Cx)
{
    // Sequence to clear ADDR: Read SR1 -> Read SR2
    uint32_t dummy_read;
    dummy_read = pI2Cx->SR1;
    dummy_read = pI2Cx->SR2;
    (void)dummy_read;
}

/*********************************************************************
 * Public API Implementations
 *********************************************************************/


void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}else
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}
}

uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
    if(pI2Cx->SR1 & FlagName)
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}

void I2C_GenereteStart(I2C_RegDef_t *pI2Cx)
{
    pI2Cx->CR1 |= (1 << I2C_CR1_START);
    while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_SB));
}

void I2C_GenereteStop(I2C_RegDef_t *pI2Cx)
{
    pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

void I2C_WaitBusy(I2C_RegDef_t *pI2Cx)
{
    while((pI2Cx->SR2 & (1 << 1)));
}

void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
    }else
    {
        pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
    }
}

void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

void I2C_SendAddress(I2C_RegDef_t *pI2Cx, uint8_t address, uint8_t sendType)
{
    address = (address << 1);
    address |= (sendType << 0);

    pI2Cx->DR = address;
    while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ADDR));
    I2C_ClearADDRFlag(pI2Cx);
}


void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		}else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		}else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
	}else
	{
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		}else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		}else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}

void I2C_Init(I2C_Config_t *pI2CConfig)
{
    uint32_t tempreg = 0;

    I2C_PeriClockControl(pI2CConfig->pI2Cx, ENABLE);

    pI2CConfig->pI2Cx->CR1 |= (1 << I2C_CR1_SWRST);
    pI2CConfig->pI2Cx->CR1 &= ~(1 << I2C_CR1_SWRST);

    uint32_t pclk1 = clock_get();

    tempreg = 0;
    tempreg |= pI2CConfig->I2C_DeviceAddress << 1;
    tempreg |= (1 << 14); // Software must keep this at 1
    pI2CConfig->pI2Cx->OAR1 = tempreg;

    // Configure CCR
    uint16_t ccr_value = 0;
    tempreg = 0;

    if(pI2CConfig->I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
    {
        // Standard Mode
        ccr_value = (pclk1 / (2 * pI2CConfig->I2C_SCLSpeed));
        tempreg |= (ccr_value & 0xFFF);
    }
    else
    {
        // Fast Mode
        tempreg |= (1 << 15);
        tempreg |= (pI2CConfig->I2C_FMDutyCycle << 14);
        if(pI2CConfig->I2C_FMDutyCycle == I2C_FM_DUTY_2)
            ccr_value = (pclk1 / (3 * pI2CConfig->I2C_SCLSpeed));
        else
            ccr_value = (pclk1 / (25 * pI2CConfig->I2C_SCLSpeed));
        
        tempreg |= (ccr_value & 0xFFF);
    }
    pI2CConfig->pI2Cx->CCR = tempreg;

    // Configure TRISE
    if(pI2CConfig->I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
    {
        tempreg = (pclk1 / 1000000U) + 1;
    }
    else
    {
        tempreg = ((pclk1 * 300) / 1000000U) + 1;
    }
    
    //pI2CConfig->pI2Cx->TRISE = (tempreg & 0x3F);    
    pI2CConfig->pI2Cx->TRISE = 0x21;
}

void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{

}

/*
 * Data send and receive
 */

void I2C_MasterSendData(I2C_RegDef_t *pI2Cx, uint8_t *pTxbuffer, uint32_t Len, uint8_t Sr)
{
    if(Sr == I2C_DISABLE_SR)
    {
        I2C_GenerateStopCondition(pI2Cx);
    }
}

void I2C_MasterReceiveData(I2C_RegDef_t *pI2Cx, uint8_t *pRxBuffer, uint8_t Len, uint8_t Sr)
{
}


void I2C_Send(I2C_RegDef_t *pI2Cx, uint8_t *pTxbuffer, uint32_t Len)
{
    for(uint32_t i = 0; i < Len; i++)
    {
        pI2Cx->DR = pTxbuffer[i];
        while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_TXE));
    }
    while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_BTF));
}