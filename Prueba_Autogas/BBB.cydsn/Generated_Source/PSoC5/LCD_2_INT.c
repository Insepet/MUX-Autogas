/*******************************************************************************
* File Name: LCD_2INT.c
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

#include "LCD_2.h"



/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (LCD_2_RX_INTERRUPT_ENABLED && (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED))
    /*******************************************************************************
    * Function Name: LCD_2_RXISR
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
    *  LCD_2_rxBuffer - RAM buffer pointer for save received data.
    *  LCD_2_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  LCD_2_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  LCD_2_rxBufferOverflow - software overflow flag. Set to one
    *     when LCD_2_rxBufferWrite index overtakes
    *     LCD_2_rxBufferRead index.
    *  LCD_2_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when LCD_2_rxBufferWrite is equal to
    *    LCD_2_rxBufferRead
    *  LCD_2_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  LCD_2_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(LCD_2_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef LCD_2_RXISR_ENTRY_CALLBACK
        LCD_2_RXISR_EntryCallback();
    #endif /* LCD_2_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START LCD_2_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = LCD_2_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in LCD_2_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (LCD_2_RX_STS_BREAK | 
                            LCD_2_RX_STS_PAR_ERROR |
                            LCD_2_RX_STS_STOP_ERROR | 
                            LCD_2_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                LCD_2_errorStatus |= readStatus & ( LCD_2_RX_STS_BREAK | 
                                                            LCD_2_RX_STS_PAR_ERROR | 
                                                            LCD_2_RX_STS_STOP_ERROR | 
                                                            LCD_2_RX_STS_OVERRUN);
                /* `#START LCD_2_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef LCD_2_RXISR_ERROR_CALLBACK
                LCD_2_RXISR_ERROR_Callback();
            #endif /* LCD_2_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & LCD_2_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = LCD_2_RXDATA_REG;
            #if (LCD_2_RXHW_ADDRESS_ENABLED)
                if(LCD_2_rxAddressMode == (uint8)LCD_2__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & LCD_2_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & LCD_2_RX_STS_ADDR_MATCH) != 0u)
                        {
                            LCD_2_rxAddressDetected = 1u;
                        }
                        else
                        {
                            LCD_2_rxAddressDetected = 0u;
                        }
                    }
                    if(LCD_2_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        LCD_2_rxBuffer[LCD_2_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    LCD_2_rxBuffer[LCD_2_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                LCD_2_rxBuffer[LCD_2_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (LCD_2_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(LCD_2_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        LCD_2_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    LCD_2_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(LCD_2_rxBufferWrite >= LCD_2_RX_BUFFER_SIZE)
                    {
                        LCD_2_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(LCD_2_rxBufferWrite == LCD_2_rxBufferRead)
                    {
                        LCD_2_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (LCD_2_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            LCD_2_RXSTATUS_MASK_REG  &= (uint8)~LCD_2_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(LCD_2_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (LCD_2_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & LCD_2_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START LCD_2_RXISR_END` */

        /* `#END` */

    #ifdef LCD_2_RXISR_EXIT_CALLBACK
        LCD_2_RXISR_ExitCallback();
    #endif /* LCD_2_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (LCD_2_RX_INTERRUPT_ENABLED && (LCD_2_RX_ENABLED || LCD_2_HD_ENABLED)) */


#if (LCD_2_TX_INTERRUPT_ENABLED && LCD_2_TX_ENABLED)
    /*******************************************************************************
    * Function Name: LCD_2_TXISR
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
    *  LCD_2_txBuffer - RAM buffer pointer for transmit data from.
    *  LCD_2_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  LCD_2_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(LCD_2_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef LCD_2_TXISR_ENTRY_CALLBACK
        LCD_2_TXISR_EntryCallback();
    #endif /* LCD_2_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START LCD_2_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((LCD_2_txBufferRead != LCD_2_txBufferWrite) &&
             ((LCD_2_TXSTATUS_REG & LCD_2_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(LCD_2_txBufferRead >= LCD_2_TX_BUFFER_SIZE)
            {
                LCD_2_txBufferRead = 0u;
            }

            LCD_2_TXDATA_REG = LCD_2_txBuffer[LCD_2_txBufferRead];

            /* Set next pointer */
            LCD_2_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START LCD_2_TXISR_END` */

        /* `#END` */

    #ifdef LCD_2_TXISR_EXIT_CALLBACK
        LCD_2_TXISR_ExitCallback();
    #endif /* LCD_2_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (LCD_2_TX_INTERRUPT_ENABLED && LCD_2_TX_ENABLED) */


/* [] END OF FILE */
