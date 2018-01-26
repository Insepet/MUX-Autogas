/*******************************************************************************
* File Name: PC.c
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

#include "PC.h"
#if (PC_INTERNAL_CLOCK_USED)
    #include "PC_IntClock.h"
#endif /* End PC_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 PC_initVar = 0u;

#if (PC_TX_INTERRUPT_ENABLED && PC_TX_ENABLED)
    volatile uint8 PC_txBuffer[PC_TX_BUFFER_SIZE];
    volatile uint8 PC_txBufferRead = 0u;
    uint8 PC_txBufferWrite = 0u;
#endif /* (PC_TX_INTERRUPT_ENABLED && PC_TX_ENABLED) */

#if (PC_RX_INTERRUPT_ENABLED && (PC_RX_ENABLED || PC_HD_ENABLED))
    uint8 PC_errorStatus = 0u;
    volatile uint8 PC_rxBuffer[PC_RX_BUFFER_SIZE];
    volatile uint16 PC_rxBufferRead  = 0u;
    volatile uint16 PC_rxBufferWrite = 0u;
    volatile uint8 PC_rxBufferLoopDetect = 0u;
    volatile uint8 PC_rxBufferOverflow   = 0u;
    #if (PC_RXHW_ADDRESS_ENABLED)
        volatile uint8 PC_rxAddressMode = PC_RX_ADDRESS_MODE;
        volatile uint8 PC_rxAddressDetected = 0u;
    #endif /* (PC_RXHW_ADDRESS_ENABLED) */
#endif /* (PC_RX_INTERRUPT_ENABLED && (PC_RX_ENABLED || PC_HD_ENABLED)) */


