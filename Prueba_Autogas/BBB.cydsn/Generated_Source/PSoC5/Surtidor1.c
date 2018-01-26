/*******************************************************************************
* File Name: Surtidor1.c
* Version 2.30
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Surtidor1.h"
#include "CyLib.h"
#if(Surtidor1_INTERNAL_CLOCK_USED)
    #include "Surtidor1_IntClock.h"
#endif /* End Surtidor1_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 Surtidor1_initVar = 0u;
#if( Surtidor1_TX_ENABLED && (Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
    volatile uint8 Surtidor1_txBuffer[Surtidor1_TXBUFFERSIZE];
    volatile uint8 Surtidor1_txBufferRead = 0u;
    uint8 Surtidor1_txBufferWrite = 0u;
#endif /* End Surtidor1_TX_ENABLED */
#if( ( Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED ) && \
     (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH) )
    volatile uint8 Surtidor1_rxBuffer[Surtidor1_RXBUFFERSIZE];
    volatile uint8 Surtidor1_rxBufferRead = 0u;
    volatile uint8 Surtidor1_rxBufferWrite = 0u;
    volatile uint8 Surtidor1_rxBufferLoopDetect = 0u;
    volatile uint8 Surtidor1_rxBufferOverflow = 0u;
    #if (Surtidor1_RXHW_ADDRESS_ENABLED)
        volatile uint8 Surtidor1_rxAddressMode = Surtidor1_RXADDRESSMODE;
        volatile uint8 Surtidor1_rxAddressDetected = 0u;
    #endif /* End EnableHWAddress */
#endif /* End Surtidor1_RX_ENABLED */


