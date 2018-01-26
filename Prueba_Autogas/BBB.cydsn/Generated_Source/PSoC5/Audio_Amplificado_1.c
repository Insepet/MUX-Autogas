/*******************************************************************************
* File Name: Audio_Amplificado_1.c  
* Version 1.70
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "cytypes.h"
#include "Audio_Amplificado_1.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Audio_Amplificado_1__PORT == 15 && (Audio_Amplificado_1__MASK & 0xC0))

/*******************************************************************************
* Function Name: Audio_Amplificado_1_Write
********************************************************************************
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  void 
*  
*******************************************************************************/
void Audio_Amplificado_1_Write(uint8 value) 
{
    uint8 staticBits = Audio_Amplificado_1_DR & ~Audio_Amplificado_1_MASK;
    Audio_Amplificado_1_DR = staticBits | ((value << Audio_Amplificado_1_SHIFT) & Audio_Amplificado_1_MASK);
}


/*******************************************************************************
* Function Name: Audio_Amplificado_1_SetDriveMode
********************************************************************************
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to this drive mode.
*
* Return: 
*  void
*
*******************************************************************************/
void Audio_Amplificado_1_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(Audio_Amplificado_1_0, mode);
}


/*******************************************************************************
* Function Name: Audio_Amplificado_1_Read
********************************************************************************
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  void 
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro Audio_Amplificado_1_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 Audio_Amplificado_1_Read(void) 
{
    return (Audio_Amplificado_1_PS & Audio_Amplificado_1_MASK) >> Audio_Amplificado_1_SHIFT;
}


/*******************************************************************************
* Function Name: Audio_Amplificado_1_ReadDataReg
********************************************************************************
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  void 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 Audio_Amplificado_1_ReadDataReg(void) 
{
    return (Audio_Amplificado_1_DR & Audio_Amplificado_1_MASK) >> Audio_Amplificado_1_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(Audio_Amplificado_1_INTSTAT) 

    /*******************************************************************************
    * Function Name: Audio_Amplificado_1_ClearInterrupt
    ********************************************************************************
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  void 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 Audio_Amplificado_1_ClearInterrupt(void) 
    {
        return (Audio_Amplificado_1_INTSTAT & Audio_Amplificado_1_MASK) >> Audio_Amplificado_1_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif
/* [] END OF FILE */ 
