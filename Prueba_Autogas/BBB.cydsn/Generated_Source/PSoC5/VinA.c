/*******************************************************************************
* File Name: VinA.c  
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
#include "VinA.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 VinA__PORT == 15 && ((VinA__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: VinA_Write
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
void VinA_Write(uint8 value) 
{
    uint8 staticBits = (VinA_DR & (uint8)(~VinA_MASK));
    VinA_DR = staticBits | ((uint8)(value << VinA_SHIFT) & VinA_MASK);
}


/*******************************************************************************
* Function Name: VinA_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  VinA_DM_STRONG     Strong Drive 
*  VinA_DM_OD_HI      Open Drain, Drives High 
*  VinA_DM_OD_LO      Open Drain, Drives Low 
*  VinA_DM_RES_UP     Resistive Pull Up 
*  VinA_DM_RES_DWN    Resistive Pull Down 
*  VinA_DM_RES_UPDWN  Resistive Pull Up/Down 
*  VinA_DM_DIG_HIZ    High Impedance Digital 
*  VinA_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void VinA_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(VinA_0, mode);
}


/*******************************************************************************
* Function Name: VinA_Read
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
*  Macro VinA_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 VinA_Read(void) 
{
    return (VinA_PS & VinA_MASK) >> VinA_SHIFT;
}


/*******************************************************************************
* Function Name: VinA_ReadDataReg
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
uint8 VinA_ReadDataReg(void) 
{
    return (VinA_DR & VinA_MASK) >> VinA_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(VinA_INTSTAT) 

    /*******************************************************************************
    * Function Name: VinA_ClearInterrupt
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
    uint8 VinA_ClearInterrupt(void) 
    {
        return (VinA_INTSTAT & VinA_MASK) >> VinA_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
