/*******************************************************************************
* File Name: VinB.h  
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

#if !defined(CY_PINS_VinB_H) /* Pins VinB_H */
#define CY_PINS_VinB_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "VinB_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 VinB__PORT == 15 && ((VinB__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    VinB_Write(uint8 value) ;
void    VinB_SetDriveMode(uint8 mode) ;
uint8   VinB_ReadDataReg(void) ;
uint8   VinB_Read(void) ;
uint8   VinB_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define VinB_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define VinB_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define VinB_DM_RES_UP          PIN_DM_RES_UP
#define VinB_DM_RES_DWN         PIN_DM_RES_DWN
#define VinB_DM_OD_LO           PIN_DM_OD_LO
#define VinB_DM_OD_HI           PIN_DM_OD_HI
#define VinB_DM_STRONG          PIN_DM_STRONG
#define VinB_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define VinB_MASK               VinB__MASK
#define VinB_SHIFT              VinB__SHIFT
#define VinB_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define VinB_PS                     (* (reg8 *) VinB__PS)
/* Data Register */
#define VinB_DR                     (* (reg8 *) VinB__DR)
/* Port Number */
#define VinB_PRT_NUM                (* (reg8 *) VinB__PRT) 
/* Connect to Analog Globals */                                                  
#define VinB_AG                     (* (reg8 *) VinB__AG)                       
/* Analog MUX bux enable */
#define VinB_AMUX                   (* (reg8 *) VinB__AMUX) 
/* Bidirectional Enable */                                                        
#define VinB_BIE                    (* (reg8 *) VinB__BIE)
/* Bit-mask for Aliased Register Access */
#define VinB_BIT_MASK               (* (reg8 *) VinB__BIT_MASK)
/* Bypass Enable */
#define VinB_BYP                    (* (reg8 *) VinB__BYP)
/* Port wide control signals */                                                   
#define VinB_CTL                    (* (reg8 *) VinB__CTL)
/* Drive Modes */
#define VinB_DM0                    (* (reg8 *) VinB__DM0) 
#define VinB_DM1                    (* (reg8 *) VinB__DM1)
#define VinB_DM2                    (* (reg8 *) VinB__DM2) 
/* Input Buffer Disable Override */
#define VinB_INP_DIS                (* (reg8 *) VinB__INP_DIS)
/* LCD Common or Segment Drive */
#define VinB_LCD_COM_SEG            (* (reg8 *) VinB__LCD_COM_SEG)
/* Enable Segment LCD */
#define VinB_LCD_EN                 (* (reg8 *) VinB__LCD_EN)
/* Slew Rate Control */
#define VinB_SLW                    (* (reg8 *) VinB__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define VinB_PRTDSI__CAPS_SEL       (* (reg8 *) VinB__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define VinB_PRTDSI__DBL_SYNC_IN    (* (reg8 *) VinB__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define VinB_PRTDSI__OE_SEL0        (* (reg8 *) VinB__PRTDSI__OE_SEL0) 
#define VinB_PRTDSI__OE_SEL1        (* (reg8 *) VinB__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define VinB_PRTDSI__OUT_SEL0       (* (reg8 *) VinB__PRTDSI__OUT_SEL0) 
#define VinB_PRTDSI__OUT_SEL1       (* (reg8 *) VinB__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define VinB_PRTDSI__SYNC_OUT       (* (reg8 *) VinB__PRTDSI__SYNC_OUT) 


#if defined(VinB__INTSTAT)  /* Interrupt Registers */

    #define VinB_INTSTAT                (* (reg8 *) VinB__INTSTAT)
    #define VinB_SNAP                   (* (reg8 *) VinB__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_VinB_H */


/* [] END OF FILE */
