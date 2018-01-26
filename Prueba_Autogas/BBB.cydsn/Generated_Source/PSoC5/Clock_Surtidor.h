/*******************************************************************************
* File Name: Clock_Surtidor.h
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

#if !defined(CY_CLOCK_Clock_Surtidor_H)
#define CY_CLOCK_Clock_Surtidor_H

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

void Clock_Surtidor_Start(void) ;
void Clock_Surtidor_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Clock_Surtidor_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Clock_Surtidor_StandbyPower(uint8 state) ;
void Clock_Surtidor_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Clock_Surtidor_GetDividerRegister(void) ;
void Clock_Surtidor_SetModeRegister(uint8 modeBitMask) ;
void Clock_Surtidor_ClearModeRegister(uint8 modeBitMask) ;
uint8 Clock_Surtidor_GetModeRegister(void) ;
void Clock_Surtidor_SetSourceRegister(uint8 clkSource) ;
uint8 Clock_Surtidor_GetSourceRegister(void) ;
#if defined(Clock_Surtidor__CFG3)
void Clock_Surtidor_SetPhaseRegister(uint8 clkPhase) ;
uint8 Clock_Surtidor_GetPhaseRegister(void) ;
#endif /* defined(Clock_Surtidor__CFG3) */

#define Clock_Surtidor_Enable()                       Clock_Surtidor_Start()
#define Clock_Surtidor_Disable()                      Clock_Surtidor_Stop()
#define Clock_Surtidor_SetDivider(clkDivider)         Clock_Surtidor_SetDividerRegister(clkDivider, 1)
#define Clock_Surtidor_SetDividerValue(clkDivider)    Clock_Surtidor_SetDividerRegister((clkDivider) - 1, 1)
#define Clock_Surtidor_SetMode(clkMode)               Clock_Surtidor_SetModeRegister(clkMode)
#define Clock_Surtidor_SetSource(clkSource)           Clock_Surtidor_SetSourceRegister(clkSource)
#if defined(Clock_Surtidor__CFG3)
#define Clock_Surtidor_SetPhase(clkPhase)             Clock_Surtidor_SetPhaseRegister(clkPhase)
#define Clock_Surtidor_SetPhaseValue(clkPhase)        Clock_Surtidor_SetPhaseRegister((clkPhase) + 1)
#endif /* defined(Clock_Surtidor__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Clock_Surtidor_CLKEN              (* (reg8 *) Clock_Surtidor__PM_ACT_CFG)
#define Clock_Surtidor_CLKEN_PTR          ((reg8 *) Clock_Surtidor__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Clock_Surtidor_CLKSTBY            (* (reg8 *) Clock_Surtidor__PM_STBY_CFG)
#define Clock_Surtidor_CLKSTBY_PTR        ((reg8 *) Clock_Surtidor__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Clock_Surtidor_DIV_LSB            (* (reg8 *) Clock_Surtidor__CFG0)
#define Clock_Surtidor_DIV_LSB_PTR        ((reg8 *) Clock_Surtidor__CFG0)
#define Clock_Surtidor_DIV_PTR            ((reg16 *) Clock_Surtidor__CFG0)

/* Clock MSB divider configuration register. */
#define Clock_Surtidor_DIV_MSB            (* (reg8 *) Clock_Surtidor__CFG1)
#define Clock_Surtidor_DIV_MSB_PTR        ((reg8 *) Clock_Surtidor__CFG1)

/* Mode and source configuration register */
#define Clock_Surtidor_MOD_SRC            (* (reg8 *) Clock_Surtidor__CFG2)
#define Clock_Surtidor_MOD_SRC_PTR        ((reg8 *) Clock_Surtidor__CFG2)

#if defined(Clock_Surtidor__CFG3)
/* Analog clock phase configuration register */
#define Clock_Surtidor_PHASE              (* (reg8 *) Clock_Surtidor__CFG3)
#define Clock_Surtidor_PHASE_PTR          ((reg8 *) Clock_Surtidor__CFG3)
#endif /* defined(Clock_Surtidor__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Clock_Surtidor_CLKEN_MASK         Clock_Surtidor__PM_ACT_MSK
#define Clock_Surtidor_CLKSTBY_MASK       Clock_Surtidor__PM_STBY_MSK

/* CFG2 field masks */
#define Clock_Surtidor_SRC_SEL_MSK        Clock_Surtidor__CFG2_SRC_SEL_MASK
#define Clock_Surtidor_MODE_MASK          (~(Clock_Surtidor_SRC_SEL_MSK))

#if defined(Clock_Surtidor__CFG3)
/* CFG3 phase mask */
#define Clock_Surtidor_PHASE_MASK         Clock_Surtidor__CFG3_PHASE_DLY_MASK
#endif /* defined(Clock_Surtidor__CFG3) */

#endif /* CY_CLOCK_Clock_Surtidor_H */


/* [] END OF FILE */
