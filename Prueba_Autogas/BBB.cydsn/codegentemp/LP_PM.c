/*******************************************************************************
* File Name: LP_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LP.h"


/***************************************
* Local data allocation
***************************************/

static LP_BACKUP_STRUCT  LP_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: LP_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the LP_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LP_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LP_SaveConfig(void)
{
    #if(LP_CONTROL_REG_REMOVED == 0u)
        LP_backup.cr = LP_CONTROL_REG;
    #endif /* End LP_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: LP_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LP_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling LP_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void LP_RestoreConfig(void)
{
    #if(LP_CONTROL_REG_REMOVED == 0u)
        LP_CONTROL_REG = LP_backup.cr;
    #endif /* End LP_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: LP_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The LP_Sleep() API saves the current component state. Then it
*  calls the LP_Stop() function and calls 
*  LP_SaveConfig() to save the hardware configuration.
*  Call the LP_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LP_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LP_Sleep(void)
{
    #if(LP_RX_ENABLED || LP_HD_ENABLED)
        if((LP_RXSTATUS_ACTL_REG  & LP_INT_ENABLE) != 0u)
        {
            LP_backup.enableState = 1u;
        }
        else
        {
            LP_backup.enableState = 0u;
        }
    #else
        if((LP_TXSTATUS_ACTL_REG  & LP_INT_ENABLE) !=0u)
        {
            LP_backup.enableState = 1u;
        }
        else
        {
            LP_backup.enableState = 0u;
        }
    #endif /* End LP_RX_ENABLED || LP_HD_ENABLED*/

    LP_Stop();
    LP_SaveConfig();
}


/*******************************************************************************
* Function Name: LP_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  LP_Sleep() was called. The LP_Wakeup() function
*  calls the LP_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  LP_Sleep() function was called, the LP_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LP_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LP_Wakeup(void)
{
    LP_RestoreConfig();
    #if( (LP_RX_ENABLED) || (LP_HD_ENABLED) )
        LP_ClearRxBuffer();
    #endif /* End (LP_RX_ENABLED) || (LP_HD_ENABLED) */
    #if(LP_TX_ENABLED || LP_HD_ENABLED)
        LP_ClearTxBuffer();
    #endif /* End LP_TX_ENABLED || LP_HD_ENABLED */

    if(LP_backup.enableState != 0u)
    {
        LP_Enable();
    }
}


/* [] END OF FILE */
