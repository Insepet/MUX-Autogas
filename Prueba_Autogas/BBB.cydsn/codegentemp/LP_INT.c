/*******************************************************************************
* File Name: LPINT.c
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

#include "LP.h"



/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (LP_RX_INTERRUPT_ENABLED && (LP_RX_ENABLED || LP_HD_ENABLED))
    /*******************************************************************************
    * Function Name: LP_RXISR
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
    *  LP_rxBuffer - RAM buffer pointer for save received data.
    *  LP_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  LP_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  LP_rxBufferOverflow - software overflow flag. Set to one
    *     when LP_rxBufferWrite index overtakes
    *     LP_rxBufferRead index.
    *  LP_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when LP_rxBufferWrite is equal to
    *    LP_rxBufferRead
    *  LP_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  LP_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(LP_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef LP_RXISR_ENTRY_CALLBACK
        LP_RXISR_EntryCallback();
    #endif /* LP_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START LP_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = LP_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in LP_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (LP_RX_STS_BREAK | 
                            LP_RX_STS_PAR_ERROR |
                            LP_RX_STS_STOP_ERROR | 
                            LP_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                LP_errorStatus |= readStatus & ( LP_RX_STS_BREAK | 
                                                            LP_RX_STS_PAR_ERROR | 
                                                            LP_RX_STS_STOP_ERROR | 
                                                            LP_RX_STS_OVERRUN);
                /* `#START LP_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef LP_RXISR_ERROR_CALLBACK
                LP_RXISR_ERROR_Callback();
            #endif /* LP_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & LP_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = LP_RXDATA_REG;
            #if (LP_RXHW_ADDRESS_ENABLED)
                if(LP_rxAddressMode == (uint8)LP__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & LP_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & LP_RX_STS_ADDR_MATCH) != 0u)
                        {
                            LP_rxAddressDetected = 1u;
                        }
                        else
                        {
                            LP_rxAddressDetected = 0u;
                        }
                    }
                    if(LP_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        LP_rxBuffer[LP_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    LP_rxBuffer[LP_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                LP_rxBuffer[LP_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (LP_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(LP_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        LP_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    LP_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(LP_rxBufferWrite >= LP_RX_BUFFER_SIZE)
                    {
                        LP_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(LP_rxBufferWrite == LP_rxBufferRead)
                    {
                        LP_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (LP_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            LP_RXSTATUS_MASK_REG  &= (uint8)~LP_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(LP_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (LP_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & LP_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START LP_RXISR_END` */

        /* `#END` */

    #ifdef LP_RXISR_EXIT_CALLBACK
        LP_RXISR_ExitCallback();
    #endif /* LP_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (LP_RX_INTERRUPT_ENABLED && (LP_RX_ENABLED || LP_HD_ENABLED)) */


#if (LP_TX_INTERRUPT_ENABLED && LP_TX_ENABLED)
    /*******************************************************************************
    * Function Name: LP_TXISR
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
    *  LP_txBuffer - RAM buffer pointer for transmit data from.
    *  LP_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  LP_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(LP_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef LP_TXISR_ENTRY_CALLBACK
        LP_TXISR_EntryCallback();
    #endif /* LP_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START LP_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((LP_txBufferRead != LP_txBufferWrite) &&
             ((LP_TXSTATUS_REG & LP_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(LP_txBufferRead >= LP_TX_BUFFER_SIZE)
            {
                LP_txBufferRead = 0u;
            }

            LP_TXDATA_REG = LP_txBuffer[LP_txBufferRead];

            /* Set next pointer */
            LP_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START LP_TXISR_END` */

        /* `#END` */

    #ifdef LP_TXISR_EXIT_CALLBACK
        LP_TXISR_ExitCallback();
    #endif /* LP_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (LP_TX_INTERRUPT_ENABLED && LP_TX_ENABLED) */


/* [] END OF FILE */
