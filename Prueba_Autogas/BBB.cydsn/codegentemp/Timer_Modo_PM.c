/*******************************************************************************
* File Name: Timer_Modo_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_Modo.h"

static Timer_Modo_backupStruct Timer_Modo_backup;


/*******************************************************************************
* Function Name: Timer_Modo_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Modo_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_Modo_SaveConfig(void) 
{
    #if (!Timer_Modo_UsingFixedFunction)
        Timer_Modo_backup.TimerUdb = Timer_Modo_ReadCounter();
        Timer_Modo_backup.InterruptMaskValue = Timer_Modo_STATUS_MASK;
        #if (Timer_Modo_UsingHWCaptureCounter)
            Timer_Modo_backup.TimerCaptureCounter = Timer_Modo_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_Modo_UDB_CONTROL_REG_REMOVED)
            Timer_Modo_backup.TimerControlRegister = Timer_Modo_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_Modo_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Modo_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Modo_RestoreConfig(void) 
{   
    #if (!Timer_Modo_UsingFixedFunction)

        Timer_Modo_WriteCounter(Timer_Modo_backup.TimerUdb);
        Timer_Modo_STATUS_MASK =Timer_Modo_backup.InterruptMaskValue;
        #if (Timer_Modo_UsingHWCaptureCounter)
            Timer_Modo_SetCaptureCount(Timer_Modo_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_Modo_UDB_CONTROL_REG_REMOVED)
            Timer_Modo_WriteControlRegister(Timer_Modo_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_Modo_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Modo_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_Modo_Sleep(void) 
{
    #if(!Timer_Modo_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_Modo_CTRL_ENABLE == (Timer_Modo_CONTROL & Timer_Modo_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_Modo_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_Modo_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_Modo_Stop();
    Timer_Modo_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_Modo_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Modo_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Modo_Wakeup(void) 
{
    Timer_Modo_RestoreConfig();
    #if(!Timer_Modo_UDB_CONTROL_REG_REMOVED)
        if(Timer_Modo_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_Modo_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
