/*******************************************************************************
* File Name: Psoc_INT.c
* Version 2.30
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
* Note:
*  Any unusual or non-standard behavior should be noted here. Other-
*  wise, this section should remain blank.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Psoc.h"
#include "CyLib.h"


/***************************************
* Custom Declratations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if( (Psoc_RX_ENABLED || Psoc_HD_ENABLED) && \
     (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH))


    /*******************************************************************************
    * Function Name: Psoc_RXISR
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
    *  Psoc_rxBuffer - RAM buffer pointer for save received data.
    *  Psoc_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Psoc_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Psoc_rxBufferOverflow - software overflow flag. Set to one
    *     when Psoc_rxBufferWrite index overtakes
    *     Psoc_rxBufferRead index.
    *  Psoc_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Psoc_rxBufferWrite is equal to
    *    Psoc_rxBufferRead
    *  Psoc_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Psoc_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Psoc_RXISR)
    {
        uint8 readData;
        uint8 increment_pointer = 0u;
        #if(CY_PSOC3)
            uint8 int_en;
        #endif /* CY_PSOC3 */

        /* User code required at start of ISR */
        /* `#START Psoc_RXISR_START` */

        /* `#END` */

        #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
            int_en = EA;
            CyGlobalIntEnable;
        #endif /* CY_PSOC3 */

        readData = Psoc_RXSTATUS_REG;

        if((readData & (Psoc_RX_STS_BREAK | Psoc_RX_STS_PAR_ERROR |
                        Psoc_RX_STS_STOP_ERROR | Psoc_RX_STS_OVERRUN)) != 0u)
        {
            /* ERROR handling. */
            /* `#START Psoc_RXISR_ERROR` */

            /* `#END` */
        }

        while((readData & Psoc_RX_STS_FIFO_NOTEMPTY) != 0u)
        {

            #if (Psoc_RXHW_ADDRESS_ENABLED)
                if(Psoc_rxAddressMode == (uint8)Psoc__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readData & Psoc_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readData & Psoc_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Psoc_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Psoc_rxAddressDetected = 0u;
                        }
                    }

                    readData = Psoc_RXDATA_REG;
                    if(Psoc_rxAddressDetected != 0u)
                    {   /* store only addressed data */
                        Psoc_rxBuffer[Psoc_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* without software addressing */
                {
                    Psoc_rxBuffer[Psoc_rxBufferWrite] = Psoc_RXDATA_REG;
                    increment_pointer = 1u;
                }
            #else  /* without addressing */
                Psoc_rxBuffer[Psoc_rxBufferWrite] = Psoc_RXDATA_REG;
                increment_pointer = 1u;
            #endif /* End SW_DETECT_TO_BUFFER */

            /* do not increment buffer pointer when skip not adderessed data */
            if( increment_pointer != 0u )
            {
                if(Psoc_rxBufferLoopDetect != 0u)
                {   /* Set Software Buffer status Overflow */
                    Psoc_rxBufferOverflow = 1u;
                }
                /* Set next pointer. */
                Psoc_rxBufferWrite++;

                /* Check pointer for a loop condition */
                if(Psoc_rxBufferWrite >= Psoc_RXBUFFERSIZE)
                {
                    Psoc_rxBufferWrite = 0u;
                }
                /* Detect pre-overload condition and set flag */
                if(Psoc_rxBufferWrite == Psoc_rxBufferRead)
                {
                    Psoc_rxBufferLoopDetect = 1u;
                    /* When Hardware Flow Control selected */
                    #if(Psoc_FLOW_CONTROL != 0u)
                    /* Disable RX interrupt mask, it will be enabled when user read data from the buffer using APIs */
                        Psoc_RXSTATUS_MASK_REG  &= (uint8)~Psoc_RX_STS_FIFO_NOTEMPTY;
                        CyIntClearPending(Psoc_RX_VECT_NUM);
                        break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                    #endif /* End Psoc_FLOW_CONTROL != 0 */
                }
            }

            /* Check again if there is data. */
            readData = Psoc_RXSTATUS_REG;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Psoc_RXISR_END` */

        /* `#END` */

        #if(CY_PSOC3)
            EA = int_en;
        #endif /* CY_PSOC3 */

    }

#endif /* End Psoc_RX_ENABLED && (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH) */


#if(Psoc_TX_ENABLED && (Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH))


    /*******************************************************************************
    * Function Name: Psoc_TXISR
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
    *  Psoc_txBuffer - RAM buffer pointer for transmit data from.
    *  Psoc_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmited byte.
    *  Psoc_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Psoc_TXISR)
    {

        #if(CY_PSOC3)
            uint8 int_en;
        #endif /* CY_PSOC3 */

        /* User code required at start of ISR */
        /* `#START Psoc_TXISR_START` */

        /* `#END` */

        #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
            int_en = EA;
            CyGlobalIntEnable;
        #endif /* CY_PSOC3 */

        while((Psoc_txBufferRead != Psoc_txBufferWrite) &&
             ((Psoc_TXSTATUS_REG & Psoc_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer. */
            if(Psoc_txBufferRead >= Psoc_TXBUFFERSIZE)
            {
                Psoc_txBufferRead = 0u;
            }

            Psoc_TXDATA_REG = Psoc_txBuffer[Psoc_txBufferRead];

            /* Set next pointer. */
            Psoc_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Psoc_TXISR_END` */

        /* `#END` */

        #if(CY_PSOC3)
            EA = int_en;
        #endif /* CY_PSOC3 */

    }

#endif /* End Psoc_TX_ENABLED && (Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH) */


/* [] END OF FILE */
