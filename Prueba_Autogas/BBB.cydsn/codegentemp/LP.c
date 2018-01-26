/*******************************************************************************
* File Name: LP.c
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

#include "LP.h"
#if (LP_INTERNAL_CLOCK_USED)
    #include "LP_IntClock.h"
#endif /* End LP_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 LP_initVar = 0u;

#if (LP_TX_INTERRUPT_ENABLED && LP_TX_ENABLED)
    volatile uint8 LP_txBuffer[LP_TX_BUFFER_SIZE];
    volatile uint8 LP_txBufferRead = 0u;
    uint8 LP_txBufferWrite = 0u;
#endif /* (LP_TX_INTERRUPT_ENABLED && LP_TX_ENABLED) */

#if (LP_RX_INTERRUPT_ENABLED && (LP_RX_ENABLED || LP_HD_ENABLED))
    uint8 LP_errorStatus = 0u;
    volatile uint8 LP_rxBuffer[LP_RX_BUFFER_SIZE];
    volatile uint8 LP_rxBufferRead  = 0u;
    volatile uint8 LP_rxBufferWrite = 0u;
    volatile uint8 LP_rxBufferLoopDetect = 0u;
    volatile uint8 LP_rxBufferOverflow   = 0u;
    #if (LP_RXHW_ADDRESS_ENABLED)
        volatile uint8 LP_rxAddressMode = LP_RX_ADDRESS_MODE;
        volatile uint8 LP_rxAddressDetected = 0u;
    #endif /* (LP_RXHW_ADDRESS_ENABLED) */
#endif /* (LP_RX_INTERRUPT_ENABLED && (LP_RX_ENABLED || LP_HD_ENABLED)) */


