/*******************************************************************************
* File Name: Timer_Animacion_PM.c
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

#include "Timer_Animacion.h"

static Timer_Animacion_backupStruct Timer_Animacion_backup;


/*******************************************************************************
* Function Name: Timer_Animacion_SaveConfig
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
*  Timer_Animacion_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_Animacion_SaveConfig(void) 
{
    #if (!Timer_Animacion_UsingFixedFunction)
        Timer_Animacion_backup.TimerUdb = Timer_Animacion_ReadCounter();
        Timer_Animacion_backup.InterruptMaskValue = Timer_Animacion_STATUS_MASK;
        #if (Timer_Animacion_UsingHWCaptureCounter)
            Timer_Animacion_backup.TimerCaptureCounter = Timer_Animacion_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_Animacion_UDB_CONTROL_REG_REMOVED)
            Timer_Animacion_backup.TimerControlRegister = Timer_Animacion_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_Animacion_RestoreConfig
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
*  Timer_Animacion_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Animacion_RestoreConfig(void) 
{   
    #if (!Timer_Animacion_UsingFixedFunction)

        Timer_Animacion_WriteCounter(Timer_Animacion_backup.TimerUdb);
        Timer_Animacion_STATUS_MASK =Timer_Animacion_backup.InterruptMaskValue;
        #if (Timer_Animacion_UsingHWCaptureCounter)
            Timer_Animacion_SetCaptureCount(Timer_Animacion_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_Animacion_UDB_CONTROL_REG_REMOVED)
            Timer_Animacion_WriteControlRegister(Timer_Animacion_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_Animacion_Sleep
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
*  Timer_Animacion_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_Animacion_Sleep(void) 
{
    #if(!Timer_Animacion_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_Animacion_CTRL_ENABLE == (Timer_Animacion_CONTROL & Timer_Animacion_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_Animacion_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_Animacion_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_Animacion_Stop();
    Timer_Animacion_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_Animacion_Wakeup
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
*  Timer_Animacion_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Animacion_Wakeup(void) 
{
    Timer_Animacion_RestoreConfig();
    #if(!Timer_Animacion_UDB_CONTROL_REG_REMOVED)
        if(Timer_Animacion_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_Animacion_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
