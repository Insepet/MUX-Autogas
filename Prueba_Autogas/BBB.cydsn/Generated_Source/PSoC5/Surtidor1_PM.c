/*******************************************************************************
* File Name: Surtidor1_PM.c
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

#include "Surtidor1.h"


/***************************************
* Local data allocation
***************************************/

static Surtidor1_BACKUP_STRUCT  Surtidor1_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Surtidor1_SaveConfig
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
*  Surtidor1_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Surtidor1_SaveConfig(void)
{
    #if (CY_UDB_V0)

        #if(Surtidor1_CONTROL_REG_REMOVED == 0u)
            Surtidor1_backup.cr = Surtidor1_CONTROL_REG;
        #endif /* End Surtidor1_CONTROL_REG_REMOVED */

        #if( (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) )
            Surtidor1_backup.rx_period = Surtidor1_RXBITCTR_PERIOD_REG;
            Surtidor1_backup.rx_mask = Surtidor1_RXSTATUS_MASK_REG;
            #if (Surtidor1_RXHW_ADDRESS_ENABLED)
                Surtidor1_backup.rx_addr1 = Surtidor1_RXADDRESS1_REG;
                Surtidor1_backup.rx_addr2 = Surtidor1_RXADDRESS2_REG;
            #endif /* End Surtidor1_RXHW_ADDRESS_ENABLED */
        #endif /* End Surtidor1_RX_ENABLED | Surtidor1_HD_ENABLED*/

        #if(Surtidor1_TX_ENABLED)
            #if(Surtidor1_TXCLKGEN_DP)
                Surtidor1_backup.tx_clk_ctr = Surtidor1_TXBITCLKGEN_CTR_REG;
                Surtidor1_backup.tx_clk_compl = Surtidor1_TXBITCLKTX_COMPLETE_REG;
            #else
                Surtidor1_backup.tx_period = Surtidor1_TXBITCTR_PERIOD_REG;
            #endif /*End Surtidor1_TXCLKGEN_DP */
            Surtidor1_backup.tx_mask = Surtidor1_TXSTATUS_MASK_REG;
        #endif /*End Surtidor1_TX_ENABLED */


    #else /* CY_UDB_V1 */

        #if(Surtidor1_CONTROL_REG_REMOVED == 0u)
            Surtidor1_backup.cr = Surtidor1_CONTROL_REG;
        #endif /* End Surtidor1_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: Surtidor1_RestoreConfig
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
*  Surtidor1_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Surtidor1_RestoreConfig(void)
{

    #if (CY_UDB_V0)

        #if(Surtidor1_CONTROL_REG_REMOVED == 0u)
            Surtidor1_CONTROL_REG = Surtidor1_backup.cr;
        #endif /* End Surtidor1_CONTROL_REG_REMOVED */

        #if( (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) )
            Surtidor1_RXBITCTR_PERIOD_REG = Surtidor1_backup.rx_period;
            Surtidor1_RXSTATUS_MASK_REG = Surtidor1_backup.rx_mask;
            #if (Surtidor1_RXHW_ADDRESS_ENABLED)
                Surtidor1_RXADDRESS1_REG = Surtidor1_backup.rx_addr1;
                Surtidor1_RXADDRESS2_REG = Surtidor1_backup.rx_addr2;
            #endif /* End Surtidor1_RXHW_ADDRESS_ENABLED */
        #endif  /* End (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) */

        #if(Surtidor1_TX_ENABLED)
            #if(Surtidor1_TXCLKGEN_DP)
                Surtidor1_TXBITCLKGEN_CTR_REG = Surtidor1_backup.tx_clk_ctr;
                Surtidor1_TXBITCLKTX_COMPLETE_REG = Surtidor1_backup.tx_clk_compl;
            #else
                Surtidor1_TXBITCTR_PERIOD_REG = Surtidor1_backup.tx_period;
            #endif /*End Surtidor1_TXCLKGEN_DP */
            Surtidor1_TXSTATUS_MASK_REG = Surtidor1_backup.tx_mask;
        #endif /*End Surtidor1_TX_ENABLED */

    #else /* CY_UDB_V1 */

        #if(Surtidor1_CONTROL_REG_REMOVED == 0u)
            Surtidor1_CONTROL_REG = Surtidor1_backup.cr;
        #endif /* End Surtidor1_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: Surtidor1_Sleep
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
*  Surtidor1_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Surtidor1_Sleep(void)
{

    #if(Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED)
        if((Surtidor1_RXSTATUS_ACTL_REG  & Surtidor1_INT_ENABLE) != 0u)
        {
            Surtidor1_backup.enableState = 1u;
        }
        else
        {
            Surtidor1_backup.enableState = 0u;
        }
    #else
        if((Surtidor1_TXSTATUS_ACTL_REG  & Surtidor1_INT_ENABLE) !=0u)
        {
            Surtidor1_backup.enableState = 1u;
        }
        else
        {
            Surtidor1_backup.enableState = 0u;
        }
    #endif /* End Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED*/

    Surtidor1_Stop();
    Surtidor1_SaveConfig();
}


/*******************************************************************************
* Function Name: Surtidor1_Wakeup
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
*  Surtidor1_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Surtidor1_Wakeup(void)
{
    Surtidor1_RestoreConfig();
    #if( (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) )
        Surtidor1_ClearRxBuffer();
    #endif /* End (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) */
    #if(Surtidor1_TX_ENABLED || Surtidor1_HD_ENABLED)
        Surtidor1_ClearTxBuffer();
    #endif /* End Surtidor1_TX_ENABLED || Surtidor1_HD_ENABLED */

    if(Surtidor1_backup.enableState != 0u)
    {
        Surtidor1_Enable();
    }
}


/* [] END OF FILE */
