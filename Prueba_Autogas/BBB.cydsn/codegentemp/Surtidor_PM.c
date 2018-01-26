/*******************************************************************************
* File Name: Surtidor_PM.c
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

#include "Surtidor.h"


/***************************************
* Local data allocation
***************************************/

static Surtidor_BACKUP_STRUCT  Surtidor_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Surtidor_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Surtidor_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Surtidor_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Surtidor_SaveConfig(void)
{
    #if(Surtidor_CONTROL_REG_REMOVED == 0u)
        Surtidor_backup.cr = Surtidor_CONTROL_REG;
    #endif /* End Surtidor_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Surtidor_RestoreConfig
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
*  Surtidor_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Surtidor_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Surtidor_RestoreConfig(void)
{
    #if(Surtidor_CONTROL_REG_REMOVED == 0u)
        Surtidor_CONTROL_REG = Surtidor_backup.cr;
    #endif /* End Surtidor_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Surtidor_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Surtidor_Sleep() API saves the current component state. Then it
*  calls the Surtidor_Stop() function and calls 
*  Surtidor_SaveConfig() to save the hardware configuration.
*  Call the Surtidor_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Surtidor_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Surtidor_Sleep(void)
{
    #if(Surtidor_RX_ENABLED || Surtidor_HD_ENABLED)
        if((Surtidor_RXSTATUS_ACTL_REG  & Surtidor_INT_ENABLE) != 0u)
        {
            Surtidor_backup.enableState = 1u;
        }
        else
        {
            Surtidor_backup.enableState = 0u;
        }
    #else
        if((Surtidor_TXSTATUS_ACTL_REG  & Surtidor_INT_ENABLE) !=0u)
        {
            Surtidor_backup.enableState = 1u;
        }
        else
        {
            Surtidor_backup.enableState = 0u;
        }
    #endif /* End Surtidor_RX_ENABLED || Surtidor_HD_ENABLED*/

    Surtidor_Stop();
    Surtidor_SaveConfig();
}


/*******************************************************************************
* Function Name: Surtidor_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Surtidor_Sleep() was called. The Surtidor_Wakeup() function
*  calls the Surtidor_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Surtidor_Sleep() function was called, the Surtidor_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Surtidor_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Surtidor_Wakeup(void)
{
    Surtidor_RestoreConfig();
    #if( (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) )
        Surtidor_ClearRxBuffer();
    #endif /* End (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) */
    #if(Surtidor_TX_ENABLED || Surtidor_HD_ENABLED)
        Surtidor_ClearTxBuffer();
    #endif /* End Surtidor_TX_ENABLED || Surtidor_HD_ENABLED */

    if(Surtidor_backup.enableState != 0u)
    {
        Surtidor_Enable();
    }
}


/* [] END OF FILE */
