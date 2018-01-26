/*******************************************************************************
* File Name: SDown.c  
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
#include "SDown.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 SDown__PORT == 15 && ((SDown__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: SDown_Write
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
void SDown_Write(uint8 value) 
{
    uint8 staticBits = (SDown_DR & (uint8)(~SDown_MASK));
    SDown_DR = staticBits | ((uint8)(value << SDown_SHIFT) & SDown_MASK);
}


/*******************************************************************************
* Function Name: SDown_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  SDown_DM_STRONG     Strong Drive 
*  SDown_DM_OD_HI      Open Drain, Drives High 
*  SDown_DM_OD_LO      Open Drain, Drives Low 
*  SDown_DM_RES_UP     Resistive Pull Up 
*  SDown_DM_RES_DWN    Resistive Pull Down 
*  SDown_DM_RES_UPDWN  Resistive Pull Up/Down 
*  SDown_DM_DIG_HIZ    High Impedance Digital 
*  SDown_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void SDown_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(SDown_0, mode);
}


/*******************************************************************************
* Function Name: SDown_Read
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
*  Macro SDown_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 SDown_Read(void) 
{
    return (SDown_PS & SDown_MASK) >> SDown_SHIFT;
}


/*******************************************************************************
* Function Name: SDown_ReadDataReg
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
uint8 SDown_ReadDataReg(void) 
{
    return (SDown_DR & SDown_MASK) >> SDown_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(SDown_INTSTAT) 

    /*******************************************************************************
    * Function Name: SDown_ClearInterrupt
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
    uint8 SDown_ClearInterrupt(void) 
    {
        return (SDown_INTSTAT & SDown_MASK) >> SDown_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
