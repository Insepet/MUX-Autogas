/*******************************************************************************
* File Name: SurtidorINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Surtidor.h"



/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (Surtidor_RX_INTERRUPT_ENABLED && (Surtidor_RX_ENABLED || Surtidor_HD_ENABLED))
    /*******************************************************************************
    * Function Name: Surtidor_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor_rxBuffer - RAM buffer pointer for save received data.
    *  Surtidor_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Surtidor_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Surtidor_rxBufferOverflow - software overflow flag. Set to one
    *     when Surtidor_rxBufferWrite index overtakes
    *     Surtidor_rxBufferRead index.
    *  Surtidor_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Surtidor_rxBufferWrite is equal to
    *    Surtidor_rxBufferRead
    *  Surtidor_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Surtidor_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Surtidor_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Surtidor_RXISR_ENTRY_CALLBACK
        Surtidor_RXISR_EntryCallback();
    #endif /* Surtidor_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Surtidor_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = Surtidor_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in Surtidor_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (Surtidor_RX_STS_BREAK | 
                            Surtidor_RX_STS_PAR_ERROR |
                            Surtidor_RX_STS_STOP_ERROR | 
                            Surtidor_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                Surtidor_errorStatus |= readStatus & ( Surtidor_RX_STS_BREAK | 
                                                            Surtidor_RX_STS_PAR_ERROR | 
                                                            Surtidor_RX_STS_STOP_ERROR | 
                                                            Surtidor_RX_STS_OVERRUN);
                /* `#START Surtidor_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef Surtidor_RXISR_ERROR_CALLBACK
                Surtidor_RXISR_ERROR_Callback();
            #endif /* Surtidor_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & Surtidor_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = Surtidor_RXDATA_REG;
            #if (Surtidor_RXHW_ADDRESS_ENABLED)
                if(Surtidor_rxAddressMode == (uint8)Surtidor__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & Surtidor_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & Surtidor_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Surtidor_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Surtidor_rxAddressDetected = 0u;
                        }
                    }
                    if(Surtidor_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        Surtidor_rxBuffer[Surtidor_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    Surtidor_rxBuffer[Surtidor_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                Surtidor_rxBuffer[Surtidor_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (Surtidor_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(Surtidor_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        Surtidor_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    Surtidor_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(Surtidor_rxBufferWrite >= Surtidor_RX_BUFFER_SIZE)
                    {
                        Surtidor_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(Surtidor_rxBufferWrite == Surtidor_rxBufferRead)
                    {
                        Surtidor_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (Surtidor_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            Surtidor_RXSTATUS_MASK_REG  &= (uint8)~Surtidor_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(Surtidor_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (Surtidor_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & Surtidor_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START Surtidor_RXISR_END` */

        /* `#END` */

    #ifdef Surtidor_RXISR_EXIT_CALLBACK
        Surtidor_RXISR_ExitCallback();
    #endif /* Surtidor_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (Surtidor_RX_INTERRUPT_ENABLED && (Surtidor_RX_ENABLED || Surtidor_HD_ENABLED)) */


#if (Surtidor_TX_INTERRUPT_ENABLED && Surtidor_TX_ENABLED)
    /*******************************************************************************
    * Function Name: Surtidor_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Surtidor_txBuffer - RAM buffer pointer for transmit data from.
    *  Surtidor_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  Surtidor_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Surtidor_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Surtidor_TXISR_ENTRY_CALLBACK
        Surtidor_TXISR_EntryCallback();
    #endif /* Surtidor_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Surtidor_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((Surtidor_txBufferRead != Surtidor_txBufferWrite) &&
             ((Surtidor_TXSTATUS_REG & Surtidor_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(Surtidor_txBufferRead >= Surtidor_TX_BUFFER_SIZE)
            {
                Surtidor_txBufferRead = 0u;
            }

            Surtidor_TXDATA_REG = Surtidor_txBuffer[Surtidor_txBufferRead];

            /* Set next pointer */
            Surtidor_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Surtidor_TXISR_END` */

        /* `#END` */

    #ifdef Surtidor_TXISR_EXIT_CALLBACK
        Surtidor_TXISR_ExitCallback();
    #endif /* Surtidor_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (Surtidor_TX_INTERRUPT_ENABLED && Surtidor_TX_ENABLED) */


/* [] END OF FILE */