/*******************************************************************************
* Function Name: PC_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  PC_Start() sets the initVar variable, calls the
*  PC_Init() function, and then calls the
*  PC_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The PC_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time PC_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the PC_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PC_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(PC_initVar == 0u)
    {
        PC_Init();
        PC_initVar = 1u;
    }

    PC_Enable();
}


/*******************************************************************************
* Function Name: PC_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call PC_Init() because
*  the PC_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void PC_Init(void) 
{
    #if(PC_RX_ENABLED || PC_HD_ENABLED)

        #if (PC_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(PC_RX_VECT_NUM, &PC_RXISR);
            CyIntSetPriority(PC_RX_VECT_NUM, PC_RX_PRIOR_NUM);
            PC_errorStatus = 0u;
        #endif /* (PC_RX_INTERRUPT_ENABLED) */

        #if (PC_RXHW_ADDRESS_ENABLED)
            PC_SetRxAddressMode(PC_RX_ADDRESS_MODE);
            PC_SetRxAddress1(PC_RX_HW_ADDRESS1);
            PC_SetRxAddress2(PC_RX_HW_ADDRESS2);
        #endif /* End PC_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        PC_RXBITCTR_PERIOD_REG = PC_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        PC_RXSTATUS_MASK_REG  = PC_INIT_RX_INTERRUPTS_MASK;
    #endif /* End PC_RX_ENABLED || PC_HD_ENABLED*/

    #if(PC_TX_ENABLED)
        #if (PC_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(PC_TX_VECT_NUM, &PC_TXISR);
            CyIntSetPriority(PC_TX_VECT_NUM, PC_TX_PRIOR_NUM);
        #endif /* (PC_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (PC_TXCLKGEN_DP)
            PC_TXBITCLKGEN_CTR_REG = PC_BIT_CENTER;
            PC_TXBITCLKTX_COMPLETE_REG = ((PC_NUMBER_OF_DATA_BITS +
                        PC_NUMBER_OF_START_BIT) * PC_OVER_SAMPLE_COUNT) - 1u;
        #else
            PC_TXBITCTR_PERIOD_REG = ((PC_NUMBER_OF_DATA_BITS +
                        PC_NUMBER_OF_START_BIT) * PC_OVER_SAMPLE_8) - 1u;
        #endif /* End PC_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (PC_TX_INTERRUPT_ENABLED)
            PC_TXSTATUS_MASK_REG = PC_TX_STS_FIFO_EMPTY;
        #else
            PC_TXSTATUS_MASK_REG = PC_INIT_TX_INTERRUPTS_MASK;
        #endif /*End PC_TX_INTERRUPT_ENABLED*/

    #endif /* End PC_TX_ENABLED */

    #if(PC_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        PC_WriteControlRegister( \
            (PC_ReadControlRegister() & (uint8)~PC_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(PC_PARITY_TYPE << PC_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End PC_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: PC_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call PC_Enable() because the PC_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PC_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void PC_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (PC_RX_ENABLED || PC_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        PC_RXBITCTR_CONTROL_REG |= PC_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        PC_RXSTATUS_ACTL_REG  |= PC_INT_ENABLE;

        #if (PC_RX_INTERRUPT_ENABLED)
            PC_EnableRxInt();

            #if (PC_RXHW_ADDRESS_ENABLED)
                PC_rxAddressDetected = 0u;
            #endif /* (PC_RXHW_ADDRESS_ENABLED) */
        #endif /* (PC_RX_INTERRUPT_ENABLED) */
    #endif /* (PC_RX_ENABLED || PC_HD_ENABLED) */

    #if(PC_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!PC_TXCLKGEN_DP)
            PC_TXBITCTR_CONTROL_REG |= PC_CNTR_ENABLE;
        #endif /* End PC_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        PC_TXSTATUS_ACTL_REG |= PC_INT_ENABLE;
        #if (PC_TX_INTERRUPT_ENABLED)
            PC_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            PC_EnableTxInt();
        #endif /* (PC_TX_INTERRUPT_ENABLED) */
     #endif /* (PC_TX_INTERRUPT_ENABLED) */

    #if (PC_INTERNAL_CLOCK_USED)
        PC_IntClock_Start();  /* Enable the clock */
    #endif /* (PC_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: PC_Stop
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
void PC_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (PC_RX_ENABLED || PC_HD_ENABLED)
        PC_RXBITCTR_CONTROL_REG &= (uint8) ~PC_CNTR_ENABLE;
    #endif /* (PC_RX_ENABLED || PC_HD_ENABLED) */

    #if (PC_TX_ENABLED)
        #if(!PC_TXCLKGEN_DP)
            PC_TXBITCTR_CONTROL_REG &= (uint8) ~PC_CNTR_ENABLE;
        #endif /* (!PC_TXCLKGEN_DP) */
    #endif /* (PC_TX_ENABLED) */

    #if (PC_INTERNAL_CLOCK_USED)
        PC_IntClock_Stop();   /* Disable the clock */
    #endif /* (PC_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (PC_RX_ENABLED || PC_HD_ENABLED)
        PC_RXSTATUS_ACTL_REG  &= (uint8) ~PC_INT_ENABLE;

        #if (PC_RX_INTERRUPT_ENABLED)
            PC_DisableRxInt();
        #endif /* (PC_RX_INTERRUPT_ENABLED) */
    #endif /* (PC_RX_ENABLED || PC_HD_ENABLED) */

    #if (PC_TX_ENABLED)
        PC_TXSTATUS_ACTL_REG &= (uint8) ~PC_INT_ENABLE;

        #if (PC_TX_INTERRUPT_ENABLED)
            PC_DisableTxInt();
        #endif /* (PC_TX_INTERRUPT_ENABLED) */
    #endif /* (PC_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: PC_ReadControlRegister
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
uint8 PC_ReadControlRegister(void) 
{
    #if (PC_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(PC_CONTROL_REG);
    #endif /* (PC_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: PC_WriteControlRegister
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
void  PC_WriteControlRegister(uint8 control) 
{
    #if (PC_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       PC_CONTROL_REG = control;
    #endif /* (PC_CONTROL_REG_REMOVED) */
}


#if(PC_RX_ENABLED || PC_HD_ENABLED)
    /*******************************************************************************
    * Function Name: PC_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      PC_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      PC_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      PC_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      PC_RX_STS_BREAK            Interrupt on break.
    *      PC_RX_STS_OVERRUN          Interrupt on overrun error.
    *      PC_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      PC_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void PC_SetRxInterruptMode(uint8 intSrc) 
    {
        PC_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: PC_ReadRxData
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
    *  PC_rxBuffer - RAM buffer pointer for save received data.
    *  PC_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  PC_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  PC_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 PC_ReadRxData(void) 
    {
        uint8 rxData;

    #if (PC_RX_INTERRUPT_ENABLED)

        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        PC_DisableRxInt();

        locRxBufferRead  = PC_rxBufferRead;
        locRxBufferWrite = PC_rxBufferWrite;

        if( (PC_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = PC_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= PC_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            PC_rxBufferRead = locRxBufferRead;

            if(PC_rxBufferLoopDetect != 0u)
            {
                PC_rxBufferLoopDetect = 0u;
                #if ((PC_RX_INTERRUPT_ENABLED) && (PC_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( PC_HD_ENABLED )
                        if((PC_CONTROL_REG & PC_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            PC_RXSTATUS_MASK_REG  |= PC_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        PC_RXSTATUS_MASK_REG  |= PC_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end PC_HD_ENABLED */
                #endif /* ((PC_RX_INTERRUPT_ENABLED) && (PC_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = PC_RXDATA_REG;
        }

        PC_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = PC_RXDATA_REG;

    #endif /* (PC_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: PC_ReadRxStatus
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
    *  PC_RX_STS_FIFO_NOTEMPTY.
    *  PC_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  PC_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   PC_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   PC_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 PC_ReadRxStatus(void) 
    {
        uint8 status;

        status = PC_RXSTATUS_REG & PC_RX_HW_MASK;

    #if (PC_RX_INTERRUPT_ENABLED)
        if(PC_rxBufferOverflow != 0u)
        {
            status |= PC_RX_STS_SOFT_BUFF_OVER;
            PC_rxBufferOverflow = 0u;
        }
    #endif /* (PC_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: PC_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. PC_GetChar() is
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
    *  PC_rxBuffer - RAM buffer pointer for save received data.
    *  PC_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  PC_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  PC_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 PC_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (PC_RX_INTERRUPT_ENABLED)
        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        PC_DisableRxInt();

        locRxBufferRead  = PC_rxBufferRead;
        locRxBufferWrite = PC_rxBufferWrite;

        if( (PC_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = PC_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= PC_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            PC_rxBufferRead = locRxBufferRead;

            if(PC_rxBufferLoopDetect != 0u)
            {
                PC_rxBufferLoopDetect = 0u;
                #if( (PC_RX_INTERRUPT_ENABLED) && (PC_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( PC_HD_ENABLED )
                        if((PC_CONTROL_REG & PC_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            PC_RXSTATUS_MASK_REG |= PC_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        PC_RXSTATUS_MASK_REG |= PC_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end PC_HD_ENABLED */
                #endif /* PC_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = PC_RXSTATUS_REG;
            if((rxStatus & PC_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = PC_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (PC_RX_STS_BREAK | PC_RX_STS_PAR_ERROR |
                                PC_RX_STS_STOP_ERROR | PC_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        PC_EnableRxInt();

    #else

        rxStatus =PC_RXSTATUS_REG;
        if((rxStatus & PC_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = PC_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (PC_RX_STS_BREAK | PC_RX_STS_PAR_ERROR |
                            PC_RX_STS_STOP_ERROR | PC_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (PC_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: PC_GetByte
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
    uint16 PC_GetByte(void) 
    {
        
    #if (PC_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        PC_DisableRxInt();
        locErrorStatus = (uint16)PC_errorStatus;
        PC_errorStatus = 0u;
        PC_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | PC_ReadRxData() );
    #else
        return ( ((uint16)PC_ReadRxStatus() << 8u) | PC_ReadRxData() );
    #endif /* PC_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: PC_GetRxBufferSize
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
    *  uint16: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  PC_rxBufferWrite - used to calculate left bytes.
    *  PC_rxBufferRead - used to calculate left bytes.
    *  PC_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint16 PC_GetRxBufferSize(void)
                                                            
    {
        uint16 size;

    #if (PC_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        PC_DisableRxInt();

        if(PC_rxBufferRead == PC_rxBufferWrite)
        {
            if(PC_rxBufferLoopDetect != 0u)
            {
                size = PC_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(PC_rxBufferRead < PC_rxBufferWrite)
        {
            size = (PC_rxBufferWrite - PC_rxBufferRead);
        }
        else
        {
            size = (PC_RX_BUFFER_SIZE - PC_rxBufferRead) + PC_rxBufferWrite;
        }

        PC_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((PC_RXSTATUS_REG & PC_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (PC_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: PC_ClearRxBuffer
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
    *  PC_rxBufferWrite - cleared to zero.
    *  PC_rxBufferRead - cleared to zero.
    *  PC_rxBufferLoopDetect - cleared to zero.
    *  PC_rxBufferOverflow - cleared to zero.
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
    void PC_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        PC_RXDATA_AUX_CTL_REG |= (uint8)  PC_RX_FIFO_CLR;
        PC_RXDATA_AUX_CTL_REG &= (uint8) ~PC_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (PC_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        PC_DisableRxInt();

        PC_rxBufferRead = 0u;
        PC_rxBufferWrite = 0u;
        PC_rxBufferLoopDetect = 0u;
        PC_rxBufferOverflow = 0u;

        PC_EnableRxInt();

    #endif /* (PC_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: PC_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  PC__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  PC__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  PC__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  PC__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  PC__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  PC_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  PC_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void PC_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(PC_RXHW_ADDRESS_ENABLED)
            #if(PC_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* PC_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = PC_CONTROL_REG & (uint8)~PC_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << PC_CTRL_RXADDR_MODE0_SHIFT);
                PC_CONTROL_REG = tmpCtrl;

                #if(PC_RX_INTERRUPT_ENABLED && \
                   (PC_RXBUFFERSIZE > PC_FIFO_LENGTH) )
                    PC_rxAddressMode = addressMode;
                    PC_rxAddressDetected = 0u;
                #endif /* End PC_RXBUFFERSIZE > PC_FIFO_LENGTH*/
            #endif /* End PC_CONTROL_REG_REMOVED */
        #else /* PC_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End PC_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: PC_SetRxAddress1
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
    void PC_SetRxAddress1(uint8 address) 
    {
        PC_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: PC_SetRxAddress2
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
    void PC_SetRxAddress2(uint8 address) 
    {
        PC_RXADDRESS2_REG = address;
    }

#endif  /* PC_RX_ENABLED || PC_HD_ENABLED*/


#if( (PC_TX_ENABLED) || (PC_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: PC_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   PC_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   PC_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   PC_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   PC_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void PC_SetTxInterruptMode(uint8 intSrc) 
    {
        PC_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: PC_WriteTxData
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
    *  PC_txBuffer - RAM buffer pointer for save data for transmission
    *  PC_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  PC_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  PC_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void PC_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(PC_initVar != 0u)
        {
        #if (PC_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            PC_DisableTxInt();

            if( (PC_txBufferRead == PC_txBufferWrite) &&
                ((PC_TXSTATUS_REG & PC_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                PC_TXDATA_REG = txDataByte;
            }
            else
            {
                if(PC_txBufferWrite >= PC_TX_BUFFER_SIZE)
                {
                    PC_txBufferWrite = 0u;
                }

                PC_txBuffer[PC_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                PC_txBufferWrite++;
            }

            PC_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            PC_TXDATA_REG = txDataByte;

        #endif /*(PC_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: PC_ReadTxStatus
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
    uint8 PC_ReadTxStatus(void) 
    {
        return(PC_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: PC_PutChar
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
    *  PC_txBuffer - RAM buffer pointer for save data for transmission
    *  PC_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  PC_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  PC_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void PC_PutChar(uint8 txDataByte) 
    {
    #if (PC_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((PC_TX_BUFFER_SIZE > PC_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            PC_DisableTxInt();
        #endif /* (PC_TX_BUFFER_SIZE > PC_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = PC_txBufferWrite;
            locTxBufferRead  = PC_txBufferRead;

        #if ((PC_TX_BUFFER_SIZE > PC_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            PC_EnableTxInt();
        #endif /* (PC_TX_BUFFER_SIZE > PC_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(PC_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((PC_TXSTATUS_REG & PC_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            PC_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= PC_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            PC_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((PC_TX_BUFFER_SIZE > PC_MAX_BYTE_VALUE) && (CY_PSOC3))
            PC_DisableTxInt();
        #endif /* (PC_TX_BUFFER_SIZE > PC_MAX_BYTE_VALUE) && (CY_PSOC3) */

            PC_txBufferWrite = locTxBufferWrite;

        #if ((PC_TX_BUFFER_SIZE > PC_MAX_BYTE_VALUE) && (CY_PSOC3))
            PC_EnableTxInt();
        #endif /* (PC_TX_BUFFER_SIZE > PC_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (PC_TXSTATUS_REG & PC_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                PC_SetPendingTxInt();
            }
        }

    #else

        while((PC_TXSTATUS_REG & PC_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        PC_TXDATA_REG = txDataByte;

    #endif /* PC_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: PC_PutString
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
    *  PC_initVar - checked to identify that the component has been
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
    void PC_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(PC_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                PC_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: PC_PutArray
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
    *  PC_initVar - checked to identify that the component has been
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
    void PC_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(PC_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                PC_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: PC_PutCRLF
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
    *  PC_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void PC_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(PC_initVar != 0u)
        {
            PC_PutChar(txDataByte);
            PC_PutChar(0x0Du);
            PC_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: PC_GetTxBufferSize
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
    *  PC_txBufferWrite - used to calculate left space.
    *  PC_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 PC_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (PC_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        PC_DisableTxInt();

        if(PC_txBufferRead == PC_txBufferWrite)
        {
            size = 0u;
        }
        else if(PC_txBufferRead < PC_txBufferWrite)
        {
            size = (PC_txBufferWrite - PC_txBufferRead);
        }
        else
        {
            size = (PC_TX_BUFFER_SIZE - PC_txBufferRead) +
                    PC_txBufferWrite;
        }

        PC_EnableTxInt();

    #else

        size = PC_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & PC_TX_STS_FIFO_FULL) != 0u)
        {
            size = PC_FIFO_LENGTH;
        }
        else if((size & PC_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (PC_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: PC_ClearTxBuffer
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
    *  PC_txBufferWrite - cleared to zero.
    *  PC_txBufferRead - cleared to zero.
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
    void PC_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        PC_TXDATA_AUX_CTL_REG |= (uint8)  PC_TX_FIFO_CLR;
        PC_TXDATA_AUX_CTL_REG &= (uint8) ~PC_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (PC_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        PC_DisableTxInt();

        PC_txBufferRead = 0u;
        PC_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        PC_EnableTxInt();

    #endif /* (PC_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: PC_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   PC_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   PC_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   PC_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   PC_SEND_WAIT_REINIT - Performs both options: 
    *      PC_SEND_BREAK and PC_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  PC_initVar - checked to identify that the component has been
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
    *     When interrupt appear with PC_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The PC_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void PC_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(PC_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(PC_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == PC_SEND_BREAK) ||
                (retMode == PC_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                PC_WriteControlRegister(PC_ReadControlRegister() |
                                                      PC_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                PC_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = PC_TXSTATUS_REG;
                }
                while((tmpStat & PC_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == PC_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == PC_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = PC_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & PC_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == PC_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == PC_REINIT) ||
                (retMode == PC_SEND_WAIT_REINIT) )
            {
                PC_WriteControlRegister(PC_ReadControlRegister() &
                                              (uint8)~PC_CTRL_HD_SEND_BREAK);
            }

        #else /* PC_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == PC_SEND_BREAK) ||
                (retMode == PC_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (PC_PARITY_TYPE != PC__B_UART__NONE_REVB) || \
                                    (PC_PARITY_TYPE_SW != 0u) )
                    PC_WriteControlRegister(PC_ReadControlRegister() |
                                                          PC_CTRL_HD_SEND_BREAK);
                #endif /* End PC_PARITY_TYPE != PC__B_UART__NONE_REVB  */

                #if(PC_TXCLKGEN_DP)
                    txPeriod = PC_TXBITCLKTX_COMPLETE_REG;
                    PC_TXBITCLKTX_COMPLETE_REG = PC_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = PC_TXBITCTR_PERIOD_REG;
                    PC_TXBITCTR_PERIOD_REG = PC_TXBITCTR_BREAKBITS8X;
                #endif /* End PC_TXCLKGEN_DP */

                /* Send zeros */
                PC_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = PC_TXSTATUS_REG;
                }
                while((tmpStat & PC_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == PC_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == PC_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = PC_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & PC_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == PC_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == PC_REINIT) ||
                (retMode == PC_SEND_WAIT_REINIT) )
            {

            #if(PC_TXCLKGEN_DP)
                PC_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                PC_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End PC_TXCLKGEN_DP */

            #if( (PC_PARITY_TYPE != PC__B_UART__NONE_REVB) || \
                 (PC_PARITY_TYPE_SW != 0u) )
                PC_WriteControlRegister(PC_ReadControlRegister() &
                                                      (uint8) ~PC_CTRL_HD_SEND_BREAK);
            #endif /* End PC_PARITY_TYPE != NONE */
            }
        #endif    /* End PC_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: PC_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       PC_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       PC_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears PC_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void PC_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( PC_CONTROL_REG_REMOVED == 0u )
            PC_WriteControlRegister(PC_ReadControlRegister() |
                                                  PC_CTRL_MARK);
        #endif /* End PC_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( PC_CONTROL_REG_REMOVED == 0u )
            PC_WriteControlRegister(PC_ReadControlRegister() &
                                                  (uint8) ~PC_CTRL_MARK);
        #endif /* End PC_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndPC_TX_ENABLED */

#if(PC_HD_ENABLED)


    /*******************************************************************************
    * Function Name: PC_LoadRxConfig
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
    void PC_LoadRxConfig(void) 
    {
        PC_WriteControlRegister(PC_ReadControlRegister() &
                                                (uint8)~PC_CTRL_HD_SEND);
        PC_RXBITCTR_PERIOD_REG = PC_HD_RXBITCTR_INIT;

    #if (PC_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        PC_SetRxInterruptMode(PC_INIT_RX_INTERRUPTS_MASK);
    #endif /* (PC_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: PC_LoadTxConfig
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
    void PC_LoadTxConfig(void) 
    {
    #if (PC_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        PC_SetRxInterruptMode(0u);
    #endif /* (PC_RX_INTERRUPT_ENABLED) */

        PC_WriteControlRegister(PC_ReadControlRegister() | PC_CTRL_HD_SEND);
        PC_RXBITCTR_PERIOD_REG = PC_HD_TXBITCTR_INIT;
    }

#endif  /* PC_HD_ENABLED */


/* [] END OF FILE */
