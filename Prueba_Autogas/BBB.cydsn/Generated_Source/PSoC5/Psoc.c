/*******************************************************************************
* File Name: Psoc.c
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

#include "Psoc.h"
#include "CyLib.h"
#if(Psoc_INTERNAL_CLOCK_USED)
    #include "Psoc_IntClock.h"
#endif /* End Psoc_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 Psoc_initVar = 0u;
#if( Psoc_TX_ENABLED && (Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH))
    volatile uint8 Psoc_txBuffer[Psoc_TXBUFFERSIZE];
    volatile uint8 Psoc_txBufferRead = 0u;
    uint8 Psoc_txBufferWrite = 0u;
#endif /* End Psoc_TX_ENABLED */
#if( ( Psoc_RX_ENABLED || Psoc_HD_ENABLED ) && \
     (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH) )
    volatile uint8 Psoc_rxBuffer[Psoc_RXBUFFERSIZE];
    volatile uint8 Psoc_rxBufferRead = 0u;
    volatile uint8 Psoc_rxBufferWrite = 0u;
    volatile uint8 Psoc_rxBufferLoopDetect = 0u;
    volatile uint8 Psoc_rxBufferOverflow = 0u;
    #if (Psoc_RXHW_ADDRESS_ENABLED)
        volatile uint8 Psoc_rxAddressMode = Psoc_RXADDRESSMODE;
        volatile uint8 Psoc_rxAddressDetected = 0u;
    #endif /* End EnableHWAddress */
#endif /* End Psoc_RX_ENABLED */


