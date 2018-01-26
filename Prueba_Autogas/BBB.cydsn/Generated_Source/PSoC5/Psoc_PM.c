/*******************************************************************************
* File Name: Psoc_PM.c
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

#include "Psoc.h"


/***************************************
* Local data allocation
***************************************/

static Psoc_BACKUP_STRUCT  Psoc_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Psoc_SaveConfig
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
*  Psoc_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Psoc_SaveConfig(void)
{
    #if (CY_UDB_V0)

        #if(Psoc_CONTROL_REG_REMOVED == 0u)
            Psoc_backup.cr = Psoc_CONTROL_REG;
        #endif /* End Psoc_CONTROL_REG_REMOVED */

        #if( (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) )
            Psoc_backup.rx_period = Psoc_RXBITCTR_PERIOD_REG;
            Psoc_backup.rx_mask = Psoc_RXSTATUS_MASK_REG;
            #if (Psoc_RXHW_ADDRESS_ENABLED)
                Psoc_backup.rx_addr1 = Psoc_RXADDRESS1_REG;
                Psoc_backup.rx_addr2 = Psoc_RXADDRESS2_REG;
            #endif /* End Psoc_RXHW_ADDRESS_ENABLED */
        #endif /* End Psoc_RX_ENABLED | Psoc_HD_ENABLED*/

        #if(Psoc_TX_ENABLED)
            #if(Psoc_TXCLKGEN_DP)
                Psoc_backup.tx_clk_ctr = Psoc_TXBITCLKGEN_CTR_REG;
                Psoc_backup.tx_clk_compl = Psoc_TXBITCLKTX_COMPLETE_REG;
            #else
                Psoc_backup.tx_period = Psoc_TXBITCTR_PERIOD_REG;
            #endif /*End Psoc_TXCLKGEN_DP */
            Psoc_backup.tx_mask = Psoc_TXSTATUS_MASK_REG;
        #endif /*End Psoc_TX_ENABLED */


    #else /* CY_UDB_V1 */

        #if(Psoc_CONTROL_REG_REMOVED == 0u)
            Psoc_backup.cr = Psoc_CONTROL_REG;
        #endif /* End Psoc_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: Psoc_RestoreConfig
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
*  Psoc_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Psoc_RestoreConfig(void)
{

    #if (CY_UDB_V0)

        #if(Psoc_CONTROL_REG_REMOVED == 0u)
            Psoc_CONTROL_REG = Psoc_backup.cr;
        #endif /* End Psoc_CONTROL_REG_REMOVED */

        #if( (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) )
            Psoc_RXBITCTR_PERIOD_REG = Psoc_backup.rx_period;
            Psoc_RXSTATUS_MASK_REG = Psoc_backup.rx_mask;
            #if (Psoc_RXHW_ADDRESS_ENABLED)
                Psoc_RXADDRESS1_REG = Psoc_backup.rx_addr1;
                Psoc_RXADDRESS2_REG = Psoc_backup.rx_addr2;
            #endif /* End Psoc_RXHW_ADDRESS_ENABLED */
        #endif  /* End (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) */

        #if(Psoc_TX_ENABLED)
            #if(Psoc_TXCLKGEN_DP)
                Psoc_TXBITCLKGEN_CTR_REG = Psoc_backup.tx_clk_ctr;
                Psoc_TXBITCLKTX_COMPLETE_REG = Psoc_backup.tx_clk_compl;
            #else
                Psoc_TXBITCTR_PERIOD_REG = Psoc_backup.tx_period;
            #endif /*End Psoc_TXCLKGEN_DP */
            Psoc_TXSTATUS_MASK_REG = Psoc_backup.tx_mask;
        #endif /*End Psoc_TX_ENABLED */

    #else /* CY_UDB_V1 */

        #if(Psoc_CONTROL_REG_REMOVED == 0u)
            Psoc_CONTROL_REG = Psoc_backup.cr;
        #endif /* End Psoc_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: Psoc_Sleep
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
*  Psoc_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Psoc_Sleep(void)
{

    #if(Psoc_RX_ENABLED || Psoc_HD_ENABLED)
        if((Psoc_RXSTATUS_ACTL_REG  & Psoc_INT_ENABLE) != 0u)
        {
            Psoc_backup.enableState = 1u;
        }
        else
        {
            Psoc_backup.enableState = 0u;
        }
    #else
        if((Psoc_TXSTATUS_ACTL_REG  & Psoc_INT_ENABLE) !=0u)
        {
            Psoc_backup.enableState = 1u;
        }
        else
        {
            Psoc_backup.enableState = 0u;
        }
    #endif /* End Psoc_RX_ENABLED || Psoc_HD_ENABLED*/

    Psoc_Stop();
    Psoc_SaveConfig();
}


/*******************************************************************************
* Function Name: Psoc_Wakeup
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
*  Psoc_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Psoc_Wakeup(void)
{
    Psoc_RestoreConfig();
    #if( (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) )
        Psoc_ClearRxBuffer();
    #endif /* End (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) */
    #if(Psoc_TX_ENABLED || Psoc_HD_ENABLED)
        Psoc_ClearTxBuffer();
    #endif /* End Psoc_TX_ENABLED || Psoc_HD_ENABLED */

    if(Psoc_backup.enableState != 0u)
    {
        Psoc_Enable();
    }
}


/* [] END OF FILE */
