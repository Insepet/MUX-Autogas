/*******************************************************************************
* File Name: PCINT.c
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

#include "PC.h"



/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (PC_RX_INTERRUPT_ENABLED && (PC_RX_ENABLED || PC_HD_ENABLED))
    /*******************************************************************************
    * Function Name: PC_RXISR
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
    *  PC_rxBuffer - RAM buffer pointer for save received data.
    *  PC_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  PC_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  PC_rxBufferOverflow - software overflow flag. Set to one
    *     when PC_rxBufferWrite index overtakes
    *     PC_rxBufferRead index.
    *  PC_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when PC_rxBufferWrite is equal to
    *    PC_rxBufferRead
    *  PC_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  PC_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(PC_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef PC_RXISR_ENTRY_CALLBACK
        PC_RXISR_EntryCallback();
    #endif /* PC_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START PC_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = PC_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in PC_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (PC_RX_STS_BREAK | 
                            PC_RX_STS_PAR_ERROR |
                            PC_RX_STS_STOP_ERROR | 
                            PC_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                PC_errorStatus |= readStatus & ( PC_RX_STS_BREAK | 
                                                            PC_RX_STS_PAR_ERROR | 
                                                            PC_RX_STS_STOP_ERROR | 
                                                            PC_RX_STS_OVERRUN);
                /* `#START PC_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef PC_RXISR_ERROR_CALLBACK
                PC_RXISR_ERROR_Callback();
            #endif /* PC_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & PC_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = PC_RXDATA_REG;
            #if (PC_RXHW_ADDRESS_ENABLED)
                if(PC_rxAddressMode == (uint8)PC__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & PC_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & PC_RX_STS_ADDR_MATCH) != 0u)
                        {
                            PC_rxAddressDetected = 1u;
                        }
                        else
                        {
                            PC_rxAddressDetected = 0u;
                        }
                    }
                    if(PC_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        PC_rxBuffer[PC_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    PC_rxBuffer[PC_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                PC_rxBuffer[PC_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (PC_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(PC_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        PC_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    PC_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(PC_rxBufferWrite >= PC_RX_BUFFER_SIZE)
                    {
                        PC_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(PC_rxBufferWrite == PC_rxBufferRead)
                    {
                        PC_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (PC_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            PC_RXSTATUS_MASK_REG  &= (uint8)~PC_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(PC_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (PC_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & PC_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START PC_RXISR_END` */

        /* `#END` */

    #ifdef PC_RXISR_EXIT_CALLBACK
        PC_RXISR_ExitCallback();
    #endif /* PC_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (PC_RX_INTERRUPT_ENABLED && (PC_RX_ENABLED || PC_HD_ENABLED)) */


#if (PC_TX_INTERRUPT_ENABLED && PC_TX_ENABLED)
    /*******************************************************************************
    * Function Name: PC_TXISR
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
    *  PC_txBuffer - RAM buffer pointer for transmit data from.
    *  PC_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  PC_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(PC_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef PC_TXISR_ENTRY_CALLBACK
        PC_TXISR_EntryCallback();
    #endif /* PC_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START PC_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((PC_txBufferRead != PC_txBufferWrite) &&
             ((PC_TXSTATUS_REG & PC_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(PC_txBufferRead >= PC_TX_BUFFER_SIZE)
            {
                PC_txBufferRead = 0u;
            }

            PC_TXDATA_REG = PC_txBuffer[PC_txBufferRead];

            /* Set next pointer */
            PC_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START PC_TXISR_END` */

        /* `#END` */

    #ifdef PC_TXISR_EXIT_CALLBACK
        PC_TXISR_ExitCallback();
    #endif /* PC_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (PC_TX_INTERRUPT_ENABLED && PC_TX_ENABLED) */


/* [] END OF FILE */
