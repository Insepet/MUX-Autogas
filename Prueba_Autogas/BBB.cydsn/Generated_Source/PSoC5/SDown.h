/*******************************************************************************
* File Name: SDown.h  
* Version 2.0
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_SDown_H) /* Pins SDown_H */
#define CY_PINS_SDown_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "SDown_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 SDown__PORT == 15 && ((SDown__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    SDown_Write(uint8 value) ;
void    SDown_SetDriveMode(uint8 mode) ;
uint8   SDown_ReadDataReg(void) ;
uint8   SDown_Read(void) ;
uint8   SDown_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define SDown_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define SDown_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define SDown_DM_RES_UP          PIN_DM_RES_UP
#define SDown_DM_RES_DWN         PIN_DM_RES_DWN
#define SDown_DM_OD_LO           PIN_DM_OD_LO
#define SDown_DM_OD_HI           PIN_DM_OD_HI
#define SDown_DM_STRONG          PIN_DM_STRONG
#define SDown_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define SDown_MASK               SDown__MASK
#define SDown_SHIFT              SDown__SHIFT
#define SDown_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SDown_PS                     (* (reg8 *) SDown__PS)
/* Data Register */
#define SDown_DR                     (* (reg8 *) SDown__DR)
/* Port Number */
#define SDown_PRT_NUM                (* (reg8 *) SDown__PRT) 
/* Connect to Analog Globals */                                                  
#define SDown_AG                     (* (reg8 *) SDown__AG)                       
/* Analog MUX bux enable */
#define SDown_AMUX                   (* (reg8 *) SDown__AMUX) 
/* Bidirectional Enable */                                                        
#define SDown_BIE                    (* (reg8 *) SDown__BIE)
/* Bit-mask for Aliased Register Access */
#define SDown_BIT_MASK               (* (reg8 *) SDown__BIT_MASK)
/* Bypass Enable */
#define SDown_BYP                    (* (reg8 *) SDown__BYP)
/* Port wide control signals */                                                   
#define SDown_CTL                    (* (reg8 *) SDown__CTL)
/* Drive Modes */
#define SDown_DM0                    (* (reg8 *) SDown__DM0) 
#define SDown_DM1                    (* (reg8 *) SDown__DM1)
#define SDown_DM2                    (* (reg8 *) SDown__DM2) 
/* Input Buffer Disable Override */
#define SDown_INP_DIS                (* (reg8 *) SDown__INP_DIS)
/* LCD Common or Segment Drive */
#define SDown_LCD_COM_SEG            (* (reg8 *) SDown__LCD_COM_SEG)
/* Enable Segment LCD */
#define SDown_LCD_EN                 (* (reg8 *) SDown__LCD_EN)
/* Slew Rate Control */
#define SDown_SLW                    (* (reg8 *) SDown__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define SDown_PRTDSI__CAPS_SEL       (* (reg8 *) SDown__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define SDown_PRTDSI__DBL_SYNC_IN    (* (reg8 *) SDown__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define SDown_PRTDSI__OE_SEL0        (* (reg8 *) SDown__PRTDSI__OE_SEL0) 
#define SDown_PRTDSI__OE_SEL1        (* (reg8 *) SDown__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define SDown_PRTDSI__OUT_SEL0       (* (reg8 *) SDown__PRTDSI__OUT_SEL0) 
#define SDown_PRTDSI__OUT_SEL1       (* (reg8 *) SDown__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define SDown_PRTDSI__SYNC_OUT       (* (reg8 *) SDown__PRTDSI__SYNC_OUT) 


#if defined(SDown__INTSTAT)  /* Interrupt Registers */

    #define SDown_INTSTAT                (* (reg8 *) SDown__INTSTAT)
    #define SDown_SNAP                   (* (reg8 *) SDown__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_SDown_H */


/* [] END OF FILE */
