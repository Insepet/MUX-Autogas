/*******************************************************************************
* File Name: PC_PM.c
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

#include "PC.h"


/***************************************
* Local data allocation
***************************************/

static PC_BACKUP_STRUCT  PC_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: PC_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the PC_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PC_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PC_SaveConfig(void)
{
    #if(PC_CONTROL_REG_REMOVED == 0u)
        PC_backup.cr = PC_CONTROL_REG;
    #endif /* End PC_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: PC_RestoreConfig
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
*  PC_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling PC_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void PC_RestoreConfig(void)
{
    #if(PC_CONTROL_REG_REMOVED == 0u)
        PC_CONTROL_REG = PC_backup.cr;
    #endif /* End PC_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: PC_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The PC_Sleep() API saves the current component state. Then it
*  calls the PC_Stop() function and calls 
*  PC_SaveConfig() to save the hardware configuration.
*  Call the PC_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PC_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PC_Sleep(void)
{
    #if(PC_RX_ENABLED || PC_HD_ENABLED)
        if((PC_RXSTATUS_ACTL_REG  & PC_INT_ENABLE) != 0u)
        {
            PC_backup.enableState = 1u;
        }
        else
        {
            PC_backup.enableState = 0u;
        }
    #else
        if((PC_TXSTATUS_ACTL_REG  & PC_INT_ENABLE) !=0u)
        {
            PC_backup.enableState = 1u;
        }
        else
        {
            PC_backup.enableState = 0u;
        }
    #endif /* End PC_RX_ENABLED || PC_HD_ENABLED*/

    PC_Stop();
    PC_SaveConfig();
}


/*******************************************************************************
* Function Name: PC_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  PC_Sleep() was called. The PC_Wakeup() function
*  calls the PC_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  PC_Sleep() function was called, the PC_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PC_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PC_Wakeup(void)
{
    PC_RestoreConfig();
    #if( (PC_RX_ENABLED) || (PC_HD_ENABLED) )
        PC_ClearRxBuffer();
    #endif /* End (PC_RX_ENABLED) || (PC_HD_ENABLED) */
    #if(PC_TX_ENABLED || PC_HD_ENABLED)
        PC_ClearTxBuffer();
    #endif /* End PC_TX_ENABLED || PC_HD_ENABLED */

    if(PC_backup.enableState != 0u)
    {
        PC_Enable();
    }
}


/* [] END OF FILE */
