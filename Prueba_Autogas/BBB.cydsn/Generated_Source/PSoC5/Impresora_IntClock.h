/*******************************************************************************
* File Name: Impresora_IntClock.h
* Version 2.20
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

#if !defined(CY_CLOCK_Impresora_IntClock_H)
#define CY_CLOCK_Impresora_IntClock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void Impresora_IntClock_Start(void) ;
void Impresora_IntClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Impresora_IntClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Impresora_IntClock_StandbyPower(uint8 state) ;
void Impresora_IntClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Impresora_IntClock_GetDividerRegister(void) ;
void Impresora_IntClock_SetModeRegister(uint8 modeBitMask) ;
void Impresora_IntClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 Impresora_IntClock_GetModeRegister(void) ;
void Impresora_IntClock_SetSourceRegister(uint8 clkSource) ;
uint8 Impresora_IntClock_GetSourceRegister(void) ;
#if defined(Impresora_IntClock__CFG3)
void Impresora_IntClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 Impresora_IntClock_GetPhaseRegister(void) ;
#endif /* defined(Impresora_IntClock__CFG3) */

#define Impresora_IntClock_Enable()                       Impresora_IntClock_Start()
#define Impresora_IntClock_Disable()                      Impresora_IntClock_Stop()
#define Impresora_IntClock_SetDivider(clkDivider)         Impresora_IntClock_SetDividerRegister(clkDivider, 1u)
#define Impresora_IntClock_SetDividerValue(clkDivider)    Impresora_IntClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define Impresora_IntClock_SetMode(clkMode)               Impresora_IntClock_SetModeRegister(clkMode)
#define Impresora_IntClock_SetSource(clkSource)           Impresora_IntClock_SetSourceRegister(clkSource)
#if defined(Impresora_IntClock__CFG3)
#define Impresora_IntClock_SetPhase(clkPhase)             Impresora_IntClock_SetPhaseRegister(clkPhase)
#define Impresora_IntClock_SetPhaseValue(clkPhase)        Impresora_IntClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(Impresora_IntClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Impresora_IntClock_CLKEN              (* (reg8 *) Impresora_IntClock__PM_ACT_CFG)
#define Impresora_IntClock_CLKEN_PTR          ((reg8 *) Impresora_IntClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Impresora_IntClock_CLKSTBY            (* (reg8 *) Impresora_IntClock__PM_STBY_CFG)
#define Impresora_IntClock_CLKSTBY_PTR        ((reg8 *) Impresora_IntClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Impresora_IntClock_DIV_LSB            (* (reg8 *) Impresora_IntClock__CFG0)
#define Impresora_IntClock_DIV_LSB_PTR        ((reg8 *) Impresora_IntClock__CFG0)
#define Impresora_IntClock_DIV_PTR            ((reg16 *) Impresora_IntClock__CFG0)

/* Clock MSB divider configuration register. */
#define Impresora_IntClock_DIV_MSB            (* (reg8 *) Impresora_IntClock__CFG1)
#define Impresora_IntClock_DIV_MSB_PTR        ((reg8 *) Impresora_IntClock__CFG1)

/* Mode and source configuration register */
#define Impresora_IntClock_MOD_SRC            (* (reg8 *) Impresora_IntClock__CFG2)
#define Impresora_IntClock_MOD_SRC_PTR        ((reg8 *) Impresora_IntClock__CFG2)

#if defined(Impresora_IntClock__CFG3)
/* Analog clock phase configuration register */
#define Impresora_IntClock_PHASE              (* (reg8 *) Impresora_IntClock__CFG3)
#define Impresora_IntClock_PHASE_PTR          ((reg8 *) Impresora_IntClock__CFG3)
#endif /* defined(Impresora_IntClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Impresora_IntClock_CLKEN_MASK         Impresora_IntClock__PM_ACT_MSK
#define Impresora_IntClock_CLKSTBY_MASK       Impresora_IntClock__PM_STBY_MSK

/* CFG2 field masks */
#define Impresora_IntClock_SRC_SEL_MSK        Impresora_IntClock__CFG2_SRC_SEL_MASK
#define Impresora_IntClock_MODE_MASK          (~(Impresora_IntClock_SRC_SEL_MSK))

#if defined(Impresora_IntClock__CFG3)
/* CFG3 phase mask */
#define Impresora_IntClock_PHASE_MASK         Impresora_IntClock__CFG3_PHASE_DLY_MASK
#endif /* defined(Impresora_IntClock__CFG3) */

#endif /* CY_CLOCK_Impresora_IntClock_H */


/* [] END OF FILE */
