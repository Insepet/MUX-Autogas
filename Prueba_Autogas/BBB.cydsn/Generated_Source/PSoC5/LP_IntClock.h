/*******************************************************************************
* File Name: LP_IntClock.h
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

#if !defined(CY_CLOCK_LP_IntClock_H)
#define CY_CLOCK_LP_IntClock_H

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

void LP_IntClock_Start(void) ;
void LP_IntClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void LP_IntClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void LP_IntClock_StandbyPower(uint8 state) ;
void LP_IntClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 LP_IntClock_GetDividerRegister(void) ;
void LP_IntClock_SetModeRegister(uint8 modeBitMask) ;
void LP_IntClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 LP_IntClock_GetModeRegister(void) ;
void LP_IntClock_SetSourceRegister(uint8 clkSource) ;
uint8 LP_IntClock_GetSourceRegister(void) ;
#if defined(LP_IntClock__CFG3)
void LP_IntClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 LP_IntClock_GetPhaseRegister(void) ;
#endif /* defined(LP_IntClock__CFG3) */

#define LP_IntClock_Enable()                       LP_IntClock_Start()
#define LP_IntClock_Disable()                      LP_IntClock_Stop()
#define LP_IntClock_SetDivider(clkDivider)         LP_IntClock_SetDividerRegister(clkDivider, 1u)
#define LP_IntClock_SetDividerValue(clkDivider)    LP_IntClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define LP_IntClock_SetMode(clkMode)               LP_IntClock_SetModeRegister(clkMode)
#define LP_IntClock_SetSource(clkSource)           LP_IntClock_SetSourceRegister(clkSource)
#if defined(LP_IntClock__CFG3)
#define LP_IntClock_SetPhase(clkPhase)             LP_IntClock_SetPhaseRegister(clkPhase)
#define LP_IntClock_SetPhaseValue(clkPhase)        LP_IntClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(LP_IntClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define LP_IntClock_CLKEN              (* (reg8 *) LP_IntClock__PM_ACT_CFG)
#define LP_IntClock_CLKEN_PTR          ((reg8 *) LP_IntClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define LP_IntClock_CLKSTBY            (* (reg8 *) LP_IntClock__PM_STBY_CFG)
#define LP_IntClock_CLKSTBY_PTR        ((reg8 *) LP_IntClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define LP_IntClock_DIV_LSB            (* (reg8 *) LP_IntClock__CFG0)
#define LP_IntClock_DIV_LSB_PTR        ((reg8 *) LP_IntClock__CFG0)
#define LP_IntClock_DIV_PTR            ((reg16 *) LP_IntClock__CFG0)

/* Clock MSB divider configuration register. */
#define LP_IntClock_DIV_MSB            (* (reg8 *) LP_IntClock__CFG1)
#define LP_IntClock_DIV_MSB_PTR        ((reg8 *) LP_IntClock__CFG1)

/* Mode and source configuration register */
#define LP_IntClock_MOD_SRC            (* (reg8 *) LP_IntClock__CFG2)
#define LP_IntClock_MOD_SRC_PTR        ((reg8 *) LP_IntClock__CFG2)

#if defined(LP_IntClock__CFG3)
/* Analog clock phase configuration register */
#define LP_IntClock_PHASE              (* (reg8 *) LP_IntClock__CFG3)
#define LP_IntClock_PHASE_PTR          ((reg8 *) LP_IntClock__CFG3)
#endif /* defined(LP_IntClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define LP_IntClock_CLKEN_MASK         LP_IntClock__PM_ACT_MSK
#define LP_IntClock_CLKSTBY_MASK       LP_IntClock__PM_STBY_MSK

/* CFG2 field masks */
#define LP_IntClock_SRC_SEL_MSK        LP_IntClock__CFG2_SRC_SEL_MASK
#define LP_IntClock_MODE_MASK          (~(LP_IntClock_SRC_SEL_MSK))

#if defined(LP_IntClock__CFG3)
/* CFG3 phase mask */
#define LP_IntClock_PHASE_MASK         LP_IntClock__CFG3_PHASE_DLY_MASK
#endif /* defined(LP_IntClock__CFG3) */

#endif /* CY_CLOCK_LP_IntClock_H */


/* [] END OF FILE */
