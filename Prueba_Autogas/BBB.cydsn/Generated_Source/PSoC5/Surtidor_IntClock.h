/*******************************************************************************
* File Name: Surtidor_IntClock.h
* Version 2.0
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_Surtidor_IntClock_H)
#define CY_CLOCK_Surtidor_IntClock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void Surtidor_IntClock_Start(void) ;
void Surtidor_IntClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Surtidor_IntClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Surtidor_IntClock_StandbyPower(uint8 state) ;
void Surtidor_IntClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Surtidor_IntClock_GetDividerRegister(void) ;
void Surtidor_IntClock_SetModeRegister(uint8 modeBitMask) ;
void Surtidor_IntClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 Surtidor_IntClock_GetModeRegister(void) ;
void Surtidor_IntClock_SetSourceRegister(uint8 clkSource) ;
uint8 Surtidor_IntClock_GetSourceRegister(void) ;
#if defined(Surtidor_IntClock__CFG3)
void Surtidor_IntClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 Surtidor_IntClock_GetPhaseRegister(void) ;
#endif /* defined(Surtidor_IntClock__CFG3) */

#define Surtidor_IntClock_Enable()                       Surtidor_IntClock_Start()
#define Surtidor_IntClock_Disable()                      Surtidor_IntClock_Stop()
#define Surtidor_IntClock_SetDivider(clkDivider)         Surtidor_IntClock_SetDividerRegister(clkDivider, 1)
#define Surtidor_IntClock_SetDividerValue(clkDivider)    Surtidor_IntClock_SetDividerRegister((clkDivider) - 1, 1)
#define Surtidor_IntClock_SetMode(clkMode)               Surtidor_IntClock_SetModeRegister(clkMode)
#define Surtidor_IntClock_SetSource(clkSource)           Surtidor_IntClock_SetSourceRegister(clkSource)
#if defined(Surtidor_IntClock__CFG3)
#define Surtidor_IntClock_SetPhase(clkPhase)             Surtidor_IntClock_SetPhaseRegister(clkPhase)
#define Surtidor_IntClock_SetPhaseValue(clkPhase)        Surtidor_IntClock_SetPhaseRegister((clkPhase) + 1)
#endif /* defined(Surtidor_IntClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Surtidor_IntClock_CLKEN              (* (reg8 *) Surtidor_IntClock__PM_ACT_CFG)
#define Surtidor_IntClock_CLKEN_PTR          ((reg8 *) Surtidor_IntClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Surtidor_IntClock_CLKSTBY            (* (reg8 *) Surtidor_IntClock__PM_STBY_CFG)
#define Surtidor_IntClock_CLKSTBY_PTR        ((reg8 *) Surtidor_IntClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Surtidor_IntClock_DIV_LSB            (* (reg8 *) Surtidor_IntClock__CFG0)
#define Surtidor_IntClock_DIV_LSB_PTR        ((reg8 *) Surtidor_IntClock__CFG0)
#define Surtidor_IntClock_DIV_PTR            ((reg16 *) Surtidor_IntClock__CFG0)

/* Clock MSB divider configuration register. */
#define Surtidor_IntClock_DIV_MSB            (* (reg8 *) Surtidor_IntClock__CFG1)
#define Surtidor_IntClock_DIV_MSB_PTR        ((reg8 *) Surtidor_IntClock__CFG1)

/* Mode and source configuration register */
#define Surtidor_IntClock_MOD_SRC            (* (reg8 *) Surtidor_IntClock__CFG2)
#define Surtidor_IntClock_MOD_SRC_PTR        ((reg8 *) Surtidor_IntClock__CFG2)

#if defined(Surtidor_IntClock__CFG3)
/* Analog clock phase configuration register */
#define Surtidor_IntClock_PHASE              (* (reg8 *) Surtidor_IntClock__CFG3)
#define Surtidor_IntClock_PHASE_PTR          ((reg8 *) Surtidor_IntClock__CFG3)
#endif /* defined(Surtidor_IntClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Surtidor_IntClock_CLKEN_MASK         Surtidor_IntClock__PM_ACT_MSK
#define Surtidor_IntClock_CLKSTBY_MASK       Surtidor_IntClock__PM_STBY_MSK

/* CFG2 field masks */
#define Surtidor_IntClock_SRC_SEL_MSK        Surtidor_IntClock__CFG2_SRC_SEL_MASK
#define Surtidor_IntClock_MODE_MASK          (~(Surtidor_IntClock_SRC_SEL_MSK))

#if defined(Surtidor_IntClock__CFG3)
/* CFG3 phase mask */
#define Surtidor_IntClock_PHASE_MASK         Surtidor_IntClock__CFG3_PHASE_DLY_MASK
#endif /* defined(Surtidor_IntClock__CFG3) */

#endif /* CY_CLOCK_Surtidor_IntClock_H */


/* [] END OF FILE */
