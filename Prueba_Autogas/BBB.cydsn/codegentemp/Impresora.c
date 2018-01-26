/*******************************************************************************
* File Name: Impresora.c
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

#include "Impresora.h"
#if (Impresora_INTERNAL_CLOCK_USED)
    #include "Impresora_IntClock.h"
#endif /* End Impresora_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 Impresora_initVar = 0u;

#if (Impresora_TX_INTERRUPT_ENABLED && Impresora_TX_ENABLED)
    volatile uint8 Impresora_txBuffer[Impresora_TX_BUFFER_SIZE];
    volatile uint8 Impresora_txBufferRead = 0u;
    uint8 Impresora_txBufferWrite = 0u;
#endif /* (Impresora_TX_INTERRUPT_ENABLED && Impresora_TX_ENABLED) */

#if (Impresora_RX_INTERRUPT_ENABLED && (Impresora_RX_ENABLED || Impresora_HD_ENABLED))
    uint8 Impresora_errorStatus = 0u;
    volatile uint8 Impresora_rxBuffer[Impresora_RX_BUFFER_SIZE];
    volatile uint8 Impresora_rxBufferRead  = 0u;
    volatile uint8 Impresora_rxBufferWrite = 0u;
    volatile uint8 Impresora_rxBufferLoopDetect = 0u;
    volatile uint8 Impresora_rxBufferOverflow   = 0u;
    #if (Impresora_RXHW_ADDRESS_ENABLED)
        volatile uint8 Impresora_rxAddressMode = Impresora_RX_ADDRESS_MODE;
        volatile uint8 Impresora_rxAddressDetected = 0u;
    #endif /* (Impresora_RXHW_ADDRESS_ENABLED) */
#endif /* (Impresora_RX_INTERRUPT_ENABLED && (Impresora_RX_ENABLED || Impresora_HD_ENABLED)) */