/*******************************************************************************
* Function Name: Surtidor1_Start
********************************************************************************
*
* Summary:
*  Initialize and Enable the UART component.
*  Enable the clock input to enable operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The Surtidor1_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time UART_Start() is called. This allows for
*  component initialization without re-initialization in all subsequent calls
*  to the Surtidor1_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Surtidor1_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(Surtidor1_initVar == 0u)
    {
        Surtidor1_Init();
        Surtidor1_initVar = 1u;
    }
    Surtidor1_Enable();
}


/*******************************************************************************
* Function Name: Surtidor1_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  Surtidor1_Start().
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Surtidor1_Init(void) 
{
    #if(Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED)

        #if(Surtidor1_RX_INTERRUPT_ENABLED && (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
            /* Set the RX Interrupt. */
            (void)CyIntSetVector(Surtidor1_RX_VECT_NUM, &Surtidor1_RXISR);
            CyIntSetPriority(Surtidor1_RX_VECT_NUM, Surtidor1_RX_PRIOR_NUM);
        #endif /* End Surtidor1_RX_INTERRUPT_ENABLED */

        #if (Surtidor1_RXHW_ADDRESS_ENABLED)
            Surtidor1_SetRxAddressMode(Surtidor1_RXAddressMode);
            Surtidor1_SetRxAddress1(Surtidor1_RXHWADDRESS1);
            Surtidor1_SetRxAddress2(Surtidor1_RXHWADDRESS2);
        #endif /* End Surtidor1_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        Surtidor1_RXBITCTR_PERIOD_REG = Surtidor1_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        Surtidor1_RXSTATUS_MASK_REG  = Surtidor1_INIT_RX_INTERRUPTS_MASK;
    #endif /* End Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED*/

    #if(Surtidor1_TX_ENABLED)
        #if(Surtidor1_TX_INTERRUPT_ENABLED && (Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
            /* Set the TX Interrupt. */
            (void)CyIntSetVector(Surtidor1_TX_VECT_NUM, &Surtidor1_TXISR);
            CyIntSetPriority(Surtidor1_TX_VECT_NUM, Surtidor1_TX_PRIOR_NUM);
        #endif /* End Surtidor1_TX_INTERRUPT_ENABLED */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if(Surtidor1_TXCLKGEN_DP)
            Surtidor1_TXBITCLKGEN_CTR_REG = Surtidor1_BIT_CENTER;
            Surtidor1_TXBITCLKTX_COMPLETE_REG = (Surtidor1_NUMBER_OF_DATA_BITS +
                        Surtidor1_NUMBER_OF_START_BIT) * Surtidor1_OVER_SAMPLE_COUNT;
        #else
            Surtidor1_TXBITCTR_PERIOD_REG = ((Surtidor1_NUMBER_OF_DATA_BITS +
                        Surtidor1_NUMBER_OF_START_BIT) * Surtidor1_OVER_SAMPLE_8) - 1u;
        #endif /* End Surtidor1_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if(Surtidor1_TX_INTERRUPT_ENABLED && (Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
            Surtidor1_TXSTATUS_MASK_REG = Surtidor1_TX_STS_FIFO_EMPTY;
        #else
            Surtidor1_TXSTATUS_MASK_REG = Surtidor1_INIT_TX_INTERRUPTS_MASK;
        #endif /*End Surtidor1_TX_INTERRUPT_ENABLED*/

    #endif /* End Surtidor1_TX_ENABLED */

    #if(Surtidor1_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        Surtidor1_WriteControlRegister( \
            (Surtidor1_ReadControlRegister() & (uint8)~Surtidor1_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(Surtidor1_PARITY_TYPE << Surtidor1_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End Surtidor1_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: Surtidor1_Enable
********************************************************************************
*
* Summary:
*  Enables the UART block operation
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Surtidor1_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void Surtidor1_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if(Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED)
        /*RX Counter (Count7) Enable */
        Surtidor1_RXBITCTR_CONTROL_REG |= Surtidor1_CNTR_ENABLE;
        /* Enable the RX Interrupt. */
        Surtidor1_RXSTATUS_ACTL_REG  |= Surtidor1_INT_ENABLE;
        #if(Surtidor1_RX_INTERRUPT_ENABLED && (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
            CyIntEnable(Surtidor1_RX_VECT_NUM);
            #if (Surtidor1_RXHW_ADDRESS_ENABLED)
                Surtidor1_rxAddressDetected = 0u;
            #endif /* End Surtidor1_RXHW_ADDRESS_ENABLED */
        #endif /* End Surtidor1_RX_INTERRUPT_ENABLED */
    #endif /* End Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED*/

    #if(Surtidor1_TX_ENABLED)
        /*TX Counter (DP/Count7) Enable */
        #if(!Surtidor1_TXCLKGEN_DP)
            Surtidor1_TXBITCTR_CONTROL_REG |= Surtidor1_CNTR_ENABLE;
        #endif /* End Surtidor1_TXCLKGEN_DP */
        /* Enable the TX Interrupt. */
        Surtidor1_TXSTATUS_ACTL_REG |= Surtidor1_INT_ENABLE;
        #if(Surtidor1_TX_INTERRUPT_ENABLED && (Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
            CyIntEnable(Surtidor1_TX_VECT_NUM);
        #endif /* End Surtidor1_TX_INTERRUPT_ENABLED*/
     #endif /* End Surtidor1_TX_ENABLED */

    #if(Surtidor1_INTERNAL_CLOCK_USED)
        /* Enable the clock. */
        Surtidor1_IntClock_Start();
    #endif /* End Surtidor1_INTERNAL_CLOCK_USED */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Surtidor1_Stop
********************************************************************************
*
* Summary:
*  Disable the UART component
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Surtidor1_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if(Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED)
        Surtidor1_RXBITCTR_CONTROL_REG &= (uint8)~Surtidor1_CNTR_ENABLE;
    #endif /* End Surtidor1_RX_ENABLED */

    #if(Surtidor1_TX_ENABLED)
        #if(!Surtidor1_TXCLKGEN_DP)
            Surtidor1_TXBITCTR_CONTROL_REG &= (uint8)~Surtidor1_CNTR_ENABLE;
        #endif /* End Surtidor1_TXCLKGEN_DP */
    #endif /* Surtidor1_TX_ENABLED */

    #if(Surtidor1_INTERNAL_CLOCK_USED)
        /* Disable the clock. */
        Surtidor1_IntClock_Stop();
    #endif /* End Surtidor1_INTERNAL_CLOCK_USED */

    /* Disable internal interrupt component */
    #if(Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED)
        Surtidor1_RXSTATUS_ACTL_REG  &= (uint8)~Surtidor1_INT_ENABLE;
        #if(Surtidor1_RX_INTERRUPT_ENABLED && (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
            Surtidor1_DisableRxInt();
        #endif /* End Surtidor1_RX_INTERRUPT_ENABLED */
    #endif /* End Surtidor1_RX_ENABLED */

    #if(Surtidor1_TX_ENABLED)
        Surtidor1_TXSTATUS_ACTL_REG &= (uint8)~Surtidor1_INT_ENABLE;
        #if(Surtidor1_TX_INTERRUPT_ENABLED && (Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
            Surtidor1_DisableTxInt();
        #endif /* End Surtidor1_TX_INTERRUPT_ENABLED */
    #endif /* End Surtidor1_TX_ENABLED */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Surtidor1_ReadControlRegister
********************************************************************************
*
* Summary:
*  Read the current state of the control register
*
* Parameters:
*  None.
*
* Return:
*  Current state of the control register.
*
*******************************************************************************/
uint8 Surtidor1_ReadControlRegister(void) 
{
    #if( Surtidor1_CONTROL_REG_REMOVED )
        return(0u);
    #else
        return(Surtidor1_CONTROL_REG);
    #endif /* End Surtidor1_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Surtidor1_WriteControlRegister
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
void  Surtidor1_WriteControlRegister(uint8 control) 
{
    #if( Surtidor1_CONTROL_REG_REMOVED )
        if(control != 0u) { }      /* release compiler warning */
    #else
       Surtidor1_CONTROL_REG = control;
    #endif /* End Surtidor1_CONTROL_REG_REMOVED */
}


#if(Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED)

    #if(Surtidor1_RX_INTERRUPT_ENABLED)

        /*******************************************************************************
        * Function Name: Surtidor1_EnableRxInt
        ********************************************************************************
        *
        * Summary:
        *  Enable RX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Enable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void Surtidor1_EnableRxInt(void) 
        {
            CyIntEnable(Surtidor1_RX_VECT_NUM);
        }


        /*******************************************************************************
        * Function Name: Surtidor1_DisableRxInt
        ********************************************************************************
        *
        * Summary:
        *  Disable RX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Disable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void Surtidor1_DisableRxInt(void) 
        {
            CyIntDisable(Surtidor1_RX_VECT_NUM);
        }

    #endif /* Surtidor1_RX_INTERRUPT_ENABLED */


    /*******************************************************************************
    * Function Name: Surtidor1_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configure which status bits trigger an interrupt event
    *
    * Parameters:
    *  IntSrc:  An or'd combination of the desired status bit masks (defined in
    *           the header file)
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Surtidor1_SetRxInterruptMode(uint8 intSrc) 
    {
        Surtidor1_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: Surtidor1_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns data in RX Data register without checking status register to
    *  determine if data is valid
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  Surtidor1_rxBuffer - RAM buffer pointer for save received data.
    *  Surtidor1_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Surtidor1_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Surtidor1_rxBufferLoopDetect - creared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Surtidor1_ReadRxData(void) 
    {
        uint8 rxData;

        #if(Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH)
            uint8 loc_rxBufferRead;
            uint8 loc_rxBufferWrite;
            /* Protect variables that could change on interrupt. */
            /* Disable Rx interrupt. */
            #if(Surtidor1_RX_INTERRUPT_ENABLED)
                Surtidor1_DisableRxInt();
            #endif /* Surtidor1_RX_INTERRUPT_ENABLED */
            loc_rxBufferRead = Surtidor1_rxBufferRead;
            loc_rxBufferWrite = Surtidor1_rxBufferWrite;

            if( (Surtidor1_rxBufferLoopDetect != 0u) || (loc_rxBufferRead != loc_rxBufferWrite) )
            {
                rxData = Surtidor1_rxBuffer[loc_rxBufferRead];
                loc_rxBufferRead++;

                if(loc_rxBufferRead >= Surtidor1_RXBUFFERSIZE)
                {
                    loc_rxBufferRead = 0u;
                }
                /* Update the real pointer */
                Surtidor1_rxBufferRead = loc_rxBufferRead;

                if(Surtidor1_rxBufferLoopDetect != 0u )
                {
                    Surtidor1_rxBufferLoopDetect = 0u;
                    #if( (Surtidor1_RX_INTERRUPT_ENABLED) && (Surtidor1_FLOW_CONTROL != 0u) && \
                         (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH) )
                        /* When Hardware Flow Control selected - return RX mask */
                        #if( Surtidor1_HD_ENABLED )
                            if((Surtidor1_CONTROL_REG & Surtidor1_CTRL_HD_SEND) == 0u)
                            {   /* In Half duplex mode return RX mask only in RX
                                *  configuration set, otherwise
                                *  mask will be returned in LoadRxConfig() API.
                                */
                                Surtidor1_RXSTATUS_MASK_REG  |= Surtidor1_RX_STS_FIFO_NOTEMPTY;
                            }
                        #else
                            Surtidor1_RXSTATUS_MASK_REG  |= Surtidor1_RX_STS_FIFO_NOTEMPTY;
                        #endif /* end Surtidor1_HD_ENABLED */
                    #endif /* Surtidor1_RX_INTERRUPT_ENABLED and Hardware flow control*/
                }
            }
            else
            {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit*/
                rxData = Surtidor1_RXDATA_REG;
            }

            /* Enable Rx interrupt. */
            #if(Surtidor1_RX_INTERRUPT_ENABLED)
                Surtidor1_EnableRxInt();
            #endif /* End Surtidor1_RX_INTERRUPT_ENABLED */

        #else /* Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

            /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit*/
            rxData = Surtidor1_RXDATA_REG;

        #endif /* Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Surtidor1_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Read the current state of the status register
    *  And detect software buffer overflow.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Global Variables:
    *  Surtidor1_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn?t free space in
    *   Surtidor1_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   Surtidor1_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 Surtidor1_ReadRxStatus(void) 
    {
        uint8 status;

        status = Surtidor1_RXSTATUS_REG & Surtidor1_RX_HW_MASK;

        #if(Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH)
            if( Surtidor1_rxBufferOverflow != 0u )
            {
                status |= Surtidor1_RX_STS_SOFT_BUFF_OVER;
                Surtidor1_rxBufferOverflow = 0u;
            }
        #endif /* Surtidor1_RXBUFFERSIZE */

        return(status);
    }


    /*******************************************************************************
    * Function Name: Surtidor1_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, if data is not available or an error
    *  condition exists, zero is returned; otherwise, character is read and
    *  returned.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  Surtidor1_rxBuffer - RAM buffer pointer for save received data.
    *  Surtidor1_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Surtidor1_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Surtidor1_rxBufferLoopDetect - creared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Surtidor1_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

        #if(Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH)
            uint8 loc_rxBufferRead;
            uint8 loc_rxBufferWrite;
            /* Protect variables that could change on interrupt. */
            /* Disable Rx interrupt. */
            #if(Surtidor1_RX_INTERRUPT_ENABLED)
                Surtidor1_DisableRxInt();
            #endif /* Surtidor1_RX_INTERRUPT_ENABLED */
            loc_rxBufferRead = Surtidor1_rxBufferRead;
            loc_rxBufferWrite = Surtidor1_rxBufferWrite;

            if( (Surtidor1_rxBufferLoopDetect != 0u) || (loc_rxBufferRead != loc_rxBufferWrite) )
            {
                rxData = Surtidor1_rxBuffer[loc_rxBufferRead];
                loc_rxBufferRead++;
                if(loc_rxBufferRead >= Surtidor1_RXBUFFERSIZE)
                {
                    loc_rxBufferRead = 0u;
                }
                /* Update the real pointer */
                Surtidor1_rxBufferRead = loc_rxBufferRead;

                if(Surtidor1_rxBufferLoopDetect > 0u )
                {
                    Surtidor1_rxBufferLoopDetect = 0u;
                    #if( (Surtidor1_RX_INTERRUPT_ENABLED) && (Surtidor1_FLOW_CONTROL != 0u) )
                        /* When Hardware Flow Control selected - return RX mask */
                        #if( Surtidor1_HD_ENABLED )
                            if((Surtidor1_CONTROL_REG & Surtidor1_CTRL_HD_SEND) == 0u)
                            {   /* In Half duplex mode return RX mask only if
                                *  RX configuration set, otherwise
                                *  mask will be returned in LoadRxConfig() API.
                                */
                                Surtidor1_RXSTATUS_MASK_REG  |= Surtidor1_RX_STS_FIFO_NOTEMPTY;
                            }
                        #else
                            Surtidor1_RXSTATUS_MASK_REG  |= Surtidor1_RX_STS_FIFO_NOTEMPTY;
                        #endif /* end Surtidor1_HD_ENABLED */
                    #endif /* Surtidor1_RX_INTERRUPT_ENABLED and Hardware flow control*/
                }

            }
            else
            {   rxStatus = Surtidor1_RXSTATUS_REG;
                if((rxStatus & Surtidor1_RX_STS_FIFO_NOTEMPTY) != 0u)
                {   /* Read received data from FIFO*/
                    rxData = Surtidor1_RXDATA_REG;
                    /*Check status on error*/
                    if((rxStatus & (Surtidor1_RX_STS_BREAK | Surtidor1_RX_STS_PAR_ERROR |
                                   Surtidor1_RX_STS_STOP_ERROR | Surtidor1_RX_STS_OVERRUN)) != 0u)
                    {
                        rxData = 0u;
                    }
                }
            }

            /* Enable Rx interrupt. */
            #if(Surtidor1_RX_INTERRUPT_ENABLED)
                Surtidor1_EnableRxInt();
            #endif /* Surtidor1_RX_INTERRUPT_ENABLED */

        #else /* Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

            rxStatus =Surtidor1_RXSTATUS_REG;
            if((rxStatus & Surtidor1_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO*/
                rxData = Surtidor1_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (Surtidor1_RX_STS_BREAK | Surtidor1_RX_STS_PAR_ERROR |
                               Surtidor1_RX_STS_STOP_ERROR | Surtidor1_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        #endif /* Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Surtidor1_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Grab the next available byte of data from the recieve FIFO
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains Status Register and LSB contains UART RX data
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 Surtidor1_GetByte(void) 
    {
        return ( ((uint16)Surtidor1_ReadRxStatus() << 8u) | Surtidor1_ReadRxData() );
    }


    /*******************************************************************************
    * Function Name: Surtidor1_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Determine the amount of bytes left in the RX buffer and return the count in
    *  bytes
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Integer count of the number of bytes left
    *  in the RX buffer
    *
    * Global Variables:
    *  Surtidor1_rxBufferWrite - used to calculate left bytes.
    *  Surtidor1_rxBufferRead - used to calculate left bytes.
    *  Surtidor1_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 Surtidor1_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

        #if(Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH)

            /* Disable Rx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(Surtidor1_RX_INTERRUPT_ENABLED)
                Surtidor1_DisableRxInt();
            #endif /* Surtidor1_RX_INTERRUPT_ENABLED */

            if(Surtidor1_rxBufferRead == Surtidor1_rxBufferWrite)
            {
                if(Surtidor1_rxBufferLoopDetect > 0u)
                {
                    size = Surtidor1_RXBUFFERSIZE;
                }
                else
                {
                    size = 0u;
                }
            }
            else if(Surtidor1_rxBufferRead < Surtidor1_rxBufferWrite)
            {
                size = (Surtidor1_rxBufferWrite - Surtidor1_rxBufferRead);
            }
            else
            {
                size = (Surtidor1_RXBUFFERSIZE - Surtidor1_rxBufferRead) + Surtidor1_rxBufferWrite;
            }

            /* Enable Rx interrupt. */
            #if(Surtidor1_RX_INTERRUPT_ENABLED)
                Surtidor1_EnableRxInt();
            #endif /* End Surtidor1_RX_INTERRUPT_ENABLED */

        #else /* Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

            /* We can only know if there is data in the fifo. */
            size = ((Surtidor1_RXSTATUS_REG & Surtidor1_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

        #endif /* End Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Surtidor1_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the RX RAM buffer by setting the read and write pointers both to zero.
    *  Clears hardware RX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor1_rxBufferWrite - cleared to zero.
    *  Surtidor1_rxBufferRead - cleared to zero.
    *  Surtidor1_rxBufferLoopDetect - cleared to zero.
    *  Surtidor1_rxBufferOverflow - cleared to zero.
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
    *******************************************************************************/
    void Surtidor1_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* clear the HW FIFO */
        /* Enter critical section */
        enableInterrupts = CyEnterCriticalSection();
        Surtidor1_RXDATA_AUX_CTL_REG |=  Surtidor1_RX_FIFO_CLR;
        Surtidor1_RXDATA_AUX_CTL_REG &= (uint8)~Surtidor1_RX_FIFO_CLR;
        /* Exit critical section */
        CyExitCriticalSection(enableInterrupts);

        #if(Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH)
            /* Disable Rx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(Surtidor1_RX_INTERRUPT_ENABLED)
                Surtidor1_DisableRxInt();
            #endif /* End Surtidor1_RX_INTERRUPT_ENABLED */

            Surtidor1_rxBufferRead = 0u;
            Surtidor1_rxBufferWrite = 0u;
            Surtidor1_rxBufferLoopDetect = 0u;
            Surtidor1_rxBufferOverflow = 0u;

            /* Enable Rx interrupt. */
            #if(Surtidor1_RX_INTERRUPT_ENABLED)
                Surtidor1_EnableRxInt();
            #endif /* End Surtidor1_RX_INTERRUPT_ENABLED */
        #endif /* End Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

    }


    /*******************************************************************************
    * Function Name: Surtidor1_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Set the receive addressing mode
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  Surtidor1__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  Surtidor1__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  Surtidor1__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  Surtidor1__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  Surtidor1__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor1_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  Surtidor1_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void Surtidor1_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(Surtidor1_RXHW_ADDRESS_ENABLED)
            #if(Surtidor1_CONTROL_REG_REMOVED)
                if(addressMode != 0u) { }     /* release compiler warning */
            #else /* Surtidor1_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = Surtidor1_CONTROL_REG & (uint8)~Surtidor1_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << Surtidor1_CTRL_RXADDR_MODE0_SHIFT);
                Surtidor1_CONTROL_REG = tmpCtrl;
                #if(Surtidor1_RX_INTERRUPT_ENABLED && \
                   (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH) )
                    Surtidor1_rxAddressMode = addressMode;
                    Surtidor1_rxAddressDetected = 0u;
                #endif /* End Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH*/
            #endif /* End Surtidor1_CONTROL_REG_REMOVED */
        #else /* Surtidor1_RXHW_ADDRESS_ENABLED */
            if(addressMode != 0u) { }     /* release compiler warning */
        #endif /* End Surtidor1_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Surtidor1_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Set the first hardware address compare value
    *
    * Parameters:
    *  address
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void Surtidor1_SetRxAddress1(uint8 address) 

    {
        Surtidor1_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: Surtidor1_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Set the second hardware address compare value
    *
    * Parameters:
    *  address
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void Surtidor1_SetRxAddress2(uint8 address) 
    {
        Surtidor1_RXADDRESS2_REG = address;
    }

#endif  /* Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED*/


#if( (Surtidor1_TX_ENABLED) || (Surtidor1_HD_ENABLED) )

    #if(Surtidor1_TX_INTERRUPT_ENABLED)

        /*******************************************************************************
        * Function Name: Surtidor1_EnableTxInt
        ********************************************************************************
        *
        * Summary:
        *  Enable TX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Enable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void Surtidor1_EnableTxInt(void) 
        {
            CyIntEnable(Surtidor1_TX_VECT_NUM);
        }


        /*******************************************************************************
        * Function Name: Surtidor1_DisableTxInt
        ********************************************************************************
        *
        * Summary:
        *  Disable TX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Disable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void Surtidor1_DisableTxInt(void) 
        {
            CyIntDisable(Surtidor1_TX_VECT_NUM);
        }

    #endif /* Surtidor1_TX_INTERRUPT_ENABLED */


    /*******************************************************************************
    * Function Name: Surtidor1_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configure which status bits trigger an interrupt event
    *
    * Parameters:
    *  intSrc: An or'd combination of the desired status bit masks (defined in
    *          the header file)
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Surtidor1_SetTxInterruptMode(uint8 intSrc) 
    {
        Surtidor1_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: Surtidor1_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Write a byte of data to the Transmit FIFO or TX buffer to be sent when the
    *  bus is available. WriteTxData sends a byte without checking for buffer room
    *  or status. It is up to the user to separately check status.
    *
    * Parameters:
    *  TXDataByte: byte of data to place in the transmit FIFO
    *
    * Return:
    * void
    *
    * Global Variables:
    *  Surtidor1_txBuffer - RAM buffer pointer for save data for transmission
    *  Surtidor1_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  Surtidor1_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  Surtidor1_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Surtidor1_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Surtidor1_initVar != 0u)
        {
            #if(Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH)

                /* Disable Tx interrupt. */
                /* Protect variables that could change on interrupt. */
                #if(Surtidor1_TX_INTERRUPT_ENABLED)
                    Surtidor1_DisableTxInt();
                #endif /* End Surtidor1_TX_INTERRUPT_ENABLED */

                if( (Surtidor1_txBufferRead == Surtidor1_txBufferWrite) &&
                    ((Surtidor1_TXSTATUS_REG & Surtidor1_TX_STS_FIFO_FULL) == 0u) )
                {
                    /* Add directly to the FIFO. */
                    Surtidor1_TXDATA_REG = txDataByte;
                }
                else
                {
                    if(Surtidor1_txBufferWrite >= Surtidor1_TXBUFFERSIZE)
                    {
                        Surtidor1_txBufferWrite = 0u;
                    }

                    Surtidor1_txBuffer[Surtidor1_txBufferWrite] = txDataByte;

                    /* Add to the software buffer. */
                    Surtidor1_txBufferWrite++;

                }

                /* Enable Tx interrupt. */
                #if(Surtidor1_TX_INTERRUPT_ENABLED)
                    Surtidor1_EnableTxInt();
                #endif /* End Surtidor1_TX_INTERRUPT_ENABLED */

            #else /* Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

                /* Add directly to the FIFO. */
                Surtidor1_TXDATA_REG = txDataByte;

            #endif /* End Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH */
        }
    }


    /*******************************************************************************
    * Function Name: Surtidor1_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Read the status register for the component
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the status register which is clear on read. It is up to
    *  the user to handle all bits in this return value accordingly, even if the bit
    *  was not enabled as an interrupt source the event happened and must be handled
    *  accordingly.
    *
    *******************************************************************************/
    uint8 Surtidor1_ReadTxStatus(void) 
    {
        return(Surtidor1_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: Surtidor1_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Wait to send byte until TX register or buffer has room.
    *
    * Parameters:
    *  txDataByte: The 8-bit data value to send across the UART.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor1_txBuffer - RAM buffer pointer for save data for transmission
    *  Surtidor1_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  Surtidor1_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  Surtidor1_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void Surtidor1_PutChar(uint8 txDataByte) 
    {
            #if(Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH)
                /* The temporary output pointer is used since it takes two instructions
                *  to increment with a wrap, and we can't risk doing that with the real
                *  pointer and getting an interrupt in between instructions.
                */
                uint8 loc_txBufferWrite;
                uint8 loc_txBufferRead;

                do{
                    /* Block if software buffer is full, so we don't overwrite. */
                    #if ((Surtidor1_TXBUFFERSIZE > Surtidor1_MAX_BYTE_VALUE) && (CY_PSOC3))
                        /* Disable TX interrupt to protect variables that could change on interrupt */
                        CyIntDisable(Surtidor1_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                    loc_txBufferWrite = Surtidor1_txBufferWrite;
                    loc_txBufferRead = Surtidor1_txBufferRead;
                    #if ((Surtidor1_TXBUFFERSIZE > Surtidor1_MAX_BYTE_VALUE) && (CY_PSOC3))
                        /* Enable interrupt to continue transmission */
                        CyIntEnable(Surtidor1_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                }while( (loc_txBufferWrite < loc_txBufferRead) ? (loc_txBufferWrite == (loc_txBufferRead - 1u)) :
                                        ((loc_txBufferWrite - loc_txBufferRead) ==
                                        (uint8)(Surtidor1_TXBUFFERSIZE - 1u)) );

                if( (loc_txBufferRead == loc_txBufferWrite) &&
                    ((Surtidor1_TXSTATUS_REG & Surtidor1_TX_STS_FIFO_FULL) == 0u) )
                {
                    /* Add directly to the FIFO. */
                    Surtidor1_TXDATA_REG = txDataByte;
                }
                else
                {
                    if(loc_txBufferWrite >= Surtidor1_TXBUFFERSIZE)
                    {
                        loc_txBufferWrite = 0u;
                    }
                    /* Add to the software buffer. */
                    Surtidor1_txBuffer[loc_txBufferWrite] = txDataByte;
                    loc_txBufferWrite++;

                    /* Finally, update the real output pointer */
                    #if ((Surtidor1_TXBUFFERSIZE > Surtidor1_MAX_BYTE_VALUE) && (CY_PSOC3))
                        CyIntDisable(Surtidor1_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                    Surtidor1_txBufferWrite = loc_txBufferWrite;
                    #if ((Surtidor1_TXBUFFERSIZE > Surtidor1_MAX_BYTE_VALUE) && (CY_PSOC3))
                        CyIntEnable(Surtidor1_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                }

            #else /* Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

                while((Surtidor1_TXSTATUS_REG & Surtidor1_TX_STS_FIFO_FULL) != 0u)
                {
                    ; /* Wait for room in the FIFO. */
                }

                /* Add directly to the FIFO. */
                Surtidor1_TXDATA_REG = txDataByte;

            #endif /* End Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH */
    }


    /*******************************************************************************
    * Function Name: Surtidor1_PutString
    ********************************************************************************
    *
    * Summary:
    *  Write a Sequence of bytes on the Transmit line. Data comes from RAM or ROM.
    *
    * Parameters:
    *  string: char pointer to character string of Data to Send.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor1_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  This function will block if there is not enough memory to place the whole
    *  string, it will block until the entire string has been written to the
    *  transmit buffer.
    *
    *******************************************************************************/
    void Surtidor1_PutString(const char8 string[]) 
    {
        uint16 buf_index = 0u;
        /* If not Initialized then skip this function*/
        if(Surtidor1_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent*/
            while(string[buf_index] != (char8)0)
            {
                Surtidor1_PutChar((uint8)string[buf_index]);
                buf_index++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Surtidor1_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Write a Sequence of bytes on the Transmit line. Data comes from RAM or ROM.
    *
    * Parameters:
    *  string: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of Bytes to be transmitted.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor1_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Surtidor1_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 buf_index = 0u;
        /* If not Initialized then skip this function*/
        if(Surtidor1_initVar != 0u)
        {
            do
            {
                Surtidor1_PutChar(string[buf_index]);
                buf_index++;
            }while(buf_index < byteCount);
        }
    }


    /*******************************************************************************
    * Function Name: Surtidor1_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Write a character and then carriage return and line feed.
    *
    * Parameters:
    *  txDataByte: uint8 Character to send.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor1_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Surtidor1_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Surtidor1_initVar != 0u)
        {
            Surtidor1_PutChar(txDataByte);
            Surtidor1_PutChar(0x0Du);
            Surtidor1_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: Surtidor1_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Determine the amount of space left in the TX buffer and return the count in
    *  bytes
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Integer count of the number of bytes left in the TX buffer
    *
    * Global Variables:
    *  Surtidor1_txBufferWrite - used to calculate left space.
    *  Surtidor1_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 Surtidor1_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

        #if(Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH)

            /* Disable Tx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(Surtidor1_TX_INTERRUPT_ENABLED)
                Surtidor1_DisableTxInt();
            #endif /* End Surtidor1_TX_INTERRUPT_ENABLED */

            if(Surtidor1_txBufferRead == Surtidor1_txBufferWrite)
            {
                size = 0u;
            }
            else if(Surtidor1_txBufferRead < Surtidor1_txBufferWrite)
            {
                size = (Surtidor1_txBufferWrite - Surtidor1_txBufferRead);
            }
            else
            {
                size = (Surtidor1_TXBUFFERSIZE - Surtidor1_txBufferRead) + Surtidor1_txBufferWrite;
            }

            /* Enable Tx interrupt. */
            #if(Surtidor1_TX_INTERRUPT_ENABLED)
                Surtidor1_EnableTxInt();
            #endif /* End Surtidor1_TX_INTERRUPT_ENABLED */

        #else /* Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

            size = Surtidor1_TXSTATUS_REG;

            /* Is the fifo is full. */
            if((size & Surtidor1_TX_STS_FIFO_FULL) != 0u)
            {
                size = Surtidor1_FIFO_LENGTH;
            }
            else if((size & Surtidor1_TX_STS_FIFO_EMPTY) != 0u)
            {
                size = 0u;
            }
            else
            {
                /* We only know there is data in the fifo. */
                size = 1u;
            }

        #endif /* End Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Surtidor1_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the TX RAM buffer by setting the read and write pointers both to zero.
    *  Clears the hardware TX FIFO.  Any data present in the FIFO will not be sent.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor1_txBufferWrite - cleared to zero.
    *  Surtidor1_txBufferRead - cleared to zero.
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
    *  Any received data not read from the RAM buffer will be lost when overwritten.
    *
    *******************************************************************************/
    void Surtidor1_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Enter critical section */
        enableInterrupts = CyEnterCriticalSection();
        /* clear the HW FIFO */
        Surtidor1_TXDATA_AUX_CTL_REG |=  Surtidor1_TX_FIFO_CLR;
        Surtidor1_TXDATA_AUX_CTL_REG &= (uint8)~Surtidor1_TX_FIFO_CLR;
        /* Exit critical section */
        CyExitCriticalSection(enableInterrupts);

        #if(Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH)

            /* Disable Tx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(Surtidor1_TX_INTERRUPT_ENABLED)
                Surtidor1_DisableTxInt();
            #endif /* End Surtidor1_TX_INTERRUPT_ENABLED */

            Surtidor1_txBufferRead = 0u;
            Surtidor1_txBufferWrite = 0u;

            /* Enable Tx interrupt. */
            #if(Surtidor1_TX_INTERRUPT_ENABLED)
                Surtidor1_EnableTxInt();
            #endif /* End Surtidor1_TX_INTERRUPT_ENABLED */

        #endif /* End Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH */
    }


    /*******************************************************************************
    * Function Name: Surtidor1_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Write a Break command to the UART
    *
    * Parameters:
    *  uint8 retMode:  Wait mode,
    *   0 - Initialize registers for Break, sends the Break signal and return
    *       imediately.
    *   1 - Wait until Break sending is complete, reinitialize registers to normal
    *       transmission mode then return.
    *   2 - Reinitialize registers to normal transmission mode then return.
    *   3 - both steps: 0 and 1
    *       init registers for Break, send Break signal
    *       wait until Break sending is complete, reinit registers to normal
    *       transmission mode then return.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor1_initVar - checked to identify that the component has been
    *     initialized.
    *  tx_period - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  Trere are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Funcition will block CPU untill transmition
    *     complete.
    *  2) User may want to use bloking time if UART configured to the low speed
    *     operation
    *     Emample for this case:
    *     SendBreak(0);     - init Break signal transmition
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to init and use the interrupt for complete
    *     break operation.
    *     Example for this case:
    *     Init TX interrupt whith "TX - On TX Complete" parameter
    *     SendBreak(0);     - init Break signal transmition
    *         Add your code here to use CPU time
    *     When interrupt appear with UART_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *   Uses static variable to keep registers configuration.
    *
    *******************************************************************************/
    void Surtidor1_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(Surtidor1_initVar != 0u)
        {
            /*Set the Counter to 13-bits and transmit a 00 byte*/
            /*When that is done then reset the counter value back*/
            uint8 tmpStat;

            #if(Surtidor1_HD_ENABLED) /* Half Duplex mode*/

                if( (retMode == Surtidor1_SEND_BREAK) ||
                    (retMode == Surtidor1_SEND_WAIT_REINIT ) )
                {
                    /* CTRL_HD_SEND_BREAK - sends break bits in HD mode*/
                    Surtidor1_WriteControlRegister(Surtidor1_ReadControlRegister() |
                                                          Surtidor1_CTRL_HD_SEND_BREAK);
                    /* Send zeros*/
                    Surtidor1_TXDATA_REG = 0u;

                    do /*wait until transmit starts*/
                    {
                        tmpStat = Surtidor1_TXSTATUS_REG;
                    }while((tmpStat & Surtidor1_TX_STS_FIFO_EMPTY) != 0u);
                }

                if( (retMode == Surtidor1_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == Surtidor1_SEND_WAIT_REINIT) )
                {
                    do /*wait until transmit complete*/
                    {
                        tmpStat = Surtidor1_TXSTATUS_REG;
                    }while(((uint8)~tmpStat & Surtidor1_TX_STS_COMPLETE) != 0u);
                }

                if( (retMode == Surtidor1_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == Surtidor1_REINIT) ||
                    (retMode == Surtidor1_SEND_WAIT_REINIT) )
                {
                    Surtidor1_WriteControlRegister(Surtidor1_ReadControlRegister() &
                                                  (uint8)~Surtidor1_CTRL_HD_SEND_BREAK);
                }

            #else /* Surtidor1_HD_ENABLED Full Duplex mode */

                static uint8 tx_period;

                if( (retMode == Surtidor1_SEND_BREAK) ||
                    (retMode == Surtidor1_SEND_WAIT_REINIT) )
                {
                    /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode*/
                    #if( (Surtidor1_PARITY_TYPE != Surtidor1__B_UART__NONE_REVB) || \
                                        (Surtidor1_PARITY_TYPE_SW != 0u) )
                        Surtidor1_WriteControlRegister(Surtidor1_ReadControlRegister() |
                                                              Surtidor1_CTRL_HD_SEND_BREAK);
                    #endif /* End Surtidor1_PARITY_TYPE != Surtidor1__B_UART__NONE_REVB  */

                    #if(Surtidor1_TXCLKGEN_DP)
                        tx_period = Surtidor1_TXBITCLKTX_COMPLETE_REG;
                        Surtidor1_TXBITCLKTX_COMPLETE_REG = Surtidor1_TXBITCTR_BREAKBITS;
                    #else
                        tx_period = Surtidor1_TXBITCTR_PERIOD_REG;
                        Surtidor1_TXBITCTR_PERIOD_REG = Surtidor1_TXBITCTR_BREAKBITS8X;
                    #endif /* End Surtidor1_TXCLKGEN_DP */

                    /* Send zeros*/
                    Surtidor1_TXDATA_REG = 0u;

                    do /* wait until transmit starts */
                    {
                        tmpStat = Surtidor1_TXSTATUS_REG;
                    }while((tmpStat & Surtidor1_TX_STS_FIFO_EMPTY) != 0u);
                }

                if( (retMode == Surtidor1_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == Surtidor1_SEND_WAIT_REINIT) )
                {
                    do /*wait until transmit complete*/
                    {
                        tmpStat = Surtidor1_TXSTATUS_REG;
                    }while(((uint8)~tmpStat & Surtidor1_TX_STS_COMPLETE) != 0u);
                }

                if( (retMode == Surtidor1_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == Surtidor1_REINIT) ||
                    (retMode == Surtidor1_SEND_WAIT_REINIT) )
                {

                    #if(Surtidor1_TXCLKGEN_DP)
                        Surtidor1_TXBITCLKTX_COMPLETE_REG = tx_period;
                    #else
                        Surtidor1_TXBITCTR_PERIOD_REG = tx_period;
                    #endif /* End Surtidor1_TXCLKGEN_DP */

                    #if( (Surtidor1_PARITY_TYPE != Surtidor1__B_UART__NONE_REVB) || \
                         (Surtidor1_PARITY_TYPE_SW != 0u) )
                        Surtidor1_WriteControlRegister(Surtidor1_ReadControlRegister() &
                                                      (uint8)~Surtidor1_CTRL_HD_SEND_BREAK);
                    #endif /* End Surtidor1_PARITY_TYPE != NONE */
                }
            #endif    /* End Surtidor1_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: Surtidor1_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Set the transmit addressing mode
    *
    * Parameters:
    *  addressMode: 0 -> Space
    *               1 -> Mark
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void Surtidor1_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable*/
        if(addressMode != 0u)
        {
            #if( Surtidor1_CONTROL_REG_REMOVED == 0u )
                Surtidor1_WriteControlRegister(Surtidor1_ReadControlRegister() |
                                                      Surtidor1_CTRL_MARK);
            #endif /* End Surtidor1_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
            #if( Surtidor1_CONTROL_REG_REMOVED == 0u )
                Surtidor1_WriteControlRegister(Surtidor1_ReadControlRegister() &
                                                    (uint8)~Surtidor1_CTRL_MARK);
            #endif /* End Surtidor1_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndSurtidor1_TX_ENABLED */

#if(Surtidor1_HD_ENABLED)


    /*******************************************************************************
    * Function Name: Surtidor1_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Unloads the Rx configuration if required and loads the
    *  Tx configuration. It is the users responsibility to ensure that any
    *  transaction is complete and it is safe to unload the Tx
    *  configuration.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Valid only for half duplex UART.
    *
    * Side Effects:
    *  Disable RX interrupt mask, when software buffer has been used.
    *
    *******************************************************************************/
    void Surtidor1_LoadTxConfig(void) 
    {
        #if((Surtidor1_RX_INTERRUPT_ENABLED) && (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
            /* Disable RX interrupts before set TX configuration */
            Surtidor1_SetRxInterruptMode(0u);
        #endif /* Surtidor1_RX_INTERRUPT_ENABLED */

        Surtidor1_WriteControlRegister(Surtidor1_ReadControlRegister() | Surtidor1_CTRL_HD_SEND);
        Surtidor1_RXBITCTR_PERIOD_REG = Surtidor1_HD_TXBITCTR_INIT;
        #if(CY_UDB_V0) /* Manually clear status register when mode has been changed */
            /* Clear status register */
            CY_GET_REG8(Surtidor1_RXSTATUS_PTR);
        #endif /* CY_UDB_V0 */
    }


    /*******************************************************************************
    * Function Name: Surtidor1_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Unloads the Tx configuration if required and loads the
    *  Rx configuration. It is the users responsibility to ensure that any
    *  transaction is complete and it is safe to unload the Rx
    *  configuration.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Valid only for half duplex UART
    *
    * Side Effects:
    *  Set RX interrupt mask based on customizer settings, when software buffer
    *  has been used.
    *
    *******************************************************************************/
    void Surtidor1_LoadRxConfig(void) 
    {
        Surtidor1_WriteControlRegister(Surtidor1_ReadControlRegister() &
                                                (uint8)~Surtidor1_CTRL_HD_SEND);
        Surtidor1_RXBITCTR_PERIOD_REG = Surtidor1_HD_RXBITCTR_INIT;
        #if(CY_UDB_V0) /* Manually clear status register when mode has been changed */
            /* Clear status register */
            CY_GET_REG8(Surtidor1_RXSTATUS_PTR);
        #endif /* CY_UDB_V0 */

        #if((Surtidor1_RX_INTERRUPT_ENABLED) && (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
            /* Enable RX interrupt after set RX configuration */
            Surtidor1_SetRxInterruptMode(Surtidor1_INIT_RX_INTERRUPTS_MASK);
        #endif /* Surtidor1_RX_INTERRUPT_ENABLED */
    }

#endif  /* Surtidor1_HD_ENABLED */


/* [] END OF FILE */
