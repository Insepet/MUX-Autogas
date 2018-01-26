/*******************************************************************************
* File Name: LCD_1INT.c
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

#include "LCD_1.h"



/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (LCD_1_RX_INTERRUPT_ENABLED && (LCD_1_RX_ENABLED || LCD_1_HD_ENABLED))
    /*******************************************************************************
    * Function Name: LCD_1_RXISR
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
    *  LCD_1_rxBuffer - RAM buffer pointer for save received data.
    *  LCD_1_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  LCD_1_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  LCD_1_rxBufferOverflow - software overflow flag. Set to one
    *     when LCD_1_rxBufferWrite index overtakes
    *     LCD_1_rxBufferRead index.
    *  LCD_1_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when LCD_1_rxBufferWrite is equal to
    *    LCD_1_rxBufferRead
    *  LCD_1_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  LCD_1_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(LCD_1_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef LCD_1_RXISR_ENTRY_CALLBACK
        LCD_1_RXISR_EntryCallback();
    #endif /* LCD_1_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START LCD_1_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = LCD_1_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in LCD_1_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (LCD_1_RX_STS_BREAK | 
                            LCD_1_RX_STS_PAR_ERROR |
                            LCD_1_RX_STS_STOP_ERROR | 
                            LCD_1_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                LCD_1_errorStatus |= readStatus & ( LCD_1_RX_STS_BREAK | 
                                                            LCD_1_RX_STS_PAR_ERROR | 
                                                            LCD_1_RX_STS_STOP_ERROR | 
                                                            LCD_1_RX_STS_OVERRUN);
                /* `#START LCD_1_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef LCD_1_RXISR_ERROR_CALLBACK
                LCD_1_RXISR_ERROR_Callback();
            #endif /* LCD_1_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & LCD_1_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = LCD_1_RXDATA_REG;
            #if (LCD_1_RXHW_ADDRESS_ENABLED)
                if(LCD_1_rxAddressMode == (uint8)LCD_1__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & LCD_1_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & LCD_1_RX_STS_ADDR_MATCH) != 0u)
                        {
                            LCD_1_rxAddressDetected = 1u;
                        }
                        else
                        {
                            LCD_1_rxAddressDetected = 0u;
                        }
                    }
                    if(LCD_1_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        LCD_1_rxBuffer[LCD_1_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    LCD_1_rxBuffer[LCD_1_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                LCD_1_rxBuffer[LCD_1_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (LCD_1_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(LCD_1_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        LCD_1_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    LCD_1_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(LCD_1_rxBufferWrite >= LCD_1_RX_BUFFER_SIZE)
                    {
                        LCD_1_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(LCD_1_rxBufferWrite == LCD_1_rxBufferRead)
                    {
                        LCD_1_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (LCD_1_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            LCD_1_RXSTATUS_MASK_REG  &= (uint8)~LCD_1_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(LCD_1_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (LCD_1_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & LCD_1_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START LCD_1_RXISR_END` */

        /* `#END` */

    #ifdef LCD_1_RXISR_EXIT_CALLBACK
        LCD_1_RXISR_ExitCallback();
    #endif /* LCD_1_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (LCD_1_RX_INTERRUPT_ENABLED && (LCD_1_RX_ENABLED || LCD_1_HD_ENABLED)) */


#if (LCD_1_TX_INTERRUPT_ENABLED && LCD_1_TX_ENABLED)
    /*******************************************************************************
    * Function Name: LCD_1_TXISR
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
    *  LCD_1_txBuffer - RAM buffer pointer for transmit data from.
    *  LCD_1_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  LCD_1_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(LCD_1_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef LCD_1_TXISR_ENTRY_CALLBACK
        LCD_1_TXISR_EntryCallback();
    #endif /* LCD_1_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START LCD_1_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((LCD_1_txBufferRead != LCD_1_txBufferWrite) &&
             ((LCD_1_TXSTATUS_REG & LCD_1_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(LCD_1_txBufferRead >= LCD_1_TX_BUFFER_SIZE)
            {
                LCD_1_txBufferRead = 0u;
            }

            LCD_1_TXDATA_REG = LCD_1_txBuffer[LCD_1_txBufferRead];

            /* Set next pointer */
            LCD_1_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START LCD_1_TXISR_END` */

        /* `#END` */

    #ifdef LCD_1_TXISR_EXIT_CALLBACK
        LCD_1_TXISR_ExitCallback();
    #endif /* LCD_1_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (LCD_1_TX_INTERRUPT_ENABLED && LCD_1_TX_ENABLED) */


/* [] END OF FILE */