/*******************************************************************************
* Function Name: Impresora_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  Impresora_Start() sets the initVar variable, calls the
*  Impresora_Init() function, and then calls the
*  Impresora_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The Impresora_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time Impresora_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the Impresora_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Impresora_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(Impresora_initVar == 0u)
    {
        Impresora_Init();
        Impresora_initVar = 1u;
    }

    Impresora_Enable();
}


/*******************************************************************************
* Function Name: Impresora_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call Impresora_Init() because
*  the Impresora_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Impresora_Init(void) 
{
    #if(Impresora_RX_ENABLED || Impresora_HD_ENABLED)

        #if (Impresora_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(Impresora_RX_VECT_NUM, &Impresora_RXISR);
            CyIntSetPriority(Impresora_RX_VECT_NUM, Impresora_RX_PRIOR_NUM);
            Impresora_errorStatus = 0u;
        #endif /* (Impresora_RX_INTERRUPT_ENABLED) */

        #if (Impresora_RXHW_ADDRESS_ENABLED)
            Impresora_SetRxAddressMode(Impresora_RX_ADDRESS_MODE);
            Impresora_SetRxAddress1(Impresora_RX_HW_ADDRESS1);
            Impresora_SetRxAddress2(Impresora_RX_HW_ADDRESS2);
        #endif /* End Impresora_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        Impresora_RXBITCTR_PERIOD_REG = Impresora_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        Impresora_RXSTATUS_MASK_REG  = Impresora_INIT_RX_INTERRUPTS_MASK;
    #endif /* End Impresora_RX_ENABLED || Impresora_HD_ENABLED*/

    #if(Impresora_TX_ENABLED)
        #if (Impresora_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(Impresora_TX_VECT_NUM, &Impresora_TXISR);
            CyIntSetPriority(Impresora_TX_VECT_NUM, Impresora_TX_PRIOR_NUM);
        #endif /* (Impresora_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (Impresora_TXCLKGEN_DP)
            Impresora_TXBITCLKGEN_CTR_REG = Impresora_BIT_CENTER;
            Impresora_TXBITCLKTX_COMPLETE_REG = ((Impresora_NUMBER_OF_DATA_BITS +
                        Impresora_NUMBER_OF_START_BIT) * Impresora_OVER_SAMPLE_COUNT) - 1u;
        #else
            Impresora_TXBITCTR_PERIOD_REG = ((Impresora_NUMBER_OF_DATA_BITS +
                        Impresora_NUMBER_OF_START_BIT) * Impresora_OVER_SAMPLE_8) - 1u;
        #endif /* End Impresora_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (Impresora_TX_INTERRUPT_ENABLED)
            Impresora_TXSTATUS_MASK_REG = Impresora_TX_STS_FIFO_EMPTY;
        #else
            Impresora_TXSTATUS_MASK_REG = Impresora_INIT_TX_INTERRUPTS_MASK;
        #endif /*End Impresora_TX_INTERRUPT_ENABLED*/

    #endif /* End Impresora_TX_ENABLED */

    #if(Impresora_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        Impresora_WriteControlRegister( \
            (Impresora_ReadControlRegister() & (uint8)~Impresora_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(Impresora_PARITY_TYPE << Impresora_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End Impresora_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: Impresora_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call Impresora_Enable() because the Impresora_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Impresora_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void Impresora_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (Impresora_RX_ENABLED || Impresora_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        Impresora_RXBITCTR_CONTROL_REG |= Impresora_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        Impresora_RXSTATUS_ACTL_REG  |= Impresora_INT_ENABLE;

        #if (Impresora_RX_INTERRUPT_ENABLED)
            Impresora_EnableRxInt();

            #if (Impresora_RXHW_ADDRESS_ENABLED)
                Impresora_rxAddressDetected = 0u;
            #endif /* (Impresora_RXHW_ADDRESS_ENABLED) */
        #endif /* (Impresora_RX_INTERRUPT_ENABLED) */
    #endif /* (Impresora_RX_ENABLED || Impresora_HD_ENABLED) */

    #if(Impresora_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!Impresora_TXCLKGEN_DP)
            Impresora_TXBITCTR_CONTROL_REG |= Impresora_CNTR_ENABLE;
        #endif /* End Impresora_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        Impresora_TXSTATUS_ACTL_REG |= Impresora_INT_ENABLE;
        #if (Impresora_TX_INTERRUPT_ENABLED)
            Impresora_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            Impresora_EnableTxInt();
        #endif /* (Impresora_TX_INTERRUPT_ENABLED) */
     #endif /* (Impresora_TX_INTERRUPT_ENABLED) */

    #if (Impresora_INTERNAL_CLOCK_USED)
        Impresora_IntClock_Start();  /* Enable the clock */
    #endif /* (Impresora_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Impresora_Stop
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
void Impresora_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (Impresora_RX_ENABLED || Impresora_HD_ENABLED)
        Impresora_RXBITCTR_CONTROL_REG &= (uint8) ~Impresora_CNTR_ENABLE;
    #endif /* (Impresora_RX_ENABLED || Impresora_HD_ENABLED) */

    #if (Impresora_TX_ENABLED)
        #if(!Impresora_TXCLKGEN_DP)
            Impresora_TXBITCTR_CONTROL_REG &= (uint8) ~Impresora_CNTR_ENABLE;
        #endif /* (!Impresora_TXCLKGEN_DP) */
    #endif /* (Impresora_TX_ENABLED) */

    #if (Impresora_INTERNAL_CLOCK_USED)
        Impresora_IntClock_Stop();   /* Disable the clock */
    #endif /* (Impresora_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (Impresora_RX_ENABLED || Impresora_HD_ENABLED)
        Impresora_RXSTATUS_ACTL_REG  &= (uint8) ~Impresora_INT_ENABLE;

        #if (Impresora_RX_INTERRUPT_ENABLED)
            Impresora_DisableRxInt();
        #endif /* (Impresora_RX_INTERRUPT_ENABLED) */
    #endif /* (Impresora_RX_ENABLED || Impresora_HD_ENABLED) */

    #if (Impresora_TX_ENABLED)
        Impresora_TXSTATUS_ACTL_REG &= (uint8) ~Impresora_INT_ENABLE;

        #if (Impresora_TX_INTERRUPT_ENABLED)
            Impresora_DisableTxInt();
        #endif /* (Impresora_TX_INTERRUPT_ENABLED) */
    #endif /* (Impresora_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Impresora_ReadControlRegister
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
uint8 Impresora_ReadControlRegister(void) 
{
    #if (Impresora_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(Impresora_CONTROL_REG);
    #endif /* (Impresora_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: Impresora_WriteControlRegister
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
void  Impresora_WriteControlRegister(uint8 control) 
{
    #if (Impresora_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       Impresora_CONTROL_REG = control;
    #endif /* (Impresora_CONTROL_REG_REMOVED) */
}


#if(Impresora_RX_ENABLED || Impresora_HD_ENABLED)
    /*******************************************************************************
    * Function Name: Impresora_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      Impresora_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      Impresora_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      Impresora_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      Impresora_RX_STS_BREAK            Interrupt on break.
    *      Impresora_RX_STS_OVERRUN          Interrupt on overrun error.
    *      Impresora_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      Impresora_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Impresora_SetRxInterruptMode(uint8 intSrc) 
    {
        Impresora_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: Impresora_ReadRxData
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
    *  Impresora_rxBuffer - RAM buffer pointer for save received data.
    *  Impresora_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Impresora_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Impresora_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Impresora_ReadRxData(void) 
    {
        uint8 rxData;

    #if (Impresora_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Impresora_DisableRxInt();

        locRxBufferRead  = Impresora_rxBufferRead;
        locRxBufferWrite = Impresora_rxBufferWrite;

        if( (Impresora_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Impresora_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= Impresora_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Impresora_rxBufferRead = locRxBufferRead;

            if(Impresora_rxBufferLoopDetect != 0u)
            {
                Impresora_rxBufferLoopDetect = 0u;
                #if ((Impresora_RX_INTERRUPT_ENABLED) && (Impresora_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Impresora_HD_ENABLED )
                        if((Impresora_CONTROL_REG & Impresora_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Impresora_RXSTATUS_MASK_REG  |= Impresora_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Impresora_RXSTATUS_MASK_REG  |= Impresora_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Impresora_HD_ENABLED */
                #endif /* ((Impresora_RX_INTERRUPT_ENABLED) && (Impresora_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = Impresora_RXDATA_REG;
        }

        Impresora_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = Impresora_RXDATA_REG;

    #endif /* (Impresora_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Impresora_ReadRxStatus
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
    *  Impresora_RX_STS_FIFO_NOTEMPTY.
    *  Impresora_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  Impresora_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   Impresora_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   Impresora_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 Impresora_ReadRxStatus(void) 
    {
        uint8 status;

        status = Impresora_RXSTATUS_REG & Impresora_RX_HW_MASK;

    #if (Impresora_RX_INTERRUPT_ENABLED)
        if(Impresora_rxBufferOverflow != 0u)
        {
            status |= Impresora_RX_STS_SOFT_BUFF_OVER;
            Impresora_rxBufferOverflow = 0u;
        }
    #endif /* (Impresora_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: Impresora_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. Impresora_GetChar() is
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
    *  Impresora_rxBuffer - RAM buffer pointer for save received data.
    *  Impresora_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Impresora_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Impresora_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Impresora_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (Impresora_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Impresora_DisableRxInt();

        locRxBufferRead  = Impresora_rxBufferRead;
        locRxBufferWrite = Impresora_rxBufferWrite;

        if( (Impresora_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Impresora_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= Impresora_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Impresora_rxBufferRead = locRxBufferRead;

            if(Impresora_rxBufferLoopDetect != 0u)
            {
                Impresora_rxBufferLoopDetect = 0u;
                #if( (Impresora_RX_INTERRUPT_ENABLED) && (Impresora_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Impresora_HD_ENABLED )
                        if((Impresora_CONTROL_REG & Impresora_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Impresora_RXSTATUS_MASK_REG |= Impresora_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Impresora_RXSTATUS_MASK_REG |= Impresora_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Impresora_HD_ENABLED */
                #endif /* Impresora_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = Impresora_RXSTATUS_REG;
            if((rxStatus & Impresora_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = Impresora_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (Impresora_RX_STS_BREAK | Impresora_RX_STS_PAR_ERROR |
                                Impresora_RX_STS_STOP_ERROR | Impresora_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        Impresora_EnableRxInt();

    #else

        rxStatus =Impresora_RXSTATUS_REG;
        if((rxStatus & Impresora_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = Impresora_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (Impresora_RX_STS_BREAK | Impresora_RX_STS_PAR_ERROR |
                            Impresora_RX_STS_STOP_ERROR | Impresora_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (Impresora_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Impresora_GetByte
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
    uint16 Impresora_GetByte(void) 
    {
        
    #if (Impresora_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        Impresora_DisableRxInt();
        locErrorStatus = (uint16)Impresora_errorStatus;
        Impresora_errorStatus = 0u;
        Impresora_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | Impresora_ReadRxData() );
    #else
        return ( ((uint16)Impresora_ReadRxStatus() << 8u) | Impresora_ReadRxData() );
    #endif /* Impresora_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: Impresora_GetRxBufferSize
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
    *  Impresora_rxBufferWrite - used to calculate left bytes.
    *  Impresora_rxBufferRead - used to calculate left bytes.
    *  Impresora_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 Impresora_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Impresora_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        Impresora_DisableRxInt();

        if(Impresora_rxBufferRead == Impresora_rxBufferWrite)
        {
            if(Impresora_rxBufferLoopDetect != 0u)
            {
                size = Impresora_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(Impresora_rxBufferRead < Impresora_rxBufferWrite)
        {
            size = (Impresora_rxBufferWrite - Impresora_rxBufferRead);
        }
        else
        {
            size = (Impresora_RX_BUFFER_SIZE - Impresora_rxBufferRead) + Impresora_rxBufferWrite;
        }

        Impresora_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((Impresora_RXSTATUS_REG & Impresora_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (Impresora_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Impresora_ClearRxBuffer
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
    *  Impresora_rxBufferWrite - cleared to zero.
    *  Impresora_rxBufferRead - cleared to zero.
    *  Impresora_rxBufferLoopDetect - cleared to zero.
    *  Impresora_rxBufferOverflow - cleared to zero.
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
    void Impresora_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        Impresora_RXDATA_AUX_CTL_REG |= (uint8)  Impresora_RX_FIFO_CLR;
        Impresora_RXDATA_AUX_CTL_REG &= (uint8) ~Impresora_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Impresora_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Impresora_DisableRxInt();

        Impresora_rxBufferRead = 0u;
        Impresora_rxBufferWrite = 0u;
        Impresora_rxBufferLoopDetect = 0u;
        Impresora_rxBufferOverflow = 0u;

        Impresora_EnableRxInt();

    #endif /* (Impresora_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: Impresora_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  Impresora__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  Impresora__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  Impresora__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  Impresora__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  Impresora__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Impresora_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  Impresora_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void Impresora_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(Impresora_RXHW_ADDRESS_ENABLED)
            #if(Impresora_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* Impresora_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = Impresora_CONTROL_REG & (uint8)~Impresora_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << Impresora_CTRL_RXADDR_MODE0_SHIFT);
                Impresora_CONTROL_REG = tmpCtrl;

                #if(Impresora_RX_INTERRUPT_ENABLED && \
                   (Impresora_RXBUFFERSIZE > Impresora_FIFO_LENGTH) )
                    Impresora_rxAddressMode = addressMode;
                    Impresora_rxAddressDetected = 0u;
                #endif /* End Impresora_RXBUFFERSIZE > Impresora_FIFO_LENGTH*/
            #endif /* End Impresora_CONTROL_REG_REMOVED */
        #else /* Impresora_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End Impresora_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Impresora_SetRxAddress1
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
    void Impresora_SetRxAddress1(uint8 address) 
    {
        Impresora_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: Impresora_SetRxAddress2
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
    void Impresora_SetRxAddress2(uint8 address) 
    {
        Impresora_RXADDRESS2_REG = address;
    }

#endif  /* Impresora_RX_ENABLED || Impresora_HD_ENABLED*/


#if( (Impresora_TX_ENABLED) || (Impresora_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: Impresora_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   Impresora_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   Impresora_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   Impresora_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   Impresora_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Impresora_SetTxInterruptMode(uint8 intSrc) 
    {
        Impresora_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: Impresora_WriteTxData
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
    *  Impresora_txBuffer - RAM buffer pointer for save data for transmission
    *  Impresora_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  Impresora_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  Impresora_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Impresora_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Impresora_initVar != 0u)
        {
        #if (Impresora_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            Impresora_DisableTxInt();

            if( (Impresora_txBufferRead == Impresora_txBufferWrite) &&
                ((Impresora_TXSTATUS_REG & Impresora_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                Impresora_TXDATA_REG = txDataByte;
            }
            else
            {
                if(Impresora_txBufferWrite >= Impresora_TX_BUFFER_SIZE)
                {
                    Impresora_txBufferWrite = 0u;
                }

                Impresora_txBuffer[Impresora_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                Impresora_txBufferWrite++;
            }

            Impresora_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            Impresora_TXDATA_REG = txDataByte;

        #endif /*(Impresora_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: Impresora_ReadTxStatus
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
    uint8 Impresora_ReadTxStatus(void) 
    {
        return(Impresora_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: Impresora_PutChar
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
    *  Impresora_txBuffer - RAM buffer pointer for save data for transmission
    *  Impresora_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  Impresora_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  Impresora_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void Impresora_PutChar(uint8 txDataByte) 
    {
    #if (Impresora_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((Impresora_TX_BUFFER_SIZE > Impresora_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            Impresora_DisableTxInt();
        #endif /* (Impresora_TX_BUFFER_SIZE > Impresora_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = Impresora_txBufferWrite;
            locTxBufferRead  = Impresora_txBufferRead;

        #if ((Impresora_TX_BUFFER_SIZE > Impresora_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            Impresora_EnableTxInt();
        #endif /* (Impresora_TX_BUFFER_SIZE > Impresora_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(Impresora_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((Impresora_TXSTATUS_REG & Impresora_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            Impresora_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= Impresora_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            Impresora_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((Impresora_TX_BUFFER_SIZE > Impresora_MAX_BYTE_VALUE) && (CY_PSOC3))
            Impresora_DisableTxInt();
        #endif /* (Impresora_TX_BUFFER_SIZE > Impresora_MAX_BYTE_VALUE) && (CY_PSOC3) */

            Impresora_txBufferWrite = locTxBufferWrite;

        #if ((Impresora_TX_BUFFER_SIZE > Impresora_MAX_BYTE_VALUE) && (CY_PSOC3))
            Impresora_EnableTxInt();
        #endif /* (Impresora_TX_BUFFER_SIZE > Impresora_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (Impresora_TXSTATUS_REG & Impresora_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                Impresora_SetPendingTxInt();
            }
        }

    #else

        while((Impresora_TXSTATUS_REG & Impresora_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        Impresora_TXDATA_REG = txDataByte;

    #endif /* Impresora_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Impresora_PutString
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
    *  Impresora_initVar - checked to identify that the component has been
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
    void Impresora_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Impresora_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                Impresora_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Impresora_PutArray
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
    *  Impresora_initVar - checked to identify that the component has been
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
    void Impresora_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Impresora_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                Impresora_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Impresora_PutCRLF
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
    *  Impresora_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Impresora_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(Impresora_initVar != 0u)
        {
            Impresora_PutChar(txDataByte);
            Impresora_PutChar(0x0Du);
            Impresora_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: Impresora_GetTxBufferSize
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
    *  Impresora_txBufferWrite - used to calculate left space.
    *  Impresora_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 Impresora_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Impresora_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Impresora_DisableTxInt();

        if(Impresora_txBufferRead == Impresora_txBufferWrite)
        {
            size = 0u;
        }
        else if(Impresora_txBufferRead < Impresora_txBufferWrite)
        {
            size = (Impresora_txBufferWrite - Impresora_txBufferRead);
        }
        else
        {
            size = (Impresora_TX_BUFFER_SIZE - Impresora_txBufferRead) +
                    Impresora_txBufferWrite;
        }

        Impresora_EnableTxInt();

    #else

        size = Impresora_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & Impresora_TX_STS_FIFO_FULL) != 0u)
        {
            size = Impresora_FIFO_LENGTH;
        }
        else if((size & Impresora_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (Impresora_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: Impresora_ClearTxBuffer
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
    *  Impresora_txBufferWrite - cleared to zero.
    *  Impresora_txBufferRead - cleared to zero.
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
    void Impresora_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        Impresora_TXDATA_AUX_CTL_REG |= (uint8)  Impresora_TX_FIFO_CLR;
        Impresora_TXDATA_AUX_CTL_REG &= (uint8) ~Impresora_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Impresora_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Impresora_DisableTxInt();

        Impresora_txBufferRead = 0u;
        Impresora_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        Impresora_EnableTxInt();

    #endif /* (Impresora_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Impresora_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   Impresora_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   Impresora_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   Impresora_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   Impresora_SEND_WAIT_REINIT - Performs both options: 
    *      Impresora_SEND_BREAK and Impresora_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Impresora_initVar - checked to identify that the component has been
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
    *     When interrupt appear with Impresora_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The Impresora_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void Impresora_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(Impresora_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(Impresora_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == Impresora_SEND_BREAK) ||
                (retMode == Impresora_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                Impresora_WriteControlRegister(Impresora_ReadControlRegister() |
                                                      Impresora_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                Impresora_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Impresora_TXSTATUS_REG;
                }
                while((tmpStat & Impresora_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Impresora_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Impresora_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Impresora_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Impresora_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Impresora_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Impresora_REINIT) ||
                (retMode == Impresora_SEND_WAIT_REINIT) )
            {
                Impresora_WriteControlRegister(Impresora_ReadControlRegister() &
                                              (uint8)~Impresora_CTRL_HD_SEND_BREAK);
            }

        #else /* Impresora_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == Impresora_SEND_BREAK) ||
                (retMode == Impresora_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (Impresora_PARITY_TYPE != Impresora__B_UART__NONE_REVB) || \
                                    (Impresora_PARITY_TYPE_SW != 0u) )
                    Impresora_WriteControlRegister(Impresora_ReadControlRegister() |
                                                          Impresora_CTRL_HD_SEND_BREAK);
                #endif /* End Impresora_PARITY_TYPE != Impresora__B_UART__NONE_REVB  */

                #if(Impresora_TXCLKGEN_DP)
                    txPeriod = Impresora_TXBITCLKTX_COMPLETE_REG;
                    Impresora_TXBITCLKTX_COMPLETE_REG = Impresora_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = Impresora_TXBITCTR_PERIOD_REG;
                    Impresora_TXBITCTR_PERIOD_REG = Impresora_TXBITCTR_BREAKBITS8X;
                #endif /* End Impresora_TXCLKGEN_DP */

                /* Send zeros */
                Impresora_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Impresora_TXSTATUS_REG;
                }
                while((tmpStat & Impresora_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Impresora_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Impresora_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Impresora_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Impresora_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Impresora_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Impresora_REINIT) ||
                (retMode == Impresora_SEND_WAIT_REINIT) )
            {

            #if(Impresora_TXCLKGEN_DP)
                Impresora_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                Impresora_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End Impresora_TXCLKGEN_DP */

            #if( (Impresora_PARITY_TYPE != Impresora__B_UART__NONE_REVB) || \
                 (Impresora_PARITY_TYPE_SW != 0u) )
                Impresora_WriteControlRegister(Impresora_ReadControlRegister() &
                                                      (uint8) ~Impresora_CTRL_HD_SEND_BREAK);
            #endif /* End Impresora_PARITY_TYPE != NONE */
            }
        #endif    /* End Impresora_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: Impresora_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       Impresora_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       Impresora_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears Impresora_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void Impresora_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( Impresora_CONTROL_REG_REMOVED == 0u )
            Impresora_WriteControlRegister(Impresora_ReadControlRegister() |
                                                  Impresora_CTRL_MARK);
        #endif /* End Impresora_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( Impresora_CONTROL_REG_REMOVED == 0u )
            Impresora_WriteControlRegister(Impresora_ReadControlRegister() &
                                                  (uint8) ~Impresora_CTRL_MARK);
        #endif /* End Impresora_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndImpresora_TX_ENABLED */

#if(Impresora_HD_ENABLED)


    /*******************************************************************************
    * Function Name: Impresora_LoadRxConfig
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
    void Impresora_LoadRxConfig(void) 
    {
        Impresora_WriteControlRegister(Impresora_ReadControlRegister() &
                                                (uint8)~Impresora_CTRL_HD_SEND);
        Impresora_RXBITCTR_PERIOD_REG = Impresora_HD_RXBITCTR_INIT;

    #if (Impresora_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        Impresora_SetRxInterruptMode(Impresora_INIT_RX_INTERRUPTS_MASK);
    #endif /* (Impresora_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Impresora_LoadTxConfig
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
    void Impresora_LoadTxConfig(void) 
    {
    #if (Impresora_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        Impresora_SetRxInterruptMode(0u);
    #endif /* (Impresora_RX_INTERRUPT_ENABLED) */

        Impresora_WriteControlRegister(Impresora_ReadControlRegister() | Impresora_CTRL_HD_SEND);
        Impresora_RXBITCTR_PERIOD_REG = Impresora_HD_TXBITCTR_INIT;
    }

#endif  /* Impresora_HD_ENABLED */


/* [] END OF FILE */