/*******************************************************************************
* Function Name: LP_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  LP_Start() sets the initVar variable, calls the
*  LP_Init() function, and then calls the
*  LP_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The LP_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time LP_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the LP_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LP_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(LP_initVar == 0u)
    {
        LP_Init();
        LP_initVar = 1u;
    }

    LP_Enable();
}


/*******************************************************************************
* Function Name: LP_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call LP_Init() because
*  the LP_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LP_Init(void) 
{
    #if(LP_RX_ENABLED || LP_HD_ENABLED)

        #if (LP_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(LP_RX_VECT_NUM, &LP_RXISR);
            CyIntSetPriority(LP_RX_VECT_NUM, LP_RX_PRIOR_NUM);
            LP_errorStatus = 0u;
        #endif /* (LP_RX_INTERRUPT_ENABLED) */

        #if (LP_RXHW_ADDRESS_ENABLED)
            LP_SetRxAddressMode(LP_RX_ADDRESS_MODE);
            LP_SetRxAddress1(LP_RX_HW_ADDRESS1);
            LP_SetRxAddress2(LP_RX_HW_ADDRESS2);
        #endif /* End LP_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        LP_RXBITCTR_PERIOD_REG = LP_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        LP_RXSTATUS_MASK_REG  = LP_INIT_RX_INTERRUPTS_MASK;
    #endif /* End LP_RX_ENABLED || LP_HD_ENABLED*/

    #if(LP_TX_ENABLED)
        #if (LP_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(LP_TX_VECT_NUM, &LP_TXISR);
            CyIntSetPriority(LP_TX_VECT_NUM, LP_TX_PRIOR_NUM);
        #endif /* (LP_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (LP_TXCLKGEN_DP)
            LP_TXBITCLKGEN_CTR_REG = LP_BIT_CENTER;
            LP_TXBITCLKTX_COMPLETE_REG = ((LP_NUMBER_OF_DATA_BITS +
                        LP_NUMBER_OF_START_BIT) * LP_OVER_SAMPLE_COUNT) - 1u;
        #else
            LP_TXBITCTR_PERIOD_REG = ((LP_NUMBER_OF_DATA_BITS +
                        LP_NUMBER_OF_START_BIT) * LP_OVER_SAMPLE_8) - 1u;
        #endif /* End LP_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (LP_TX_INTERRUPT_ENABLED)
            LP_TXSTATUS_MASK_REG = LP_TX_STS_FIFO_EMPTY;
        #else
            LP_TXSTATUS_MASK_REG = LP_INIT_TX_INTERRUPTS_MASK;
        #endif /*End LP_TX_INTERRUPT_ENABLED*/

    #endif /* End LP_TX_ENABLED */

    #if(LP_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        LP_WriteControlRegister( \
            (LP_ReadControlRegister() & (uint8)~LP_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(LP_PARITY_TYPE << LP_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End LP_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: LP_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call LP_Enable() because the LP_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LP_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void LP_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (LP_RX_ENABLED || LP_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        LP_RXBITCTR_CONTROL_REG |= LP_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        LP_RXSTATUS_ACTL_REG  |= LP_INT_ENABLE;

        #if (LP_RX_INTERRUPT_ENABLED)
            LP_EnableRxInt();

            #if (LP_RXHW_ADDRESS_ENABLED)
                LP_rxAddressDetected = 0u;
            #endif /* (LP_RXHW_ADDRESS_ENABLED) */
        #endif /* (LP_RX_INTERRUPT_ENABLED) */
    #endif /* (LP_RX_ENABLED || LP_HD_ENABLED) */

    #if(LP_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!LP_TXCLKGEN_DP)
            LP_TXBITCTR_CONTROL_REG |= LP_CNTR_ENABLE;
        #endif /* End LP_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        LP_TXSTATUS_ACTL_REG |= LP_INT_ENABLE;
        #if (LP_TX_INTERRUPT_ENABLED)
            LP_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            LP_EnableTxInt();
        #endif /* (LP_TX_INTERRUPT_ENABLED) */
     #endif /* (LP_TX_INTERRUPT_ENABLED) */

    #if (LP_INTERNAL_CLOCK_USED)
        LP_IntClock_Start();  /* Enable the clock */
    #endif /* (LP_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: LP_Stop
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
void LP_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (LP_RX_ENABLED || LP_HD_ENABLED)
        LP_RXBITCTR_CONTROL_REG &= (uint8) ~LP_CNTR_ENABLE;
    #endif /* (LP_RX_ENABLED || LP_HD_ENABLED) */

    #if (LP_TX_ENABLED)
        #if(!LP_TXCLKGEN_DP)
            LP_TXBITCTR_CONTROL_REG &= (uint8) ~LP_CNTR_ENABLE;
        #endif /* (!LP_TXCLKGEN_DP) */
    #endif /* (LP_TX_ENABLED) */

    #if (LP_INTERNAL_CLOCK_USED)
        LP_IntClock_Stop();   /* Disable the clock */
    #endif /* (LP_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (LP_RX_ENABLED || LP_HD_ENABLED)
        LP_RXSTATUS_ACTL_REG  &= (uint8) ~LP_INT_ENABLE;

        #if (LP_RX_INTERRUPT_ENABLED)
            LP_DisableRxInt();
        #endif /* (LP_RX_INTERRUPT_ENABLED) */
    #endif /* (LP_RX_ENABLED || LP_HD_ENABLED) */

    #if (LP_TX_ENABLED)
        LP_TXSTATUS_ACTL_REG &= (uint8) ~LP_INT_ENABLE;

        #if (LP_TX_INTERRUPT_ENABLED)
            LP_DisableTxInt();
        #endif /* (LP_TX_INTERRUPT_ENABLED) */
    #endif /* (LP_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: LP_ReadControlRegister
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
uint8 LP_ReadControlRegister(void) 
{
    #if (LP_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(LP_CONTROL_REG);
    #endif /* (LP_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: LP_WriteControlRegister
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
void  LP_WriteControlRegister(uint8 control) 
{
    #if (LP_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       LP_CONTROL_REG = control;
    #endif /* (LP_CONTROL_REG_REMOVED) */
}


#if(LP_RX_ENABLED || LP_HD_ENABLED)
    /*******************************************************************************
    * Function Name: LP_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      LP_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      LP_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      LP_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      LP_RX_STS_BREAK            Interrupt on break.
    *      LP_RX_STS_OVERRUN          Interrupt on overrun error.
    *      LP_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      LP_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void LP_SetRxInterruptMode(uint8 intSrc) 
    {
        LP_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: LP_ReadRxData
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
    *  LP_rxBuffer - RAM buffer pointer for save received data.
    *  LP_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  LP_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  LP_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 LP_ReadRxData(void) 
    {
        uint8 rxData;

    #if (LP_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        LP_DisableRxInt();

        locRxBufferRead  = LP_rxBufferRead;
        locRxBufferWrite = LP_rxBufferWrite;

        if( (LP_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = LP_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= LP_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            LP_rxBufferRead = locRxBufferRead;

            if(LP_rxBufferLoopDetect != 0u)
            {
                LP_rxBufferLoopDetect = 0u;
                #if ((LP_RX_INTERRUPT_ENABLED) && (LP_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( LP_HD_ENABLED )
                        if((LP_CONTROL_REG & LP_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            LP_RXSTATUS_MASK_REG  |= LP_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        LP_RXSTATUS_MASK_REG  |= LP_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end LP_HD_ENABLED */
                #endif /* ((LP_RX_INTERRUPT_ENABLED) && (LP_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = LP_RXDATA_REG;
        }

        LP_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = LP_RXDATA_REG;

    #endif /* (LP_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: LP_ReadRxStatus
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
    *  LP_RX_STS_FIFO_NOTEMPTY.
    *  LP_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  LP_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   LP_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   LP_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 LP_ReadRxStatus(void) 
    {
        uint8 status;

        status = LP_RXSTATUS_REG & LP_RX_HW_MASK;

    #if (LP_RX_INTERRUPT_ENABLED)
        if(LP_rxBufferOverflow != 0u)
        {
            status |= LP_RX_STS_SOFT_BUFF_OVER;
            LP_rxBufferOverflow = 0u;
        }
    #endif /* (LP_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: LP_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. LP_GetChar() is
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
    *  LP_rxBuffer - RAM buffer pointer for save received data.
    *  LP_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  LP_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  LP_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 LP_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (LP_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        LP_DisableRxInt();

        locRxBufferRead  = LP_rxBufferRead;
        locRxBufferWrite = LP_rxBufferWrite;

        if( (LP_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = LP_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= LP_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            LP_rxBufferRead = locRxBufferRead;

            if(LP_rxBufferLoopDetect != 0u)
            {
                LP_rxBufferLoopDetect = 0u;
                #if( (LP_RX_INTERRUPT_ENABLED) && (LP_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( LP_HD_ENABLED )
                        if((LP_CONTROL_REG & LP_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            LP_RXSTATUS_MASK_REG |= LP_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        LP_RXSTATUS_MASK_REG |= LP_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end LP_HD_ENABLED */
                #endif /* LP_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = LP_RXSTATUS_REG;
            if((rxStatus & LP_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = LP_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (LP_RX_STS_BREAK | LP_RX_STS_PAR_ERROR |
                                LP_RX_STS_STOP_ERROR | LP_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        LP_EnableRxInt();

    #else

        rxStatus =LP_RXSTATUS_REG;
        if((rxStatus & LP_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = LP_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (LP_RX_STS_BREAK | LP_RX_STS_PAR_ERROR |
                            LP_RX_STS_STOP_ERROR | LP_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (LP_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: LP_GetByte
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
    uint16 LP_GetByte(void) 
    {
        
    #if (LP_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        LP_DisableRxInt();
        locErrorStatus = (uint16)LP_errorStatus;
        LP_errorStatus = 0u;
        LP_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | LP_ReadRxData() );
    #else
        return ( ((uint16)LP_ReadRxStatus() << 8u) | LP_ReadRxData() );
    #endif /* LP_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: LP_GetRxBufferSize
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
    *  LP_rxBufferWrite - used to calculate left bytes.
    *  LP_rxBufferRead - used to calculate left bytes.
    *  LP_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 LP_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (LP_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        LP_DisableRxInt();

        if(LP_rxBufferRead == LP_rxBufferWrite)
        {
            if(LP_rxBufferLoopDetect != 0u)
            {
                size = LP_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(LP_rxBufferRead < LP_rxBufferWrite)
        {
            size = (LP_rxBufferWrite - LP_rxBufferRead);
        }
        else
        {
            size = (LP_RX_BUFFER_SIZE - LP_rxBufferRead) + LP_rxBufferWrite;
        }

        LP_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((LP_RXSTATUS_REG & LP_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (LP_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: LP_ClearRxBuffer
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
    *  LP_rxBufferWrite - cleared to zero.
    *  LP_rxBufferRead - cleared to zero.
    *  LP_rxBufferLoopDetect - cleared to zero.
    *  LP_rxBufferOverflow - cleared to zero.
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
    void LP_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        LP_RXDATA_AUX_CTL_REG |= (uint8)  LP_RX_FIFO_CLR;
        LP_RXDATA_AUX_CTL_REG &= (uint8) ~LP_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (LP_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        LP_DisableRxInt();

        LP_rxBufferRead = 0u;
        LP_rxBufferWrite = 0u;
        LP_rxBufferLoopDetect = 0u;
        LP_rxBufferOverflow = 0u;

        LP_EnableRxInt();

    #endif /* (LP_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: LP_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  LP__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  LP__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  LP__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  LP__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  LP__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LP_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  LP_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void LP_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(LP_RXHW_ADDRESS_ENABLED)
            #if(LP_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* LP_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = LP_CONTROL_REG & (uint8)~LP_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << LP_CTRL_RXADDR_MODE0_SHIFT);
                LP_CONTROL_REG = tmpCtrl;

                #if(LP_RX_INTERRUPT_ENABLED && \
                   (LP_RXBUFFERSIZE > LP_FIFO_LENGTH) )
                    LP_rxAddressMode = addressMode;
                    LP_rxAddressDetected = 0u;
                #endif /* End LP_RXBUFFERSIZE > LP_FIFO_LENGTH*/
            #endif /* End LP_CONTROL_REG_REMOVED */
        #else /* LP_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End LP_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: LP_SetRxAddress1
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
    void LP_SetRxAddress1(uint8 address) 
    {
        LP_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: LP_SetRxAddress2
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
    void LP_SetRxAddress2(uint8 address) 
    {
        LP_RXADDRESS2_REG = address;
    }

#endif  /* LP_RX_ENABLED || LP_HD_ENABLED*/


#if( (LP_TX_ENABLED) || (LP_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: LP_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   LP_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   LP_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   LP_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   LP_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void LP_SetTxInterruptMode(uint8 intSrc) 
    {
        LP_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: LP_WriteTxData
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
    *  LP_txBuffer - RAM buffer pointer for save data for transmission
    *  LP_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  LP_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  LP_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void LP_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(LP_initVar != 0u)
        {
        #if (LP_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            LP_DisableTxInt();

            if( (LP_txBufferRead == LP_txBufferWrite) &&
                ((LP_TXSTATUS_REG & LP_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                LP_TXDATA_REG = txDataByte;
            }
            else
            {
                if(LP_txBufferWrite >= LP_TX_BUFFER_SIZE)
                {
                    LP_txBufferWrite = 0u;
                }

                LP_txBuffer[LP_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                LP_txBufferWrite++;
            }

            LP_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            LP_TXDATA_REG = txDataByte;

        #endif /*(LP_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: LP_ReadTxStatus
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
    uint8 LP_ReadTxStatus(void) 
    {
        return(LP_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: LP_PutChar
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
    *  LP_txBuffer - RAM buffer pointer for save data for transmission
    *  LP_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  LP_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  LP_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void LP_PutChar(uint8 txDataByte) 
    {
    #if (LP_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((LP_TX_BUFFER_SIZE > LP_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            LP_DisableTxInt();
        #endif /* (LP_TX_BUFFER_SIZE > LP_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = LP_txBufferWrite;
            locTxBufferRead  = LP_txBufferRead;

        #if ((LP_TX_BUFFER_SIZE > LP_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            LP_EnableTxInt();
        #endif /* (LP_TX_BUFFER_SIZE > LP_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(LP_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((LP_TXSTATUS_REG & LP_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            LP_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= LP_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            LP_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((LP_TX_BUFFER_SIZE > LP_MAX_BYTE_VALUE) && (CY_PSOC3))
            LP_DisableTxInt();
        #endif /* (LP_TX_BUFFER_SIZE > LP_MAX_BYTE_VALUE) && (CY_PSOC3) */

            LP_txBufferWrite = locTxBufferWrite;

        #if ((LP_TX_BUFFER_SIZE > LP_MAX_BYTE_VALUE) && (CY_PSOC3))
            LP_EnableTxInt();
        #endif /* (LP_TX_BUFFER_SIZE > LP_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (LP_TXSTATUS_REG & LP_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                LP_SetPendingTxInt();
            }
        }

    #else

        while((LP_TXSTATUS_REG & LP_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        LP_TXDATA_REG = txDataByte;

    #endif /* LP_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: LP_PutString
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
    *  LP_initVar - checked to identify that the component has been
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
    void LP_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(LP_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                LP_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: LP_PutArray
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
    *  LP_initVar - checked to identify that the component has been
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
    void LP_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(LP_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                LP_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: LP_PutCRLF
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
    *  LP_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void LP_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(LP_initVar != 0u)
        {
            LP_PutChar(txDataByte);
            LP_PutChar(0x0Du);
            LP_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: LP_GetTxBufferSize
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
    *  LP_txBufferWrite - used to calculate left space.
    *  LP_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 LP_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (LP_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        LP_DisableTxInt();

        if(LP_txBufferRead == LP_txBufferWrite)
        {
            size = 0u;
        }
        else if(LP_txBufferRead < LP_txBufferWrite)
        {
            size = (LP_txBufferWrite - LP_txBufferRead);
        }
        else
        {
            size = (LP_TX_BUFFER_SIZE - LP_txBufferRead) +
                    LP_txBufferWrite;
        }

        LP_EnableTxInt();

    #else

        size = LP_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & LP_TX_STS_FIFO_FULL) != 0u)
        {
            size = LP_FIFO_LENGTH;
        }
        else if((size & LP_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (LP_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: LP_ClearTxBuffer
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
    *  LP_txBufferWrite - cleared to zero.
    *  LP_txBufferRead - cleared to zero.
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
    void LP_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        LP_TXDATA_AUX_CTL_REG |= (uint8)  LP_TX_FIFO_CLR;
        LP_TXDATA_AUX_CTL_REG &= (uint8) ~LP_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (LP_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        LP_DisableTxInt();

        LP_txBufferRead = 0u;
        LP_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        LP_EnableTxInt();

    #endif /* (LP_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: LP_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   LP_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   LP_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   LP_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   LP_SEND_WAIT_REINIT - Performs both options: 
    *      LP_SEND_BREAK and LP_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LP_initVar - checked to identify that the component has been
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
    *     When interrupt appear with LP_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The LP_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void LP_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(LP_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(LP_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == LP_SEND_BREAK) ||
                (retMode == LP_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                LP_WriteControlRegister(LP_ReadControlRegister() |
                                                      LP_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                LP_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = LP_TXSTATUS_REG;
                }
                while((tmpStat & LP_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == LP_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == LP_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = LP_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & LP_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == LP_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == LP_REINIT) ||
                (retMode == LP_SEND_WAIT_REINIT) )
            {
                LP_WriteControlRegister(LP_ReadControlRegister() &
                                              (uint8)~LP_CTRL_HD_SEND_BREAK);
            }

        #else /* LP_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == LP_SEND_BREAK) ||
                (retMode == LP_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (LP_PARITY_TYPE != LP__B_UART__NONE_REVB) || \
                                    (LP_PARITY_TYPE_SW != 0u) )
                    LP_WriteControlRegister(LP_ReadControlRegister() |
                                                          LP_CTRL_HD_SEND_BREAK);
                #endif /* End LP_PARITY_TYPE != LP__B_UART__NONE_REVB  */

                #if(LP_TXCLKGEN_DP)
                    txPeriod = LP_TXBITCLKTX_COMPLETE_REG;
                    LP_TXBITCLKTX_COMPLETE_REG = LP_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = LP_TXBITCTR_PERIOD_REG;
                    LP_TXBITCTR_PERIOD_REG = LP_TXBITCTR_BREAKBITS8X;
                #endif /* End LP_TXCLKGEN_DP */

                /* Send zeros */
                LP_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = LP_TXSTATUS_REG;
                }
                while((tmpStat & LP_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == LP_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == LP_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = LP_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & LP_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == LP_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == LP_REINIT) ||
                (retMode == LP_SEND_WAIT_REINIT) )
            {

            #if(LP_TXCLKGEN_DP)
                LP_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                LP_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End LP_TXCLKGEN_DP */

            #if( (LP_PARITY_TYPE != LP__B_UART__NONE_REVB) || \
                 (LP_PARITY_TYPE_SW != 0u) )
                LP_WriteControlRegister(LP_ReadControlRegister() &
                                                      (uint8) ~LP_CTRL_HD_SEND_BREAK);
            #endif /* End LP_PARITY_TYPE != NONE */
            }
        #endif    /* End LP_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: LP_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       LP_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       LP_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears LP_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void LP_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( LP_CONTROL_REG_REMOVED == 0u )
            LP_WriteControlRegister(LP_ReadControlRegister() |
                                                  LP_CTRL_MARK);
        #endif /* End LP_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( LP_CONTROL_REG_REMOVED == 0u )
            LP_WriteControlRegister(LP_ReadControlRegister() &
                                                  (uint8) ~LP_CTRL_MARK);
        #endif /* End LP_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndLP_TX_ENABLED */

#if(LP_HD_ENABLED)


    /*******************************************************************************
    * Function Name: LP_LoadRxConfig
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
    void LP_LoadRxConfig(void) 
    {
        LP_WriteControlRegister(LP_ReadControlRegister() &
                                                (uint8)~LP_CTRL_HD_SEND);
        LP_RXBITCTR_PERIOD_REG = LP_HD_RXBITCTR_INIT;

    #if (LP_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        LP_SetRxInterruptMode(LP_INIT_RX_INTERRUPTS_MASK);
    #endif /* (LP_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: LP_LoadTxConfig
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
    void LP_LoadTxConfig(void) 
    {
    #if (LP_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        LP_SetRxInterruptMode(0u);
    #endif /* (LP_RX_INTERRUPT_ENABLED) */

        LP_WriteControlRegister(LP_ReadControlRegister() | LP_CTRL_HD_SEND);
        LP_RXBITCTR_PERIOD_REG = LP_HD_TXBITCTR_INIT;
    }

#endif  /* LP_HD_ENABLED */


/* [] END OF FILE */
