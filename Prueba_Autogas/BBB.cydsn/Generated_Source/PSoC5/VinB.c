/*******************************************************************************
* File Name: VinB.c  
* Version 2.0
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
#include "VinB.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 VinB__PORT == 15 && ((VinB__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: VinB_Write
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
void VinB_Write(uint8 value) 
{
    uint8 staticBits = (VinB_DR & (uint8)(~VinB_MASK));
    VinB_DR = staticBits | ((uint8)(value << VinB_SHIFT) & VinB_MASK);
}


/*******************************************************************************
* Function Name: VinB_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  VinB_DM_STRONG     Strong Drive 
*  VinB_DM_OD_HI      Open Drain, Drives High 
*  VinB_DM_OD_LO      Open Drain, Drives Low 
*  VinB_DM_RES_UP     Resistive Pull Up 
*  VinB_DM_RES_DWN    Resistive Pull Down 
*  VinB_DM_RES_UPDWN  Resistive Pull Up/Down 
*  VinB_DM_DIG_HIZ    High Impedance Digital 
*  VinB_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void VinB_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(VinB_0, mode);
}


/*******************************************************************************
* Function Name: VinB_Read
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
*  Macro VinB_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 VinB_Read(void) 
{
    return (VinB_PS & VinB_MASK) >> VinB_SHIFT;
}


/*******************************************************************************
* Function Name: VinB_ReadDataReg
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
uint8 VinB_ReadDataReg(void) 
{
    return (VinB_DR & VinB_MASK) >> VinB_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(VinB_INTSTAT) 

    /*******************************************************************************
    * Function Name: VinB_ClearInterrupt
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
    uint8 VinB_ClearInterrupt(void) 
    {
        return (VinB_INTSTAT & VinB_MASK) >> VinB_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
