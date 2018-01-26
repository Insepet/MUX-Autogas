/*******************************************************************************
* File Name: LCD_2.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LCD_2.h"
#if (LCD_2_INTERNAL_CLOCK_USED)
    #include "LCD_2_IntClock.h"
#endif /* End LCD_2_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 LCD_2_initVar = 0u;

#if (LCD_2_TX_INTERRUPT_ENABLED && LCD_2_TX_ENABLED)
    volatile uint8 LCD_2_txBuffer[LCD_2_TX_BUFFER_SIZE];
    volatile uint8 LCD_2_txBufferRead = 0u;
    uint8 LCD_2_txBufferWrite = 0u;
#endif /* (LCD_2_TX_INTERRUPT_ENABLED && LCD_2_TX_ENABLED) */

#if (LCD_2_RX_INTERRUPT_ENABLED && (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED))
    uint8 LCD_2_errorStatus = 0u;
    volatile uint8 LCD_2_rxBuffer[LCD_2_RX_BUFFER_SIZE];
    volatile uint8 LCD_2_rxBufferRead  = 0u;
    volatile uint8 LCD_2_rxBufferWrite = 0u;
    volatile uint8 LCD_2_rxBufferLoopDetect = 0u;
    volatile uint8 LCD_2_rxBufferOverflow   = 0u;
    #if (LCD_2_RXHW_ADDRESS_ENABLED)
        volatile uint8 LCD_2_rxAddressMode = LCD_2_RX_ADDRESS_MODE;
        volatile uint8 LCD_2_rxAddressDetected = 0u;
    #endif /* (LCD_2_RXHW_ADDRESS_ENABLED) */
#endif /* (LCD_2_RX_INTERRUPT_ENABLED && (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED)) */


