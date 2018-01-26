/*******************************************************************************
* File Name: Surtidor1_INT.c
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

#include "Surtidor1.h"
#include "CyLib.h"


/***************************************
* Custom Declratations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if( (Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED) && \
     (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH))


    /*******************************************************************************
    * Function Name: Surtidor1_RXISR
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
    *  Surtidor1_rxBuffer - RAM buffer pointer for save received data.
    *  Surtidor1_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Surtidor1_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Surtidor1_rxBufferOverflow - software overflow flag. Set to one
    *     when Surtidor1_rxBufferWrite index overtakes
    *     Surtidor1_rxBufferRead index.
    *  Surtidor1_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Surtidor1_rxBufferWrite is equal to
    *    Surtidor1_rxBufferRead
    *  Surtidor1_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Surtidor1_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Surtidor1_RXISR)
    {
        uint8 readData;
        uint8 increment_pointer = 0u;
        #if(CY_PSOC3)
            uint8 int_en;
        #endif /* CY_PSOC3 */

        /* User code required at start of ISR */
        /* `#START Surtidor1_RXISR_START` */

        /* `#END` */

        #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
            int_en = EA;
            CyGlobalIntEnable;
        #endif /* CY_PSOC3 */

        readData = Surtidor1_RXSTATUS_REG;

        if((readData & (Surtidor1_RX_STS_BREAK | Surtidor1_RX_STS_PAR_ERROR |
                        Surtidor1_RX_STS_STOP_ERROR | Surtidor1_RX_STS_OVERRUN)) != 0u)
        {
            /* ERROR handling. */
            /* `#START Surtidor1_RXISR_ERROR` */

            /* `#END` */
        }

        while((readData & Surtidor1_RX_STS_FIFO_NOTEMPTY) != 0u)
        {

            #if (Surtidor1_RXHW_ADDRESS_ENABLED)
                if(Surtidor1_rxAddressMode == (uint8)Surtidor1__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readData & Surtidor1_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readData & Surtidor1_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Surtidor1_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Surtidor1_rxAddressDetected = 0u;
                        }
                    }

                    readData = Surtidor1_RXDATA_REG;
                    if(Surtidor1_rxAddressDetected != 0u)
                    {   /* store only addressed data */
                        Surtidor1_rxBuffer[Surtidor1_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* without software addressing */
                {
                    Surtidor1_rxBuffer[Surtidor1_rxBufferWrite] = Surtidor1_RXDATA_REG;
                    increment_pointer = 1u;
                }
            #else  /* without addressing */
                Surtidor1_rxBuffer[Surtidor1_rxBufferWrite] = Surtidor1_RXDATA_REG;
                increment_pointer = 1u;
            #endif /* End SW_DETECT_TO_BUFFER */

            /* do not increment buffer pointer when skip not adderessed data */
            if( increment_pointer != 0u )
            {
                if(Surtidor1_rxBufferLoopDetect != 0u)
                {   /* Set Software Buffer status Overflow */
                    Surtidor1_rxBufferOverflow = 1u;
                }
                /* Set next pointer. */
                Surtidor1_rxBufferWrite++;

                /* Check pointer for a loop condition */
                if(Surtidor1_rxBufferWrite >= Surtidor1_RXBUFFERSIZE)
                {
                    Surtidor1_rxBufferWrite = 0u;
                }
                /* Detect pre-overload condition and set flag */
                if(Surtidor1_rxBufferWrite == Surtidor1_rxBufferRead)
                {
                    Surtidor1_rxBufferLoopDetect = 1u;
                    /* When Hardware Flow Control selected */
                    #if(Surtidor1_FLOW_CONTROL != 0u)
                    /* Disable RX interrupt mask, it will be enabled when user read data from the buffer using APIs */
                        Surtidor1_RXSTATUS_MASK_REG  &= (uint8)~Surtidor1_RX_STS_FIFO_NOTEMPTY;
                        CyIntClearPending(Surtidor1_RX_VECT_NUM);
                        break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                    #endif /* End Surtidor1_FLOW_CONTROL != 0 */
                }
            }

            /* Check again if there is data. */
            readData = Surtidor1_RXSTATUS_REG;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Surtidor1_RXISR_END` */

        /* `#END` */

        #if(CY_PSOC3)
            EA = int_en;
        #endif /* CY_PSOC3 */

    }

#endif /* End Surtidor1_RX_ENABLED && (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH) */


#if(Surtidor1_TX_ENABLED && (Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH))


    /*******************************************************************************
    * Function Name: Surtidor1_TXISR
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
    *  Surtidor1_txBuffer - RAM buffer pointer for transmit data from.
    *  Surtidor1_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmited byte.
    *  Surtidor1_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Surtidor1_TXISR)
    {

        #if(CY_PSOC3)
            uint8 int_en;
        #endif /* CY_PSOC3 */

        /* User code required at start of ISR */
        /* `#START Surtidor1_TXISR_START` */

        /* `#END` */

        #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
            int_en = EA;
            CyGlobalIntEnable;
        #endif /* CY_PSOC3 */

        while((Surtidor1_txBufferRead != Surtidor1_txBufferWrite) &&
             ((Surtidor1_TXSTATUS_REG & Surtidor1_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer. */
            if(Surtidor1_txBufferRead >= Surtidor1_TXBUFFERSIZE)
            {
                Surtidor1_txBufferRead = 0u;
            }

            Surtidor1_TXDATA_REG = Surtidor1_txBuffer[Surtidor1_txBufferRead];

            /* Set next pointer. */
            Surtidor1_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Surtidor1_TXISR_END` */

        /* `#END` */

        #if(CY_PSOC3)
            EA = int_en;
        #endif /* CY_PSOC3 */

    }

#endif /* End Surtidor1_TX_ENABLED && (Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH) */


/* [] END OF FILE */
