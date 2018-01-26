/*******************************************************************************
* File Name: UART_3_PM.c
* Version 2.30
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_3.h"


/***************************************
* Local data allocation
***************************************/

static UART_3_BACKUP_STRUCT  UART_3_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: UART_3_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART_3_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART_3_SaveConfig(void)
{
    #if (CY_UDB_V0)

        #if(UART_3_CONTROL_REG_REMOVED == 0u)
            UART_3_backup.cr = UART_3_CONTROL_REG;
        #endif /* End UART_3_CONTROL_REG_REMOVED */

        #if( (UART_3_RX_ENABLED) || (UART_3_HD_ENABLED) )
            UART_3_backup.rx_period = UART_3_RXBITCTR_PERIOD_REG;
            UART_3_backup.rx_mask = UART_3_RXSTATUS_MASK_REG;
            #if (UART_3_RXHW_ADDRESS_ENABLED)
                UART_3_backup.rx_addr1 = UART_3_RXADDRESS1_REG;
                UART_3_backup.rx_addr2 = UART_3_RXADDRESS2_REG;
            #endif /* End UART_3_RXHW_ADDRESS_ENABLED */
        #endif /* End UART_3_RX_ENABLED | UART_3_HD_ENABLED*/

        #if(UART_3_TX_ENABLED)
            #if(UART_3_TXCLKGEN_DP)
                UART_3_backup.tx_clk_ctr = UART_3_TXBITCLKGEN_CTR_REG;
                UART_3_backup.tx_clk_compl = UART_3_TXBITCLKTX_COMPLETE_REG;
            #else
                UART_3_backup.tx_period = UART_3_TXBITCTR_PERIOD_REG;
            #endif /*End UART_3_TXCLKGEN_DP */
            UART_3_backup.tx_mask = UART_3_TXSTATUS_MASK_REG;
        #endif /*End UART_3_TX_ENABLED */


    #else /* CY_UDB_V1 */

        #if(UART_3_CONTROL_REG_REMOVED == 0u)
            UART_3_backup.cr = UART_3_CONTROL_REG;
        #endif /* End UART_3_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: UART_3_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART_3_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART_3_RestoreConfig(void)
{

    #if (CY_UDB_V0)

        #if(UART_3_CONTROL_REG_REMOVED == 0u)
            UART_3_CONTROL_REG = UART_3_backup.cr;
        #endif /* End UART_3_CONTROL_REG_REMOVED */

        #if( (UART_3_RX_ENABLED) || (UART_3_HD_ENABLED) )
            UART_3_RXBITCTR_PERIOD_REG = UART_3_backup.rx_period;
            UART_3_RXSTATUS_MASK_REG = UART_3_backup.rx_mask;
            #if (UART_3_RXHW_ADDRESS_ENABLED)
                UART_3_RXADDRESS1_REG = UART_3_backup.rx_addr1;
                UART_3_RXADDRESS2_REG = UART_3_backup.rx_addr2;
            #endif /* End UART_3_RXHW_ADDRESS_ENABLED */
        #endif  /* End (UART_3_RX_ENABLED) || (UART_3_HD_ENABLED) */

        #if(UART_3_TX_ENABLED)
            #if(UART_3_TXCLKGEN_DP)
                UART_3_TXBITCLKGEN_CTR_REG = UART_3_backup.tx_clk_ctr;
                UART_3_TXBITCLKTX_COMPLETE_REG = UART_3_backup.tx_clk_compl;
            #else
                UART_3_TXBITCTR_PERIOD_REG = UART_3_backup.tx_period;
            #endif /*End UART_3_TXCLKGEN_DP */
            UART_3_TXSTATUS_MASK_REG = UART_3_backup.tx_mask;
        #endif /*End UART_3_TX_ENABLED */

    #else /* CY_UDB_V1 */

        #if(UART_3_CONTROL_REG_REMOVED == 0u)
            UART_3_CONTROL_REG = UART_3_backup.cr;
        #endif /* End UART_3_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: UART_3_Sleep
********************************************************************************
*
* Summary:
*  Stops and saves the user configuration. Should be called
*  just prior to entering sleep.
*
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART_3_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART_3_Sleep(void)
{

    #if(UART_3_RX_ENABLED || UART_3_HD_ENABLED)
        if((UART_3_RXSTATUS_ACTL_REG  & UART_3_INT_ENABLE) != 0u)
        {
            UART_3_backup.enableState = 1u;
        }
        else
        {
            UART_3_backup.enableState = 0u;
        }
    #else
        if((UART_3_TXSTATUS_ACTL_REG  & UART_3_INT_ENABLE) !=0u)
        {
            UART_3_backup.enableState = 1u;
        }
        else
        {
            UART_3_backup.enableState = 0u;
        }
    #endif /* End UART_3_RX_ENABLED || UART_3_HD_ENABLED*/

    UART_3_Stop();
    UART_3_SaveConfig();
}


/*******************************************************************************
* Function Name: UART_3_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called
*  just after awaking from sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART_3_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART_3_Wakeup(void)
{
    UART_3_RestoreConfig();
    #if( (UART_3_RX_ENABLED) || (UART_3_HD_ENABLED) )
        UART_3_ClearRxBuffer();
    #endif /* End (UART_3_RX_ENABLED) || (UART_3_HD_ENABLED) */
    #if(UART_3_TX_ENABLED || UART_3_HD_ENABLED)
        UART_3_ClearTxBuffer();
    #endif /* End UART_3_TX_ENABLED || UART_3_HD_ENABLED */

    if(UART_3_backup.enableState != 0u)
    {
        UART_3_Enable();
    }
}


/* [] END OF FILE */
