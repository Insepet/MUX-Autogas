/*******************************************************************************
* File Name: IB1.c  
* Version 2.10
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "IB1.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 IB1__PORT == 15 && ((IB1__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: IB1_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None
*  
*******************************************************************************/
void IB1_Write(uint8 value) 
{
    uint8 staticBits = (IB1_DR & (uint8)(~IB1_MASK));
    IB1_DR = staticBits | ((uint8)(value << IB1_SHIFT) & IB1_MASK);
}


/*******************************************************************************
* Function Name: IB1_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  IB1_DM_STRONG     Strong Drive 
*  IB1_DM_OD_HI      Open Drain, Drives High 
*  IB1_DM_OD_LO      Open Drain, Drives Low 
*  IB1_DM_RES_UP     Resistive Pull Up 
*  IB1_DM_RES_DWN    Resistive Pull Down 
*  IB1_DM_RES_UPDWN  Resistive Pull Up/Down 
*  IB1_DM_DIG_HIZ    High Impedance Digital 
*  IB1_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void IB1_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(IB1_0, mode);
}


/*******************************************************************************
* Function Name: IB1_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro IB1_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 IB1_Read(void) 
{
    return (IB1_PS & IB1_MASK) >> IB1_SHIFT;
}


/*******************************************************************************
* Function Name: IB1_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 IB1_ReadDataReg(void) 
{
    return (IB1_DR & IB1_MASK) >> IB1_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(IB1_INTSTAT) 

    /*******************************************************************************
    * Function Name: IB1_ClearInterrupt
    ********************************************************************************
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 IB1_ClearInterrupt(void) 
    {
        return (IB1_INTSTAT & IB1_MASK) >> IB1_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
