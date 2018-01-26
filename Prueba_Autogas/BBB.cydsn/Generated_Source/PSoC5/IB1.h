/*******************************************************************************
* File Name: IB1.h  
* Version 2.10
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

#if !defined(CY_PINS_IB1_H) /* Pins IB1_H */
#define CY_PINS_IB1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "IB1_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_10 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 IB1__PORT == 15 && ((IB1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    IB1_Write(uint8 value) ;
void    IB1_SetDriveMode(uint8 mode) ;
uint8   IB1_ReadDataReg(void) ;
uint8   IB1_Read(void) ;
uint8   IB1_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define IB1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define IB1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define IB1_DM_RES_UP          PIN_DM_RES_UP
#define IB1_DM_RES_DWN         PIN_DM_RES_DWN
#define IB1_DM_OD_LO           PIN_DM_OD_LO
#define IB1_DM_OD_HI           PIN_DM_OD_HI
#define IB1_DM_STRONG          PIN_DM_STRONG
#define IB1_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define IB1_MASK               IB1__MASK
#define IB1_SHIFT              IB1__SHIFT
#define IB1_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define IB1_PS                     (* (reg8 *) IB1__PS)
/* Data Register */
#define IB1_DR                     (* (reg8 *) IB1__DR)
/* Port Number */
#define IB1_PRT_NUM                (* (reg8 *) IB1__PRT) 
/* Connect to Analog Globals */                                                  
#define IB1_AG                     (* (reg8 *) IB1__AG)                       
/* Analog MUX bux enable */
#define IB1_AMUX                   (* (reg8 *) IB1__AMUX) 
/* Bidirectional Enable */                                                        
#define IB1_BIE                    (* (reg8 *) IB1__BIE)
/* Bit-mask for Aliased Register Access */
#define IB1_BIT_MASK               (* (reg8 *) IB1__BIT_MASK)
/* Bypass Enable */
#define IB1_BYP                    (* (reg8 *) IB1__BYP)
/* Port wide control signals */                                                   
#define IB1_CTL                    (* (reg8 *) IB1__CTL)
/* Drive Modes */
#define IB1_DM0                    (* (reg8 *) IB1__DM0) 
#define IB1_DM1                    (* (reg8 *) IB1__DM1)
#define IB1_DM2                    (* (reg8 *) IB1__DM2) 
/* Input Buffer Disable Override */
#define IB1_INP_DIS                (* (reg8 *) IB1__INP_DIS)
/* LCD Common or Segment Drive */
#define IB1_LCD_COM_SEG            (* (reg8 *) IB1__LCD_COM_SEG)
/* Enable Segment LCD */
#define IB1_LCD_EN                 (* (reg8 *) IB1__LCD_EN)
/* Slew Rate Control */
#define IB1_SLW                    (* (reg8 *) IB1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define IB1_PRTDSI__CAPS_SEL       (* (reg8 *) IB1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define IB1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) IB1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define IB1_PRTDSI__OE_SEL0        (* (reg8 *) IB1__PRTDSI__OE_SEL0) 
#define IB1_PRTDSI__OE_SEL1        (* (reg8 *) IB1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define IB1_PRTDSI__OUT_SEL0       (* (reg8 *) IB1__PRTDSI__OUT_SEL0) 
#define IB1_PRTDSI__OUT_SEL1       (* (reg8 *) IB1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define IB1_PRTDSI__SYNC_OUT       (* (reg8 *) IB1__PRTDSI__SYNC_OUT) 


#if defined(IB1__INTSTAT)  /* Interrupt Registers */

    #define IB1_INTSTAT                (* (reg8 *) IB1__INTSTAT)
    #define IB1_SNAP                   (* (reg8 *) IB1__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_IB1_H */


/* [] END OF FILE */