/*******************************************************************************
* Function Name: LCD_2_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  LCD_2_Start() sets the initVar variable, calls the
*  LCD_2_Init() function, and then calls the
*  LCD_2_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The LCD_2_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time LCD_2_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the LCD_2_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD_2_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(LCD_2_initVar == 0u)
    {
        LCD_2_Init();
        LCD_2_initVar = 1u;
    }

    LCD_2_Enable();
}


/*******************************************************************************
* Function Name: LCD_2_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call LCD_2_Init() because
*  the LCD_2_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_2_Init(void) 
{
    #if(LCD_2_RX_ENABLED || LCD_2_HD_ENABLED)

        #if (LCD_2_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(LCD_2_RX_VECT_NUM, &LCD_2_RXISR);
            CyIntSetPriority(LCD_2_RX_VECT_NUM, LCD_2_RX_PRIOR_NUM);
            LCD_2_errorStatus = 0u;
        #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */

        #if (LCD_2_RXHW_ADDRESS_ENABLED)
            LCD_2_SetRxAddressMode(LCD_2_RX_ADDRESS_MODE);
            LCD_2_SetRxAddress1(LCD_2_RX_HW_ADDRESS1);
            LCD_2_SetRxAddress2(LCD_2_RX_HW_ADDRESS2);
        #endif /* End LCD_2_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        LCD_2_RXBITCTR_PERIOD_REG = LCD_2_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        LCD_2_RXSTATUS_MASK_REG  = LCD_2_INIT_RX_INTERRUPTS_MASK;
    #endif /* End LCD_2_RX_ENABLED || LCD_2_HD_ENABLED*/

    #if(LCD_2_TX_ENABLED)
        #if (LCD_2_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(LCD_2_TX_VECT_NUM, &LCD_2_TXISR);
            CyIntSetPriority(LCD_2_TX_VECT_NUM, LCD_2_TX_PRIOR_NUM);
        #endif /* (LCD_2_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (LCD_2_TXCLKGEN_DP)
            LCD_2_TXBITCLKGEN_CTR_REG = LCD_2_BIT_CENTER;
            LCD_2_TXBITCLKTX_COMPLETE_REG = ((LCD_2_NUMBER_OF_DATA_BITS +
                        LCD_2_NUMBER_OF_START_BIT) * LCD_2_OVER_SAMPLE_COUNT) - 1u;
        #else
            LCD_2_TXBITCTR_PERIOD_REG = ((LCD_2_NUMBER_OF_DATA_BITS +
                        LCD_2_NUMBER_OF_START_BIT) * LCD_2_OVER_SAMPLE_8) - 1u;
        #endif /* End LCD_2_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (LCD_2_TX_INTERRUPT_ENABLED)
            LCD_2_TXSTATUS_MASK_REG = LCD_2_TX_STS_FIFO_EMPTY;
        #else
            LCD_2_TXSTATUS_MASK_REG = LCD_2_INIT_TX_INTERRUPTS_MASK;
        #endif /*End LCD_2_TX_INTERRUPT_ENABLED*/

    #endif /* End LCD_2_TX_ENABLED */

    #if(LCD_2_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        LCD_2_WriteControlRegister( \
            (LCD_2_ReadControlRegister() & (uint8)~LCD_2_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(LCD_2_PARITY_TYPE << LCD_2_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End LCD_2_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: LCD_2_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call LCD_2_Enable() because the LCD_2_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LCD_2_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void LCD_2_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        LCD_2_RXBITCTR_CONTROL_REG |= LCD_2_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        LCD_2_RXSTATUS_ACTL_REG  |= LCD_2_INT_ENABLE;

        #if (LCD_2_RX_INTERRUPT_ENABLED)
            LCD_2_EnableRxInt();

            #if (LCD_2_RXHW_ADDRESS_ENABLED)
                LCD_2_rxAddressDetected = 0u;
            #endif /* (LCD_2_RXHW_ADDRESS_ENABLED) */
        #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */
    #endif /* (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED) */

    #if(LCD_2_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!LCD_2_TXCLKGEN_DP)
            LCD_2_TXBITCTR_CONTROL_REG |= LCD_2_CNTR_ENABLE;
        #endif /* End LCD_2_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        LCD_2_TXSTATUS_ACTL_REG |= LCD_2_INT_ENABLE;
        #if (LCD_2_TX_INTERRUPT_ENABLED)
            LCD_2_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            LCD_2_EnableTxInt();
        #endif /* (LCD_2_TX_INTERRUPT_ENABLED) */
     #endif /* (LCD_2_TX_INTERRUPT_ENABLED) */

    #if (LCD_2_INTERNAL_CLOCK_USED)
        LCD_2_IntClock_Start();  /* Enable the clock */
    #endif /* (LCD_2_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: LCD_2_Stop
********************************************************************************
*
* Summary:
*  Disables the UART operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_2_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED)
        LCD_2_RXBITCTR_CONTROL_REG &= (uint8) ~LCD_2_CNTR_ENABLE;
    #endif /* (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED) */

    #if (LCD_2_TX_ENABLED)
        #if(!LCD_2_TXCLKGEN_DP)
            LCD_2_TXBITCTR_CONTROL_REG &= (uint8) ~LCD_2_CNTR_ENABLE;
        #endif /* (!LCD_2_TXCLKGEN_DP) */
    #endif /* (LCD_2_TX_ENABLED) */

    #if (LCD_2_INTERNAL_CLOCK_USED)
        LCD_2_IntClock_Stop();   /* Disable the clock */
    #endif /* (LCD_2_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED)
        LCD_2_RXSTATUS_ACTL_REG  &= (uint8) ~LCD_2_INT_ENABLE;

        #if (LCD_2_RX_INTERRUPT_ENABLED)
            LCD_2_DisableRxInt();
        #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */
    #endif /* (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED) */

    #if (LCD_2_TX_ENABLED)
        LCD_2_TXSTATUS_ACTL_REG &= (uint8) ~LCD_2_INT_ENABLE;

        #if (LCD_2_TX_INTERRUPT_ENABLED)
            LCD_2_DisableTxInt();
        #endif /* (LCD_2_TX_INTERRUPT_ENABLED) */
    #endif /* (LCD_2_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: LCD_2_ReadControlRegister
********************************************************************************
*
* Summary:
*  Returns the current value of the control register.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the control register.
*
*******************************************************************************/
uint8 LCD_2_ReadControlRegister(void) 
{
    #if (LCD_2_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(LCD_2_CONTROL_REG);
    #endif /* (LCD_2_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: LCD_2_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  LCD_2_WriteControlRegister(uint8 control) 
{
    #if (LCD_2_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       LCD_2_CONTROL_REG = control;
    #endif /* (LCD_2_CONTROL_REG_REMOVED) */
}


#if(LCD_2_RX_ENABLED || LCD_2_HD_ENABLED)
    /*******************************************************************************
    * Function Name: LCD_2_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      LCD_2_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      LCD_2_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      LCD_2_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      LCD_2_RX_STS_BREAK            Interrupt on break.
    *      LCD_2_RX_STS_OVERRUN          Interrupt on overrun error.
    *      LCD_2_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      LCD_2_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void LCD_2_SetRxInterruptMode(uint8 intSrc) 
    {
        LCD_2_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: LCD_2_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns the next byte of received data. This function returns data without
    *  checking the status. You must check the status separately.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  LCD_2_rxBuffer - RAM buffer pointer for save received data.
    *  LCD_2_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  LCD_2_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  LCD_2_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 LCD_2_ReadRxData(void) 
    {
        uint8 rxData;

    #if (LCD_2_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        LCD_2_DisableRxInt();

        locRxBufferRead  = LCD_2_rxBufferRead;
        locRxBufferWrite = LCD_2_rxBufferWrite;

        if( (LCD_2_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = LCD_2_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= LCD_2_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            LCD_2_rxBufferRead = locRxBufferRead;

            if(LCD_2_rxBufferLoopDetect != 0u)
            {
                LCD_2_rxBufferLoopDetect = 0u;
                #if ((LCD_2_RX_INTERRUPT_ENABLED) && (LCD_2_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( LCD_2_HD_ENABLED )
                        if((LCD_2_CONTROL_REG & LCD_2_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            LCD_2_RXSTATUS_MASK_REG  |= LCD_2_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        LCD_2_RXSTATUS_MASK_REG  |= LCD_2_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end LCD_2_HD_ENABLED */
                #endif /* ((LCD_2_RX_INTERRUPT_ENABLED) && (LCD_2_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = LCD_2_RXDATA_REG;
        }

        LCD_2_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = LCD_2_RXDATA_REG;

    #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: LCD_2_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the receiver status register and the software
    *  buffer overflow status.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Side Effect:
    *  All status register bits are clear-on-read except
    *  LCD_2_RX_STS_FIFO_NOTEMPTY.
    *  LCD_2_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  LCD_2_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   LCD_2_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   LCD_2_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 LCD_2_ReadRxStatus(void) 
    {
        uint8 status;

        status = LCD_2_RXSTATUS_REG & LCD_2_RX_HW_MASK;

    #if (LCD_2_RX_INTERRUPT_ENABLED)
        if(LCD_2_rxBufferOverflow != 0u)
        {
            status |= LCD_2_RX_STS_SOFT_BUFF_OVER;
            LCD_2_rxBufferOverflow = 0u;
        }
    #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: LCD_2_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. LCD_2_GetChar() is
    *  designed for ASCII characters and returns a uint8 where 1 to 255 are values
    *  for valid characters and 0 indicates an error occurred or no data is present.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  LCD_2_rxBuffer - RAM buffer pointer for save received data.
    *  LCD_2_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  LCD_2_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  LCD_2_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 LCD_2_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (LCD_2_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        LCD_2_DisableRxInt();

        locRxBufferRead  = LCD_2_rxBufferRead;
        locRxBufferWrite = LCD_2_rxBufferWrite;

        if( (LCD_2_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = LCD_2_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= LCD_2_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            LCD_2_rxBufferRead = locRxBufferRead;

            if(LCD_2_rxBufferLoopDetect != 0u)
            {
                LCD_2_rxBufferLoopDetect = 0u;
                #if( (LCD_2_RX_INTERRUPT_ENABLED) && (LCD_2_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( LCD_2_HD_ENABLED )
                        if((LCD_2_CONTROL_REG & LCD_2_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            LCD_2_RXSTATUS_MASK_REG |= LCD_2_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        LCD_2_RXSTATUS_MASK_REG |= LCD_2_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end LCD_2_HD_ENABLED */
                #endif /* LCD_2_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = LCD_2_RXSTATUS_REG;
            if((rxStatus & LCD_2_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = LCD_2_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (LCD_2_RX_STS_BREAK | LCD_2_RX_STS_PAR_ERROR |
                                LCD_2_RX_STS_STOP_ERROR | LCD_2_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        LCD_2_EnableRxInt();

    #else

        rxStatus =LCD_2_RXSTATUS_REG;
        if((rxStatus & LCD_2_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = LCD_2_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (LCD_2_RX_STS_BREAK | LCD_2_RX_STS_PAR_ERROR |
                            LCD_2_RX_STS_STOP_ERROR | LCD_2_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: LCD_2_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, returns received character and error
    *  condition.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains status and LSB contains UART RX data. If the MSB is nonzero,
    *  an error has occurred.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 LCD_2_GetByte(void) 
    {
        
    #if (LCD_2_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        LCD_2_DisableRxInt();
        locErrorStatus = (uint16)LCD_2_errorStatus;
        LCD_2_errorStatus = 0u;
        LCD_2_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | LCD_2_ReadRxData() );
    #else
        return ( ((uint16)LCD_2_ReadRxStatus() << 8u) | LCD_2_ReadRxData() );
    #endif /* LCD_2_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: LCD_2_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received bytes available in the RX buffer.
    *  * RX software buffer is disabled (RX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty RX FIFO or 1 for not empty RX FIFO.
    *  * RX software buffer is enabled: returns the number of bytes available in 
    *    the RX software buffer. Bytes available in the RX FIFO do not take to 
    *    account.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  LCD_2_rxBufferWrite - used to calculate left bytes.
    *  LCD_2_rxBufferRead - used to calculate left bytes.
    *  LCD_2_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 LCD_2_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (LCD_2_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        LCD_2_DisableRxInt();

        if(LCD_2_rxBufferRead == LCD_2_rxBufferWrite)
        {
            if(LCD_2_rxBufferLoopDetect != 0u)
            {
                size = LCD_2_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(LCD_2_rxBufferRead < LCD_2_rxBufferWrite)
        {
            size = (LCD_2_rxBufferWrite - LCD_2_rxBufferRead);
        }
        else
        {
            size = (LCD_2_RX_BUFFER_SIZE - LCD_2_rxBufferRead) + LCD_2_rxBufferWrite;
        }

        LCD_2_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((LCD_2_RXSTATUS_REG & LCD_2_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: LCD_2_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receiver memory buffer and hardware RX FIFO of all received data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD_2_rxBufferWrite - cleared to zero.
    *  LCD_2_rxBufferRead - cleared to zero.
    *  LCD_2_rxBufferLoopDetect - cleared to zero.
    *  LCD_2_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *
    *******************************************************************************/
    void LCD_2_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        LCD_2_RXDATA_AUX_CTL_REG |= (uint8)  LCD_2_RX_FIFO_CLR;
        LCD_2_RXDATA_AUX_CTL_REG &= (uint8) ~LCD_2_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (LCD_2_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        LCD_2_DisableRxInt();

        LCD_2_rxBufferRead = 0u;
        LCD_2_rxBufferWrite = 0u;
        LCD_2_rxBufferLoopDetect = 0u;
        LCD_2_rxBufferOverflow = 0u;

        LCD_2_EnableRxInt();

    #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: LCD_2_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  LCD_2__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  LCD_2__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  LCD_2__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  LCD_2__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  LCD_2__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD_2_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  LCD_2_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void LCD_2_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(LCD_2_RXHW_ADDRESS_ENABLED)
            #if(LCD_2_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* LCD_2_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = LCD_2_CONTROL_REG & (uint8)~LCD_2_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << LCD_2_CTRL_RXADDR_MODE0_SHIFT);
                LCD_2_CONTROL_REG = tmpCtrl;

                #if(LCD_2_RX_INTERRUPT_ENABLED && \
                   (LCD_2_RXBUFFERSIZE > LCD_2_FIFO_LENGTH) )
                    LCD_2_rxAddressMode = addressMode;
                    LCD_2_rxAddressDetected = 0u;
                #endif /* End LCD_2_RXBUFFERSIZE > LCD_2_FIFO_LENGTH*/
            #endif /* End LCD_2_CONTROL_REG_REMOVED */
        #else /* LCD_2_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End LCD_2_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: LCD_2_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Sets the first of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #1 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LCD_2_SetRxAddress1(uint8 address) 
    {
        LCD_2_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: LCD_2_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the second of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #2 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LCD_2_SetRxAddress2(uint8 address) 
    {
        LCD_2_RXADDRESS2_REG = address;
    }

#endif  /* LCD_2_RX_ENABLED || LCD_2_HD_ENABLED*/


#if( (LCD_2_TX_ENABLED) || (LCD_2_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: LCD_2_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   LCD_2_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   LCD_2_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   LCD_2_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   LCD_2_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void LCD_2_SetTxInterruptMode(uint8 intSrc) 
    {
        LCD_2_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: LCD_2_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data into the transmit buffer to be sent when the bus is
    *  available without checking the TX status register. You must check status
    *  separately.
    *
    * Parameters:
    *  txDataByte: data byte
    *
    * Return:
    * None.
    *
    * Global Variables:
    *  LCD_2_txBuffer - RAM buffer pointer for save data for transmission
    *  LCD_2_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  LCD_2_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  LCD_2_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void LCD_2_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(LCD_2_initVar != 0u)
        {
        #if (LCD_2_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            LCD_2_DisableTxInt();

            if( (LCD_2_txBufferRead == LCD_2_txBufferWrite) &&
                ((LCD_2_TXSTATUS_REG & LCD_2_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                LCD_2_TXDATA_REG = txDataByte;
            }
            else
            {
                if(LCD_2_txBufferWrite >= LCD_2_TX_BUFFER_SIZE)
                {
                    LCD_2_txBufferWrite = 0u;
                }

                LCD_2_txBuffer[LCD_2_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                LCD_2_txBufferWrite++;
            }

            LCD_2_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            LCD_2_TXDATA_REG = txDataByte;

        #endif /*(LCD_2_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: LCD_2_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Reads the status register for the TX portion of the UART.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the TX status register, which is cleared on read.
    *  It is up to the user to handle all bits in this return value accordingly,
    *  even if the bit was not enabled as an interrupt source the event happened
    *  and must be handled accordingly.
    *
    *******************************************************************************/
    uint8 LCD_2_ReadTxStatus(void) 
    {
        return(LCD_2_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: LCD_2_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Puts a byte of data into the transmit buffer to be sent when the bus is
    *  available. This is a blocking API that waits until the TX buffer has room to
    *  hold the data.
    *
    * Parameters:
    *  txDataByte: Byte containing the data to transmit
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD_2_txBuffer - RAM buffer pointer for save data for transmission
    *  LCD_2_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  LCD_2_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  LCD_2_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void LCD_2_PutChar(uint8 txDataByte) 
    {
    #if (LCD_2_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((LCD_2_TX_BUFFER_SIZE > LCD_2_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            LCD_2_DisableTxInt();
        #endif /* (LCD_2_TX_BUFFER_SIZE > LCD_2_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = LCD_2_txBufferWrite;
            locTxBufferRead  = LCD_2_txBufferRead;

        #if ((LCD_2_TX_BUFFER_SIZE > LCD_2_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            LCD_2_EnableTxInt();
        #endif /* (LCD_2_TX_BUFFER_SIZE > LCD_2_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(LCD_2_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((LCD_2_TXSTATUS_REG & LCD_2_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            LCD_2_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= LCD_2_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            LCD_2_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((LCD_2_TX_BUFFER_SIZE > LCD_2_MAX_BYTE_VALUE) && (CY_PSOC3))
            LCD_2_DisableTxInt();
        #endif /* (LCD_2_TX_BUFFER_SIZE > LCD_2_MAX_BYTE_VALUE) && (CY_PSOC3) */

            LCD_2_txBufferWrite = locTxBufferWrite;

        #if ((LCD_2_TX_BUFFER_SIZE > LCD_2_MAX_BYTE_VALUE) && (CY_PSOC3))
            LCD_2_EnableTxInt();
        #endif /* (LCD_2_TX_BUFFER_SIZE > LCD_2_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (LCD_2_TXSTATUS_REG & LCD_2_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                LCD_2_SetPendingTxInt();
            }
        }

    #else

        while((LCD_2_TXSTATUS_REG & LCD_2_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        LCD_2_TXDATA_REG = txDataByte;

    #endif /* LCD_2_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: LCD_2_PutString
    ********************************************************************************
    *
    * Summary:
    *  Sends a NULL terminated string to the TX buffer for transmission.
    *
    * Parameters:
    *  string[]: Pointer to the null terminated string array residing in RAM or ROM
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD_2_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void LCD_2_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(LCD_2_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                LCD_2_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: LCD_2_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Places N bytes of data from a memory array into the TX buffer for
    *  transmission.
    *
    * Parameters:
    *  string[]: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of bytes to be transmitted. The type depends on TX Buffer
    *             Size parameter.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD_2_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void LCD_2_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(LCD_2_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                LCD_2_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: LCD_2_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Writes a byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) to the transmit buffer.
    *
    * Parameters:
    *  txDataByte: Data byte to transmit before the carriage return and line feed.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD_2_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void LCD_2_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(LCD_2_initVar != 0u)
        {
            LCD_2_PutChar(txDataByte);
            LCD_2_PutChar(0x0Du);
            LCD_2_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: LCD_2_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of bytes in the TX buffer which are waiting to be 
    *  transmitted.
    *  * TX software buffer is disabled (TX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty TX FIFO, 1 for not full TX FIFO or 4 for full TX FIFO.
    *  * TX software buffer is enabled: returns the number of bytes in the TX 
    *    software buffer which are waiting to be transmitted. Bytes available in the
    *    TX FIFO do not count.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Number of bytes used in the TX buffer. Return value type depends on the TX 
    *  Buffer Size parameter.
    *
    * Global Variables:
    *  LCD_2_txBufferWrite - used to calculate left space.
    *  LCD_2_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 LCD_2_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (LCD_2_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        LCD_2_DisableTxInt();

        if(LCD_2_txBufferRead == LCD_2_txBufferWrite)
        {
            size = 0u;
        }
        else if(LCD_2_txBufferRead < LCD_2_txBufferWrite)
        {
            size = (LCD_2_txBufferWrite - LCD_2_txBufferRead);
        }
        else
        {
            size = (LCD_2_TX_BUFFER_SIZE - LCD_2_txBufferRead) +
                    LCD_2_txBufferWrite;
        }

        LCD_2_EnableTxInt();

    #else

        size = LCD_2_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & LCD_2_TX_STS_FIFO_FULL) != 0u)
        {
            size = LCD_2_FIFO_LENGTH;
        }
        else if((size & LCD_2_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (LCD_2_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: LCD_2_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears all data from the TX buffer and hardware TX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD_2_txBufferWrite - cleared to zero.
    *  LCD_2_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Data waiting in the transmit buffer is not sent; a byte that is currently
    *  transmitting finishes transmitting.
    *
    *******************************************************************************/
    void LCD_2_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        LCD_2_TXDATA_AUX_CTL_REG |= (uint8)  LCD_2_TX_FIFO_CLR;
        LCD_2_TXDATA_AUX_CTL_REG &= (uint8) ~LCD_2_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (LCD_2_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        LCD_2_DisableTxInt();

        LCD_2_txBufferRead = 0u;
        LCD_2_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        LCD_2_EnableTxInt();

    #endif /* (LCD_2_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: LCD_2_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   LCD_2_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   LCD_2_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   LCD_2_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   LCD_2_SEND_WAIT_REINIT - Performs both options: 
    *      LCD_2_SEND_BREAK and LCD_2_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD_2_initVar - checked to identify that the component has been
    *     initialized.
    *  txPeriod - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  There are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Function will block CPU until transmission
    *     complete.
    *  2) User may want to use blocking time if UART configured to the low speed
    *     operation
    *     Example for this case:
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to initialize and use the interrupt to
    *     complete break operation.
    *     Example for this case:
    *     Initialize TX interrupt with "TX - On TX Complete" parameter
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     When interrupt appear with LCD_2_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The LCD_2_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void LCD_2_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(LCD_2_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(LCD_2_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == LCD_2_SEND_BREAK) ||
                (retMode == LCD_2_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                LCD_2_WriteControlRegister(LCD_2_ReadControlRegister() |
                                                      LCD_2_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                LCD_2_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = LCD_2_TXSTATUS_REG;
                }
                while((tmpStat & LCD_2_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == LCD_2_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == LCD_2_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = LCD_2_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & LCD_2_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == LCD_2_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == LCD_2_REINIT) ||
                (retMode == LCD_2_SEND_WAIT_REINIT) )
            {
                LCD_2_WriteControlRegister(LCD_2_ReadControlRegister() &
                                              (uint8)~LCD_2_CTRL_HD_SEND_BREAK);
            }

        #else /* LCD_2_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == LCD_2_SEND_BREAK) ||
                (retMode == LCD_2_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (LCD_2_PARITY_TYPE != LCD_2__B_UART__NONE_REVB) || \
                                    (LCD_2_PARITY_TYPE_SW != 0u) )
                    LCD_2_WriteControlRegister(LCD_2_ReadControlRegister() |
                                                          LCD_2_CTRL_HD_SEND_BREAK);
                #endif /* End LCD_2_PARITY_TYPE != LCD_2__B_UART__NONE_REVB  */

                #if(LCD_2_TXCLKGEN_DP)
                    txPeriod = LCD_2_TXBITCLKTX_COMPLETE_REG;
                    LCD_2_TXBITCLKTX_COMPLETE_REG = LCD_2_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = LCD_2_TXBITCTR_PERIOD_REG;
                    LCD_2_TXBITCTR_PERIOD_REG = LCD_2_TXBITCTR_BREAKBITS8X;
                #endif /* End LCD_2_TXCLKGEN_DP */

                /* Send zeros */
                LCD_2_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = LCD_2_TXSTATUS_REG;
                }
                while((tmpStat & LCD_2_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == LCD_2_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == LCD_2_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = LCD_2_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & LCD_2_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == LCD_2_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == LCD_2_REINIT) ||
                (retMode == LCD_2_SEND_WAIT_REINIT) )
            {

            #if(LCD_2_TXCLKGEN_DP)
                LCD_2_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                LCD_2_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End LCD_2_TXCLKGEN_DP */

            #if( (LCD_2_PARITY_TYPE != LCD_2__B_UART__NONE_REVB) || \
                 (LCD_2_PARITY_TYPE_SW != 0u) )
                LCD_2_WriteControlRegister(LCD_2_ReadControlRegister() &
                                                      (uint8) ~LCD_2_CTRL_HD_SEND_BREAK);
            #endif /* End LCD_2_PARITY_TYPE != NONE */
            }
        #endif    /* End LCD_2_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: LCD_2_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       LCD_2_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       LCD_2_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears LCD_2_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void LCD_2_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( LCD_2_CONTROL_REG_REMOVED == 0u )
            LCD_2_WriteControlRegister(LCD_2_ReadControlRegister() |
                                                  LCD_2_CTRL_MARK);
        #endif /* End LCD_2_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( LCD_2_CONTROL_REG_REMOVED == 0u )
            LCD_2_WriteControlRegister(LCD_2_ReadControlRegister() &
                                                  (uint8) ~LCD_2_CTRL_MARK);
        #endif /* End LCD_2_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndLCD_2_TX_ENABLED */

#if(LCD_2_HD_ENABLED)


    /*******************************************************************************
    * Function Name: LCD_2_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the receiver configuration in half duplex mode. After calling this
    *  function, the UART is ready to receive data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the transmitter
    *  configuration.
    *
    *******************************************************************************/
    void LCD_2_LoadRxConfig(void) 
    {
        LCD_2_WriteControlRegister(LCD_2_ReadControlRegister() &
                                                (uint8)~LCD_2_CTRL_HD_SEND);
        LCD_2_RXBITCTR_PERIOD_REG = LCD_2_HD_RXBITCTR_INIT;

    #if (LCD_2_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        LCD_2_SetRxInterruptMode(LCD_2_INIT_RX_INTERRUPTS_MASK);
    #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: LCD_2_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the transmitter configuration in half duplex mode. After calling this
    *  function, the UART is ready to transmit data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the receiver configuration.
    *
    *******************************************************************************/
    void LCD_2_LoadTxConfig(void) 
    {
    #if (LCD_2_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        LCD_2_SetRxInterruptMode(0u);
    #endif /* (LCD_2_RX_INTERRUPT_ENABLED) */

        LCD_2_WriteControlRegister(LCD_2_ReadControlRegister() | LCD_2_CTRL_HD_SEND);
        LCD_2_RXBITCTR_PERIOD_REG = LCD_2_HD_TXBITCTR_INIT;
    }

#endif  /* LCD_2_HD_ENABLED */


/* [] END OF FILE */