/*******************************************************************************
* Function Name: Psoc_Start
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
*  The Psoc_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time UART_Start() is called. This allows for
*  component initialization without re-initialization in all subsequent calls
*  to the Psoc_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Psoc_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(Psoc_initVar == 0u)
    {
        Psoc_Init();
        Psoc_initVar = 1u;
    }
    Psoc_Enable();
}


/*******************************************************************************
* Function Name: Psoc_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  Psoc_Start().
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Psoc_Init(void) 
{
    #if(Psoc_RX_ENABLED || Psoc_HD_ENABLED)

        #if(Psoc_RX_INTERRUPT_ENABLED && (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH))
            /* Set the RX Interrupt. */
            (void)CyIntSetVector(Psoc_RX_VECT_NUM, &Psoc_RXISR);
            CyIntSetPriority(Psoc_RX_VECT_NUM, Psoc_RX_PRIOR_NUM);
        #endif /* End Psoc_RX_INTERRUPT_ENABLED */

        #if (Psoc_RXHW_ADDRESS_ENABLED)
            Psoc_SetRxAddressMode(Psoc_RXAddressMode);
            Psoc_SetRxAddress1(Psoc_RXHWADDRESS1);
            Psoc_SetRxAddress2(Psoc_RXHWADDRESS2);
        #endif /* End Psoc_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        Psoc_RXBITCTR_PERIOD_REG = Psoc_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        Psoc_RXSTATUS_MASK_REG  = Psoc_INIT_RX_INTERRUPTS_MASK;
    #endif /* End Psoc_RX_ENABLED || Psoc_HD_ENABLED*/

    #if(Psoc_TX_ENABLED)
        #if(Psoc_TX_INTERRUPT_ENABLED && (Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH))
            /* Set the TX Interrupt. */
            (void)CyIntSetVector(Psoc_TX_VECT_NUM, &Psoc_TXISR);
            CyIntSetPriority(Psoc_TX_VECT_NUM, Psoc_TX_PRIOR_NUM);
        #endif /* End Psoc_TX_INTERRUPT_ENABLED */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if(Psoc_TXCLKGEN_DP)
            Psoc_TXBITCLKGEN_CTR_REG = Psoc_BIT_CENTER;
            Psoc_TXBITCLKTX_COMPLETE_REG = (Psoc_NUMBER_OF_DATA_BITS +
                        Psoc_NUMBER_OF_START_BIT) * Psoc_OVER_SAMPLE_COUNT;
        #else
            Psoc_TXBITCTR_PERIOD_REG = ((Psoc_NUMBER_OF_DATA_BITS +
                        Psoc_NUMBER_OF_START_BIT) * Psoc_OVER_SAMPLE_8) - 1u;
        #endif /* End Psoc_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if(Psoc_TX_INTERRUPT_ENABLED && (Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH))
            Psoc_TXSTATUS_MASK_REG = Psoc_TX_STS_FIFO_EMPTY;
        #else
            Psoc_TXSTATUS_MASK_REG = Psoc_INIT_TX_INTERRUPTS_MASK;
        #endif /*End Psoc_TX_INTERRUPT_ENABLED*/

    #endif /* End Psoc_TX_ENABLED */

    #if(Psoc_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        Psoc_WriteControlRegister( \
            (Psoc_ReadControlRegister() & (uint8)~Psoc_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(Psoc_PARITY_TYPE << Psoc_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End Psoc_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: Psoc_Enable
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
*  Psoc_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void Psoc_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if(Psoc_RX_ENABLED || Psoc_HD_ENABLED)
        /*RX Counter (Count7) Enable */
        Psoc_RXBITCTR_CONTROL_REG |= Psoc_CNTR_ENABLE;
        /* Enable the RX Interrupt. */
        Psoc_RXSTATUS_ACTL_REG  |= Psoc_INT_ENABLE;
        #if(Psoc_RX_INTERRUPT_ENABLED && (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH))
            CyIntEnable(Psoc_RX_VECT_NUM);
            #if (Psoc_RXHW_ADDRESS_ENABLED)
                Psoc_rxAddressDetected = 0u;
            #endif /* End Psoc_RXHW_ADDRESS_ENABLED */
        #endif /* End Psoc_RX_INTERRUPT_ENABLED */
    #endif /* End Psoc_RX_ENABLED || Psoc_HD_ENABLED*/

    #if(Psoc_TX_ENABLED)
        /*TX Counter (DP/Count7) Enable */
        #if(!Psoc_TXCLKGEN_DP)
            Psoc_TXBITCTR_CONTROL_REG |= Psoc_CNTR_ENABLE;
        #endif /* End Psoc_TXCLKGEN_DP */
        /* Enable the TX Interrupt. */
        Psoc_TXSTATUS_ACTL_REG |= Psoc_INT_ENABLE;
        #if(Psoc_TX_INTERRUPT_ENABLED && (Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH))
            CyIntEnable(Psoc_TX_VECT_NUM);
        #endif /* End Psoc_TX_INTERRUPT_ENABLED*/
     #endif /* End Psoc_TX_ENABLED */

    #if(Psoc_INTERNAL_CLOCK_USED)
        /* Enable the clock. */
        Psoc_IntClock_Start();
    #endif /* End Psoc_INTERNAL_CLOCK_USED */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Psoc_Stop
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
void Psoc_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if(Psoc_RX_ENABLED || Psoc_HD_ENABLED)
        Psoc_RXBITCTR_CONTROL_REG &= (uint8)~Psoc_CNTR_ENABLE;
    #endif /* End Psoc_RX_ENABLED */

    #if(Psoc_TX_ENABLED)
        #if(!Psoc_TXCLKGEN_DP)
            Psoc_TXBITCTR_CONTROL_REG &= (uint8)~Psoc_CNTR_ENABLE;
        #endif /* End Psoc_TXCLKGEN_DP */
    #endif /* Psoc_TX_ENABLED */

    #if(Psoc_INTERNAL_CLOCK_USED)
        /* Disable the clock. */
        Psoc_IntClock_Stop();
    #endif /* End Psoc_INTERNAL_CLOCK_USED */

    /* Disable internal interrupt component */
    #if(Psoc_RX_ENABLED || Psoc_HD_ENABLED)
        Psoc_RXSTATUS_ACTL_REG  &= (uint8)~Psoc_INT_ENABLE;
        #if(Psoc_RX_INTERRUPT_ENABLED && (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH))
            Psoc_DisableRxInt();
        #endif /* End Psoc_RX_INTERRUPT_ENABLED */
    #endif /* End Psoc_RX_ENABLED */

    #if(Psoc_TX_ENABLED)
        Psoc_TXSTATUS_ACTL_REG &= (uint8)~Psoc_INT_ENABLE;
        #if(Psoc_TX_INTERRUPT_ENABLED && (Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH))
            Psoc_DisableTxInt();
        #endif /* End Psoc_TX_INTERRUPT_ENABLED */
    #endif /* End Psoc_TX_ENABLED */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Psoc_ReadControlRegister
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
uint8 Psoc_ReadControlRegister(void) 
{
    #if( Psoc_CONTROL_REG_REMOVED )
        return(0u);
    #else
        return(Psoc_CONTROL_REG);
    #endif /* End Psoc_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Psoc_WriteControlRegister
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
void  Psoc_WriteControlRegister(uint8 control) 
{
    #if( Psoc_CONTROL_REG_REMOVED )
        if(control != 0u) { }      /* release compiler warning */
    #else
       Psoc_CONTROL_REG = control;
    #endif /* End Psoc_CONTROL_REG_REMOVED */
}


#if(Psoc_RX_ENABLED || Psoc_HD_ENABLED)

    #if(Psoc_RX_INTERRUPT_ENABLED)

        /*******************************************************************************
        * Function Name: Psoc_EnableRxInt
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
        void Psoc_EnableRxInt(void) 
        {
            CyIntEnable(Psoc_RX_VECT_NUM);
        }


        /*******************************************************************************
        * Function Name: Psoc_DisableRxInt
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
        void Psoc_DisableRxInt(void) 
        {
            CyIntDisable(Psoc_RX_VECT_NUM);
        }

    #endif /* Psoc_RX_INTERRUPT_ENABLED */


    /*******************************************************************************
    * Function Name: Psoc_SetRxInterruptMode
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
    void Psoc_SetRxInterruptMode(uint8 intSrc) 
    {
        Psoc_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: Psoc_ReadRxData
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
    *  Psoc_rxBuffer - RAM buffer pointer for save received data.
    *  Psoc_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Psoc_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Psoc_rxBufferLoopDetect - creared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Psoc_ReadRxData(void) 
    {
        uint8 rxData;

        #if(Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH)
            uint8 loc_rxBufferRead;
            uint8 loc_rxBufferWrite;
            /* Protect variables that could change on interrupt. */
            /* Disable Rx interrupt. */
            #if(Psoc_RX_INTERRUPT_ENABLED)
                Psoc_DisableRxInt();
            #endif /* Psoc_RX_INTERRUPT_ENABLED */
            loc_rxBufferRead = Psoc_rxBufferRead;
            loc_rxBufferWrite = Psoc_rxBufferWrite;

            if( (Psoc_rxBufferLoopDetect != 0u) || (loc_rxBufferRead != loc_rxBufferWrite) )
            {
                rxData = Psoc_rxBuffer[loc_rxBufferRead];
                loc_rxBufferRead++;

                if(loc_rxBufferRead >= Psoc_RXBUFFERSIZE)
                {
                    loc_rxBufferRead = 0u;
                }
                /* Update the real pointer */
                Psoc_rxBufferRead = loc_rxBufferRead;

                if(Psoc_rxBufferLoopDetect != 0u )
                {
                    Psoc_rxBufferLoopDetect = 0u;
                    #if( (Psoc_RX_INTERRUPT_ENABLED) && (Psoc_FLOW_CONTROL != 0u) && \
                         (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH) )
                        /* When Hardware Flow Control selected - return RX mask */
                        #if( Psoc_HD_ENABLED )
                            if((Psoc_CONTROL_REG & Psoc_CTRL_HD_SEND) == 0u)
                            {   /* In Half duplex mode return RX mask only in RX
                                *  configuration set, otherwise
                                *  mask will be returned in LoadRxConfig() API.
                                */
                                Psoc_RXSTATUS_MASK_REG  |= Psoc_RX_STS_FIFO_NOTEMPTY;
                            }
                        #else
                            Psoc_RXSTATUS_MASK_REG  |= Psoc_RX_STS_FIFO_NOTEMPTY;
                        #endif /* end Psoc_HD_ENABLED */
                    #endif /* Psoc_RX_INTERRUPT_ENABLED and Hardware flow control*/
                }
            }
            else
            {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit*/
                rxData = Psoc_RXDATA_REG;
            }

            /* Enable Rx interrupt. */
            #if(Psoc_RX_INTERRUPT_ENABLED)
                Psoc_EnableRxInt();
            #endif /* End Psoc_RX_INTERRUPT_ENABLED */

        #else /* Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH */

            /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit*/
            rxData = Psoc_RXDATA_REG;

        #endif /* Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Psoc_ReadRxStatus
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
    *  Psoc_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn?t free space in
    *   Psoc_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   Psoc_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 Psoc_ReadRxStatus(void) 
    {
        uint8 status;

        status = Psoc_RXSTATUS_REG & Psoc_RX_HW_MASK;

        #if(Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH)
            if( Psoc_rxBufferOverflow != 0u )
            {
                status |= Psoc_RX_STS_SOFT_BUFF_OVER;
                Psoc_rxBufferOverflow = 0u;
            }
        #endif /* Psoc_RXBUFFERSIZE */

        return(status);
    }


    /*******************************************************************************
    * Function Name: Psoc_GetChar
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
    *  Psoc_rxBuffer - RAM buffer pointer for save received data.
    *  Psoc_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Psoc_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Psoc_rxBufferLoopDetect - creared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Psoc_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

        #if(Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH)
            uint8 loc_rxBufferRead;
            uint8 loc_rxBufferWrite;
            /* Protect variables that could change on interrupt. */
            /* Disable Rx interrupt. */
            #if(Psoc_RX_INTERRUPT_ENABLED)
                Psoc_DisableRxInt();
            #endif /* Psoc_RX_INTERRUPT_ENABLED */
            loc_rxBufferRead = Psoc_rxBufferRead;
            loc_rxBufferWrite = Psoc_rxBufferWrite;

            if( (Psoc_rxBufferLoopDetect != 0u) || (loc_rxBufferRead != loc_rxBufferWrite) )
            {
                rxData = Psoc_rxBuffer[loc_rxBufferRead];
                loc_rxBufferRead++;
                if(loc_rxBufferRead >= Psoc_RXBUFFERSIZE)
                {
                    loc_rxBufferRead = 0u;
                }
                /* Update the real pointer */
                Psoc_rxBufferRead = loc_rxBufferRead;

                if(Psoc_rxBufferLoopDetect > 0u )
                {
                    Psoc_rxBufferLoopDetect = 0u;
                    #if( (Psoc_RX_INTERRUPT_ENABLED) && (Psoc_FLOW_CONTROL != 0u) )
                        /* When Hardware Flow Control selected - return RX mask */
                        #if( Psoc_HD_ENABLED )
                            if((Psoc_CONTROL_REG & Psoc_CTRL_HD_SEND) == 0u)
                            {   /* In Half duplex mode return RX mask only if
                                *  RX configuration set, otherwise
                                *  mask will be returned in LoadRxConfig() API.
                                */
                                Psoc_RXSTATUS_MASK_REG  |= Psoc_RX_STS_FIFO_NOTEMPTY;
                            }
                        #else
                            Psoc_RXSTATUS_MASK_REG  |= Psoc_RX_STS_FIFO_NOTEMPTY;
                        #endif /* end Psoc_HD_ENABLED */
                    #endif /* Psoc_RX_INTERRUPT_ENABLED and Hardware flow control*/
                }

            }
            else
            {   rxStatus = Psoc_RXSTATUS_REG;
                if((rxStatus & Psoc_RX_STS_FIFO_NOTEMPTY) != 0u)
                {   /* Read received data from FIFO*/
                    rxData = Psoc_RXDATA_REG;
                    /*Check status on error*/
                    if((rxStatus & (Psoc_RX_STS_BREAK | Psoc_RX_STS_PAR_ERROR |
                                   Psoc_RX_STS_STOP_ERROR | Psoc_RX_STS_OVERRUN)) != 0u)
                    {
                        rxData = 0u;
                    }
                }
            }

            /* Enable Rx interrupt. */
            #if(Psoc_RX_INTERRUPT_ENABLED)
                Psoc_EnableRxInt();
            #endif /* Psoc_RX_INTERRUPT_ENABLED */

        #else /* Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH */

            rxStatus =Psoc_RXSTATUS_REG;
            if((rxStatus & Psoc_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO*/
                rxData = Psoc_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (Psoc_RX_STS_BREAK | Psoc_RX_STS_PAR_ERROR |
                               Psoc_RX_STS_STOP_ERROR | Psoc_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        #endif /* Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Psoc_GetByte
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
    uint16 Psoc_GetByte(void) 
    {
        return ( ((uint16)Psoc_ReadRxStatus() << 8u) | Psoc_ReadRxData() );
    }


    /*******************************************************************************
    * Function Name: Psoc_GetRxBufferSize
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
    *  Psoc_rxBufferWrite - used to calculate left bytes.
    *  Psoc_rxBufferRead - used to calculate left bytes.
    *  Psoc_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 Psoc_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

        #if(Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH)

            /* Disable Rx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(Psoc_RX_INTERRUPT_ENABLED)
                Psoc_DisableRxInt();
            #endif /* Psoc_RX_INTERRUPT_ENABLED */

            if(Psoc_rxBufferRead == Psoc_rxBufferWrite)
            {
                if(Psoc_rxBufferLoopDetect > 0u)
                {
                    size = Psoc_RXBUFFERSIZE;
                }
                else
                {
                    size = 0u;
                }
            }
            else if(Psoc_rxBufferRead < Psoc_rxBufferWrite)
            {
                size = (Psoc_rxBufferWrite - Psoc_rxBufferRead);
            }
            else
            {
                size = (Psoc_RXBUFFERSIZE - Psoc_rxBufferRead) + Psoc_rxBufferWrite;
            }

            /* Enable Rx interrupt. */
            #if(Psoc_RX_INTERRUPT_ENABLED)
                Psoc_EnableRxInt();
            #endif /* End Psoc_RX_INTERRUPT_ENABLED */

        #else /* Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH */

            /* We can only know if there is data in the fifo. */
            size = ((Psoc_RXSTATUS_REG & Psoc_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

        #endif /* End Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Psoc_ClearRxBuffer
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
    *  Psoc_rxBufferWrite - cleared to zero.
    *  Psoc_rxBufferRead - cleared to zero.
    *  Psoc_rxBufferLoopDetect - cleared to zero.
    *  Psoc_rxBufferOverflow - cleared to zero.
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
    void Psoc_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* clear the HW FIFO */
        /* Enter critical section */
        enableInterrupts = CyEnterCriticalSection();
        Psoc_RXDATA_AUX_CTL_REG |=  Psoc_RX_FIFO_CLR;
        Psoc_RXDATA_AUX_CTL_REG &= (uint8)~Psoc_RX_FIFO_CLR;
        /* Exit critical section */
        CyExitCriticalSection(enableInterrupts);

        #if(Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH)
            /* Disable Rx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(Psoc_RX_INTERRUPT_ENABLED)
                Psoc_DisableRxInt();
            #endif /* End Psoc_RX_INTERRUPT_ENABLED */

            Psoc_rxBufferRead = 0u;
            Psoc_rxBufferWrite = 0u;
            Psoc_rxBufferLoopDetect = 0u;
            Psoc_rxBufferOverflow = 0u;

            /* Enable Rx interrupt. */
            #if(Psoc_RX_INTERRUPT_ENABLED)
                Psoc_EnableRxInt();
            #endif /* End Psoc_RX_INTERRUPT_ENABLED */
        #endif /* End Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH */

    }


    /*******************************************************************************
    * Function Name: Psoc_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Set the receive addressing mode
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  Psoc__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  Psoc__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  Psoc__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  Psoc__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  Psoc__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Psoc_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  Psoc_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void Psoc_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(Psoc_RXHW_ADDRESS_ENABLED)
            #if(Psoc_CONTROL_REG_REMOVED)
                if(addressMode != 0u) { }     /* release compiler warning */
            #else /* Psoc_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = Psoc_CONTROL_REG & (uint8)~Psoc_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << Psoc_CTRL_RXADDR_MODE0_SHIFT);
                Psoc_CONTROL_REG = tmpCtrl;
                #if(Psoc_RX_INTERRUPT_ENABLED && \
                   (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH) )
                    Psoc_rxAddressMode = addressMode;
                    Psoc_rxAddressDetected = 0u;
                #endif /* End Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH*/
            #endif /* End Psoc_CONTROL_REG_REMOVED */
        #else /* Psoc_RXHW_ADDRESS_ENABLED */
            if(addressMode != 0u) { }     /* release compiler warning */
        #endif /* End Psoc_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Psoc_SetRxAddress1
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
    void Psoc_SetRxAddress1(uint8 address) 

    {
        Psoc_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: Psoc_SetRxAddress2
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
    void Psoc_SetRxAddress2(uint8 address) 
    {
        Psoc_RXADDRESS2_REG = address;
    }

#endif  /* Psoc_RX_ENABLED || Psoc_HD_ENABLED*/


#if( (Psoc_TX_ENABLED) || (Psoc_HD_ENABLED) )

    #if(Psoc_TX_INTERRUPT_ENABLED)

        /*******************************************************************************
        * Function Name: Psoc_EnableTxInt
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
        void Psoc_EnableTxInt(void) 
        {
            CyIntEnable(Psoc_TX_VECT_NUM);
        }


        /*******************************************************************************
        * Function Name: Psoc_DisableTxInt
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
        void Psoc_DisableTxInt(void) 
        {
            CyIntDisable(Psoc_TX_VECT_NUM);
        }

    #endif /* Psoc_TX_INTERRUPT_ENABLED */


    /*******************************************************************************
    * Function Name: Psoc_SetTxInterruptMode
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
    void Psoc_SetTxInterruptMode(uint8 intSrc) 
    {
        Psoc_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: Psoc_WriteTxData
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
    *  Psoc_txBuffer - RAM buffer pointer for save data for transmission
    *  Psoc_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  Psoc_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  Psoc_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Psoc_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Psoc_initVar != 0u)
        {
            #if(Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH)

                /* Disable Tx interrupt. */
                /* Protect variables that could change on interrupt. */
                #if(Psoc_TX_INTERRUPT_ENABLED)
                    Psoc_DisableTxInt();
                #endif /* End Psoc_TX_INTERRUPT_ENABLED */

                if( (Psoc_txBufferRead == Psoc_txBufferWrite) &&
                    ((Psoc_TXSTATUS_REG & Psoc_TX_STS_FIFO_FULL) == 0u) )
                {
                    /* Add directly to the FIFO. */
                    Psoc_TXDATA_REG = txDataByte;
                }
                else
                {
                    if(Psoc_txBufferWrite >= Psoc_TXBUFFERSIZE)
                    {
                        Psoc_txBufferWrite = 0u;
                    }

                    Psoc_txBuffer[Psoc_txBufferWrite] = txDataByte;

                    /* Add to the software buffer. */
                    Psoc_txBufferWrite++;

                }

                /* Enable Tx interrupt. */
                #if(Psoc_TX_INTERRUPT_ENABLED)
                    Psoc_EnableTxInt();
                #endif /* End Psoc_TX_INTERRUPT_ENABLED */

            #else /* Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH */

                /* Add directly to the FIFO. */
                Psoc_TXDATA_REG = txDataByte;

            #endif /* End Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH */
        }
    }


    /*******************************************************************************
    * Function Name: Psoc_ReadTxStatus
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
    uint8 Psoc_ReadTxStatus(void) 
    {
        return(Psoc_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: Psoc_PutChar
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
    *  Psoc_txBuffer - RAM buffer pointer for save data for transmission
    *  Psoc_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  Psoc_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  Psoc_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void Psoc_PutChar(uint8 txDataByte) 
    {
            #if(Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH)
                /* The temporary output pointer is used since it takes two instructions
                *  to increment with a wrap, and we can't risk doing that with the real
                *  pointer and getting an interrupt in between instructions.
                */
                uint8 loc_txBufferWrite;
                uint8 loc_txBufferRead;

                do{
                    /* Block if software buffer is full, so we don't overwrite. */
                    #if ((Psoc_TXBUFFERSIZE > Psoc_MAX_BYTE_VALUE) && (CY_PSOC3))
                        /* Disable TX interrupt to protect variables that could change on interrupt */
                        CyIntDisable(Psoc_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                    loc_txBufferWrite = Psoc_txBufferWrite;
                    loc_txBufferRead = Psoc_txBufferRead;
                    #if ((Psoc_TXBUFFERSIZE > Psoc_MAX_BYTE_VALUE) && (CY_PSOC3))
                        /* Enable interrupt to continue transmission */
                        CyIntEnable(Psoc_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                }while( (loc_txBufferWrite < loc_txBufferRead) ? (loc_txBufferWrite == (loc_txBufferRead - 1u)) :
                                        ((loc_txBufferWrite - loc_txBufferRead) ==
                                        (uint8)(Psoc_TXBUFFERSIZE - 1u)) );

                if( (loc_txBufferRead == loc_txBufferWrite) &&
                    ((Psoc_TXSTATUS_REG & Psoc_TX_STS_FIFO_FULL) == 0u) )
                {
                    /* Add directly to the FIFO. */
                    Psoc_TXDATA_REG = txDataByte;
                }
                else
                {
                    if(loc_txBufferWrite >= Psoc_TXBUFFERSIZE)
                    {
                        loc_txBufferWrite = 0u;
                    }
                    /* Add to the software buffer. */
                    Psoc_txBuffer[loc_txBufferWrite] = txDataByte;
                    loc_txBufferWrite++;

                    /* Finally, update the real output pointer */
                    #if ((Psoc_TXBUFFERSIZE > Psoc_MAX_BYTE_VALUE) && (CY_PSOC3))
                        CyIntDisable(Psoc_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                    Psoc_txBufferWrite = loc_txBufferWrite;
                    #if ((Psoc_TXBUFFERSIZE > Psoc_MAX_BYTE_VALUE) && (CY_PSOC3))
                        CyIntEnable(Psoc_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                }

            #else /* Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH */

                while((Psoc_TXSTATUS_REG & Psoc_TX_STS_FIFO_FULL) != 0u)
                {
                    ; /* Wait for room in the FIFO. */
                }

                /* Add directly to the FIFO. */
                Psoc_TXDATA_REG = txDataByte;

            #endif /* End Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH */
    }


    /*******************************************************************************
    * Function Name: Psoc_PutString
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
    *  Psoc_initVar - checked to identify that the component has been
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
    void Psoc_PutString(const char8 string[]) 
    {
        uint16 buf_index = 0u;
        /* If not Initialized then skip this function*/
        if(Psoc_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent*/
            while(string[buf_index] != (char8)0)
            {
                Psoc_PutChar((uint8)string[buf_index]);
                buf_index++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Psoc_PutArray
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
    *  Psoc_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Psoc_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 buf_index = 0u;
        /* If not Initialized then skip this function*/
        if(Psoc_initVar != 0u)
        {
            do
            {
                Psoc_PutChar(string[buf_index]);
                buf_index++;
            }while(buf_index < byteCount);
        }
    }


    /*******************************************************************************
    * Function Name: Psoc_PutCRLF
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
    *  Psoc_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Psoc_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Psoc_initVar != 0u)
        {
            Psoc_PutChar(txDataByte);
            Psoc_PutChar(0x0Du);
            Psoc_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: Psoc_GetTxBufferSize
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
    *  Psoc_txBufferWrite - used to calculate left space.
    *  Psoc_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 Psoc_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

        #if(Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH)

            /* Disable Tx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(Psoc_TX_INTERRUPT_ENABLED)
                Psoc_DisableTxInt();
            #endif /* End Psoc_TX_INTERRUPT_ENABLED */

            if(Psoc_txBufferRead == Psoc_txBufferWrite)
            {
                size = 0u;
            }
            else if(Psoc_txBufferRead < Psoc_txBufferWrite)
            {
                size = (Psoc_txBufferWrite - Psoc_txBufferRead);
            }
            else
            {
                size = (Psoc_TXBUFFERSIZE - Psoc_txBufferRead) + Psoc_txBufferWrite;
            }

            /* Enable Tx interrupt. */
            #if(Psoc_TX_INTERRUPT_ENABLED)
                Psoc_EnableTxInt();
            #endif /* End Psoc_TX_INTERRUPT_ENABLED */

        #else /* Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH */

            size = Psoc_TXSTATUS_REG;

            /* Is the fifo is full. */
            if((size & Psoc_TX_STS_FIFO_FULL) != 0u)
            {
                size = Psoc_FIFO_LENGTH;
            }
            else if((size & Psoc_TX_STS_FIFO_EMPTY) != 0u)
            {
                size = 0u;
            }
            else
            {
                /* We only know there is data in the fifo. */
                size = 1u;
            }

        #endif /* End Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Psoc_ClearTxBuffer
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
    *  Psoc_txBufferWrite - cleared to zero.
    *  Psoc_txBufferRead - cleared to zero.
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
    void Psoc_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Enter critical section */
        enableInterrupts = CyEnterCriticalSection();
        /* clear the HW FIFO */
        Psoc_TXDATA_AUX_CTL_REG |=  Psoc_TX_FIFO_CLR;
        Psoc_TXDATA_AUX_CTL_REG &= (uint8)~Psoc_TX_FIFO_CLR;
        /* Exit critical section */
        CyExitCriticalSection(enableInterrupts);

        #if(Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH)

            /* Disable Tx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(Psoc_TX_INTERRUPT_ENABLED)
                Psoc_DisableTxInt();
            #endif /* End Psoc_TX_INTERRUPT_ENABLED */

            Psoc_txBufferRead = 0u;
            Psoc_txBufferWrite = 0u;

            /* Enable Tx interrupt. */
            #if(Psoc_TX_INTERRUPT_ENABLED)
                Psoc_EnableTxInt();
            #endif /* End Psoc_TX_INTERRUPT_ENABLED */

        #endif /* End Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH */
    }


    /*******************************************************************************
    * Function Name: Psoc_SendBreak
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
    *  Psoc_initVar - checked to identify that the component has been
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
    void Psoc_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(Psoc_initVar != 0u)
        {
            /*Set the Counter to 13-bits and transmit a 00 byte*/
            /*When that is done then reset the counter value back*/
            uint8 tmpStat;

            #if(Psoc_HD_ENABLED) /* Half Duplex mode*/

                if( (retMode == Psoc_SEND_BREAK) ||
                    (retMode == Psoc_SEND_WAIT_REINIT ) )
                {
                    /* CTRL_HD_SEND_BREAK - sends break bits in HD mode*/
                    Psoc_WriteControlRegister(Psoc_ReadControlRegister() |
                                                          Psoc_CTRL_HD_SEND_BREAK);
                    /* Send zeros*/
                    Psoc_TXDATA_REG = 0u;

                    do /*wait until transmit starts*/
                    {
                        tmpStat = Psoc_TXSTATUS_REG;
                    }while((tmpStat & Psoc_TX_STS_FIFO_EMPTY) != 0u);
                }

                if( (retMode == Psoc_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == Psoc_SEND_WAIT_REINIT) )
                {
                    do /*wait until transmit complete*/
                    {
                        tmpStat = Psoc_TXSTATUS_REG;
                    }while(((uint8)~tmpStat & Psoc_TX_STS_COMPLETE) != 0u);
                }

                if( (retMode == Psoc_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == Psoc_REINIT) ||
                    (retMode == Psoc_SEND_WAIT_REINIT) )
                {
                    Psoc_WriteControlRegister(Psoc_ReadControlRegister() &
                                                  (uint8)~Psoc_CTRL_HD_SEND_BREAK);
                }

            #else /* Psoc_HD_ENABLED Full Duplex mode */

                static uint8 tx_period;

                if( (retMode == Psoc_SEND_BREAK) ||
                    (retMode == Psoc_SEND_WAIT_REINIT) )
                {
                    /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode*/
                    #if( (Psoc_PARITY_TYPE != Psoc__B_UART__NONE_REVB) || \
                                        (Psoc_PARITY_TYPE_SW != 0u) )
                        Psoc_WriteControlRegister(Psoc_ReadControlRegister() |
                                                              Psoc_CTRL_HD_SEND_BREAK);
                    #endif /* End Psoc_PARITY_TYPE != Psoc__B_UART__NONE_REVB  */

                    #if(Psoc_TXCLKGEN_DP)
                        tx_period = Psoc_TXBITCLKTX_COMPLETE_REG;
                        Psoc_TXBITCLKTX_COMPLETE_REG = Psoc_TXBITCTR_BREAKBITS;
                    #else
                        tx_period = Psoc_TXBITCTR_PERIOD_REG;
                        Psoc_TXBITCTR_PERIOD_REG = Psoc_TXBITCTR_BREAKBITS8X;
                    #endif /* End Psoc_TXCLKGEN_DP */

                    /* Send zeros*/
                    Psoc_TXDATA_REG = 0u;

                    do /* wait until transmit starts */
                    {
                        tmpStat = Psoc_TXSTATUS_REG;
                    }while((tmpStat & Psoc_TX_STS_FIFO_EMPTY) != 0u);
                }

                if( (retMode == Psoc_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == Psoc_SEND_WAIT_REINIT) )
                {
                    do /*wait until transmit complete*/
                    {
                        tmpStat = Psoc_TXSTATUS_REG;
                    }while(((uint8)~tmpStat & Psoc_TX_STS_COMPLETE) != 0u);
                }

                if( (retMode == Psoc_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == Psoc_REINIT) ||
                    (retMode == Psoc_SEND_WAIT_REINIT) )
                {

                    #if(Psoc_TXCLKGEN_DP)
                        Psoc_TXBITCLKTX_COMPLETE_REG = tx_period;
                    #else
                        Psoc_TXBITCTR_PERIOD_REG = tx_period;
                    #endif /* End Psoc_TXCLKGEN_DP */

                    #if( (Psoc_PARITY_TYPE != Psoc__B_UART__NONE_REVB) || \
                         (Psoc_PARITY_TYPE_SW != 0u) )
                        Psoc_WriteControlRegister(Psoc_ReadControlRegister() &
                                                      (uint8)~Psoc_CTRL_HD_SEND_BREAK);
                    #endif /* End Psoc_PARITY_TYPE != NONE */
                }
            #endif    /* End Psoc_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: Psoc_SetTxAddressMode
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
    void Psoc_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable*/
        if(addressMode != 0u)
        {
            #if( Psoc_CONTROL_REG_REMOVED == 0u )
                Psoc_WriteControlRegister(Psoc_ReadControlRegister() |
                                                      Psoc_CTRL_MARK);
            #endif /* End Psoc_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
            #if( Psoc_CONTROL_REG_REMOVED == 0u )
                Psoc_WriteControlRegister(Psoc_ReadControlRegister() &
                                                    (uint8)~Psoc_CTRL_MARK);
            #endif /* End Psoc_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndPsoc_TX_ENABLED */

#if(Psoc_HD_ENABLED)


    /*******************************************************************************
    * Function Name: Psoc_LoadTxConfig
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
    void Psoc_LoadTxConfig(void) 
    {
        #if((Psoc_RX_INTERRUPT_ENABLED) && (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH))
            /* Disable RX interrupts before set TX configuration */
            Psoc_SetRxInterruptMode(0u);
        #endif /* Psoc_RX_INTERRUPT_ENABLED */

        Psoc_WriteControlRegister(Psoc_ReadControlRegister() | Psoc_CTRL_HD_SEND);
        Psoc_RXBITCTR_PERIOD_REG = Psoc_HD_TXBITCTR_INIT;
        #if(CY_UDB_V0) /* Manually clear status register when mode has been changed */
            /* Clear status register */
            CY_GET_REG8(Psoc_RXSTATUS_PTR);
        #endif /* CY_UDB_V0 */
    }


    /*******************************************************************************
    * Function Name: Psoc_LoadRxConfig
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
    void Psoc_LoadRxConfig(void) 
    {
        Psoc_WriteControlRegister(Psoc_ReadControlRegister() &
                                                (uint8)~Psoc_CTRL_HD_SEND);
        Psoc_RXBITCTR_PERIOD_REG = Psoc_HD_RXBITCTR_INIT;
        #if(CY_UDB_V0) /* Manually clear status register when mode has been changed */
            /* Clear status register */
            CY_GET_REG8(Psoc_RXSTATUS_PTR);
        #endif /* CY_UDB_V0 */

        #if((Psoc_RX_INTERRUPT_ENABLED) && (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH))
            /* Enable RX interrupt after set RX configuration */
            Psoc_SetRxInterruptMode(Psoc_INIT_RX_INTERRUPTS_MASK);
        #endif /* Psoc_RX_INTERRUPT_ENABLED */
    }

#endif  /* Psoc_HD_ENABLED */


/* [] END OF FILE */
