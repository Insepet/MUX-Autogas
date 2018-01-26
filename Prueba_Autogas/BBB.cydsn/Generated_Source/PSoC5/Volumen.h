/*******************************************************************************
* File Name: Volumen.h  
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

#if !defined(CY_PINS_Volumen_H) /* Pins Volumen_H */
#define CY_PINS_Volumen_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Volumen_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Volumen__PORT == 15 && ((Volumen__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    Volumen_Write(uint8 value) ;
void    Volumen_SetDriveMode(uint8 mode) ;
uint8   Volumen_ReadDataReg(void) ;
uint8   Volumen_Read(void) ;
uint8   Volumen_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define Volumen_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define Volumen_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define Volumen_DM_RES_UP          PIN_DM_RES_UP
#define Volumen_DM_RES_DWN         PIN_DM_RES_DWN
#define Volumen_DM_OD_LO           PIN_DM_OD_LO
#define Volumen_DM_OD_HI           PIN_DM_OD_HI
#define Volumen_DM_STRONG          PIN_DM_STRONG
#define Volumen_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define Volumen_MASK               Volumen__MASK
#define Volumen_SHIFT              Volumen__SHIFT
#define Volumen_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Volumen_PS                     (* (reg8 *) Volumen__PS)
/* Data Register */
#define Volumen_DR                     (* (reg8 *) Volumen__DR)
/* Port Number */
#define Volumen_PRT_NUM                (* (reg8 *) Volumen__PRT) 
/* Connect to Analog Globals */                                                  
#define Volumen_AG                     (* (reg8 *) Volumen__AG)                       
/* Analog MUX bux enable */
#define Volumen_AMUX                   (* (reg8 *) Volumen__AMUX) 
/* Bidirectional Enable */                                                        
#define Volumen_BIE                    (* (reg8 *) Volumen__BIE)
/* Bit-mask for Aliased Register Access */
#define Volumen_BIT_MASK               (* (reg8 *) Volumen__BIT_MASK)
/* Bypass Enable */
#define Volumen_BYP                    (* (reg8 *) Volumen__BYP)
/* Port wide control signals */                                                   
#define Volumen_CTL                    (* (reg8 *) Volumen__CTL)
/* Drive Modes */
#define Volumen_DM0                    (* (reg8 *) Volumen__DM0) 
#define Volumen_DM1                    (* (reg8 *) Volumen__DM1)
#define Volumen_DM2                    (* (reg8 *) Volumen__DM2) 
/* Input Buffer Disable Override */
#define Volumen_INP_DIS                (* (reg8 *) Volumen__INP_DIS)
/* LCD Common or Segment Drive */
#define Volumen_LCD_COM_SEG            (* (reg8 *) Volumen__LCD_COM_SEG)
/* Enable Segment LCD */
#define Volumen_LCD_EN                 (* (reg8 *) Volumen__LCD_EN)
/* Slew Rate Control */
#define Volumen_SLW                    (* (reg8 *) Volumen__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Volumen_PRTDSI__CAPS_SEL       (* (reg8 *) Volumen__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Volumen_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Volumen__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Volumen_PRTDSI__OE_SEL0        (* (reg8 *) Volumen__PRTDSI__OE_SEL0) 
#define Volumen_PRTDSI__OE_SEL1        (* (reg8 *) Volumen__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Volumen_PRTDSI__OUT_SEL0       (* (reg8 *) Volumen__PRTDSI__OUT_SEL0) 
#define Volumen_PRTDSI__OUT_SEL1       (* (reg8 *) Volumen__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Volumen_PRTDSI__SYNC_OUT       (* (reg8 *) Volumen__PRTDSI__SYNC_OUT) 


#if defined(Volumen__INTSTAT)  /* Interrupt Registers */

    #define Volumen_INTSTAT                (* (reg8 *) Volumen__INTSTAT)
    #define Volumen_SNAP                   (* (reg8 *) Volumen__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Volumen_H */


/* [] END OF FILE */
