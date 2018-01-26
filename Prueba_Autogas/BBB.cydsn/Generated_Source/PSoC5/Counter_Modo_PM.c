/*******************************************************************************
* File Name: Counter_Modo_PM.c  
* Version 2.40
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "Counter_Modo.h"

static Counter_Modo_backupStruct Counter_Modo_backup;


/*******************************************************************************
* Function Name: Counter_Modo_SaveConfig
********************************************************************************
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
*  Counter_Modo_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Counter_Modo_SaveConfig(void) 
{
    #if (!Counter_Modo_UsingFixedFunction)

        Counter_Modo_backup.CounterUdb = Counter_Modo_ReadCounter();

        #if (CY_UDB_V0)
            Counter_Modo_backup.CounterPeriod = Counter_Modo_ReadPeriod();
            Counter_Modo_backup.CompareValue = Counter_Modo_ReadCompare();
            Counter_Modo_backup.InterruptMaskValue = Counter_Modo_STATUS_MASK;
        #endif /* CY_UDB_V0 */

        #if(!Counter_Modo_ControlRegRemoved)
            Counter_Modo_backup.CounterControlRegister = Counter_Modo_ReadControlRegister();
        #endif /* (!Counter_Modo_ControlRegRemoved) */

    #endif /* (!Counter_Modo_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Counter_Modo_RestoreConfig
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
*  Counter_Modo_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Counter_Modo_RestoreConfig(void) 
{      
    #if (!Counter_Modo_UsingFixedFunction)

        #if (CY_UDB_V0)
            uint8 Counter_Modo_interruptState;
        #endif  /* (CY_UDB_V0) */

       Counter_Modo_WriteCounter(Counter_Modo_backup.CounterUdb);

        #if (CY_UDB_V0)
            Counter_Modo_WritePeriod(Counter_Modo_backup.CounterPeriod);
            Counter_Modo_WriteCompare(Counter_Modo_backup.CompareValue);

            Counter_Modo_interruptState = CyEnterCriticalSection();
            Counter_Modo_STATUS_AUX_CTRL |= Counter_Modo_STATUS_ACTL_INT_EN_MASK;
            CyExitCriticalSection(Counter_Modo_interruptState);

            Counter_Modo_STATUS_MASK = Counter_Modo_backup.InterruptMaskValue;
        #endif  /* (CY_UDB_V0) */

        #if(!Counter_Modo_ControlRegRemoved)
            Counter_Modo_WriteControlRegister(Counter_Modo_backup.CounterControlRegister);
        #endif /* (!Counter_Modo_ControlRegRemoved) */

    #endif /* (!Counter_Modo_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Counter_Modo_Sleep
********************************************************************************
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
*  Counter_Modo_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Counter_Modo_Sleep(void) 
{
    #if(!Counter_Modo_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Counter_Modo_CTRL_ENABLE == (Counter_Modo_CONTROL & Counter_Modo_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Counter_Modo_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Counter_Modo_backup.CounterEnableState = 0u;
        }
    #else
        Counter_Modo_backup.CounterEnableState = 1u;
        if(Counter_Modo_backup.CounterEnableState != 0u)
        {
            Counter_Modo_backup.CounterEnableState = 0u;
        }
    #endif /* (!Counter_Modo_ControlRegRemoved) */
    
    Counter_Modo_Stop();
    Counter_Modo_SaveConfig();
}


/*******************************************************************************
* Function Name: Counter_Modo_Wakeup
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
*  Counter_Modo_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Counter_Modo_Wakeup(void) 
{
    Counter_Modo_RestoreConfig();
    #if(!Counter_Modo_ControlRegRemoved)
        if(Counter_Modo_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Counter_Modo_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Counter_Modo_ControlRegRemoved) */
    
}


/* [] END OF FILE */
