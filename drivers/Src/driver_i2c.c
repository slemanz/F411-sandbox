#include "driver_i2c.h"
#include "driver_clock.h" 

uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
uint16_t APB1_PreScaler[8] = {2, 4, 8, 16};

/*********************************************************************
 * Private Helper Functions (Internal Use Only)
 *********************************************************************/

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
    pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

static void I2C_ClearADDRFlag(I2C_RegDef_t *pI2Cx)
{
    // Sequence to clear ADDR: Read SR1 -> Read SR2
    uint32_t dummy_read;
    dummy_read = pI2Cx->SR1;
    dummy_read = pI2Cx->SR2;
    (void)dummy_read;
}

void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);

void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1); // SlaveAddr is Slave address + r/nw bit = 0
	pI2Cx->DR = SlaveAddr;
}

void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= 1; // SlaveAddr is Slave address + r/nw bit = 1
	pI2Cx->DR = SlaveAddr;
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
    tempreg |= (pclk1 / 1000000U) & 0x3F;
    pI2CConfig->pI2Cx->CR2 = tempreg;

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

    I2C_ManageAcking(pI2CConfig->pI2Cx, ENABLE);
}

void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{

}

/*
 * Data send and receive
 */

void I2C_MasterSendData(I2C_RegDef_t *pI2Cx, uint8_t *pTxbuffer, uint32_t Len, uint8_t Sr)
{
    I2C_GenerateStartCondition(pI2Cx);

    while( !I2C_GetFlagStatus(pI2Cx, I2C_FLAG_SB) );  // Wait until SB (Start Bit) is set

    uint8_t slaveAddr = pTxbuffer[0];
    pI2Cx->DR = slaveAddr;

    while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ADDR));

    if(slaveAddr & 0x01)
    {
        // It is a READ command. We MUST NOT clear the ADDR flag here.
        // The hardware is now stretching the clock (SCL Low).
        // We yield control to the Receive function.
        return;
    }

    I2C_ClearADDRFlag(pI2Cx);

    for(uint32_t i = 1; i < Len; i++)
    {
        // Wait until TXE is set (Data Register Empty)
        while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_TXE));
        pI2Cx->DR = pTxbuffer[i];
    }

    // 8. Wait for TXE=1 and BTF=1 (Byte Transfer Finished)
    while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_TXE));
    while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_BTF));

    // 9. Generate STOP condition (if Repeated Start is not requested)
    if(Sr == I2C_DISABLE_SR)
    {
        I2C_GenerateStopCondition(pI2Cx);
    }
}

void I2C_MasterReceiveData(I2C_RegDef_t *pI2Cx, uint8_t *pRxBuffer, uint8_t Len, uint8_t Sr)
{
    // The BUS is currently held in Low by the hardware (ADDR flag is SET)
    
    if(Len == 1)
    {
        // Disable Acking BEFORE clearing ADDR
        I2C_ManageAcking(pI2Cx, I2C_ACK_DISABLE);

        I2C_ClearADDRFlag(pI2Cx);

        if(Sr == I2C_DISABLE_SR)
        {
            I2C_GenerateStopCondition(pI2Cx);
        }

        while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_RXNE) );

        *pRxBuffer = pI2Cx->DR; // Read data
    }
    else if(Len > 1)
    {
        // 1. Clear ADDR flag to start reception
        I2C_ClearADDRFlag(pI2Cx);

        for(uint32_t i = 0; i < Len; i++)
        {
            // If we are at the 2nd to last byte (Len-2)
            if(i == (Len - 2))
            {
                while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_BTF));

                // Disable ACK
                I2C_ManageAcking(pI2Cx, I2C_ACK_DISABLE);

                // Read Data N-2
                pRxBuffer[i] = pI2Cx->DR;
                i++; // Increment index for next byte

                // Generate STOP
                if(Sr == I2C_DISABLE_SR)
                {
                    I2C_GenerateStopCondition(pI2Cx);
                }

                // Read Data N-1 (Last Byte)
                pRxBuffer[i] = pI2Cx->DR;
            }
            else
            {
                // Normal Read
                while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_RXNE));
                pRxBuffer[i] = pI2Cx->DR;
            }
        }
    }

    // Re-enable ACK for next transmission
    I2C_ManageAcking(pI2Cx, I2C_ACK_ENABLE);
}