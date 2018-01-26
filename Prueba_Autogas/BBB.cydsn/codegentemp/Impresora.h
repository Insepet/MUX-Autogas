/*******************************************************************************
* File Name: Impresora.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_Impresora_H)
#define CY_UART_Impresora_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Impresora_RX_ENABLED                     (1u)
#define Impresora_TX_ENABLED                     (1u)
#define Impresora_HD_ENABLED                     (0u)
#define Impresora_RX_INTERRUPT_ENABLED           (1u)
#define Impresora_TX_INTERRUPT_ENABLED           (0u)
#define Impresora_INTERNAL_CLOCK_USED            (1u)
#define Impresora_RXHW_ADDRESS_ENABLED           (0u)
#define Impresora_OVER_SAMPLE_COUNT              (8u)
#define Impresora_PARITY_TYPE                    (1u)
#define Impresora_PARITY_TYPE_SW                 (0u)
#define Impresora_BREAK_DETECT                   (0u)
#define Impresora_BREAK_BITS_TX                  (13u)
#define Impresora_BREAK_BITS_RX                  (13u)
#define Impresora_TXCLKGEN_DP                    (1u)
#define Impresora_USE23POLLING                   (1u)
#define Impresora_FLOW_CONTROL                   (0u)
#define Impresora_CLK_FREQ                       (0u)
#define Impresora_TX_BUFFER_SIZE                 (4u)
#define Impresora_RX_BUFFER_SIZE                 (200u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(Impresora_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define Impresora_CONTROL_REG_REMOVED            (0u)
#else
    #define Impresora_CONTROL_REG_REMOVED            (1u)
#endif /* End Impresora_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Impresora_backupStruct_
{
    uint8 enableState;

    #if(Impresora_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Impresora_CONTROL_REG_REMOVED */

} Impresora_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Impresora_Start(void) ;
void Impresora_Stop(void) ;
uint8 Impresora_ReadControlRegister(void) ;
void Impresora_WriteControlRegister(uint8 control) ;

void Impresora_Init(void) ;
void Impresora_Enable(void) ;
void Impresora_SaveConfig(void) ;
void Impresora_RestoreConfig(void) ;
void Impresora_Sleep(void) ;
void Impresora_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) )

    #if (Impresora_RX_INTERRUPT_ENABLED)
        #define Impresora_EnableRxInt()  CyIntEnable (Impresora_RX_VECT_NUM)
        #define Impresora_DisableRxInt() CyIntDisable(Impresora_RX_VECT_NUM)
        CY_ISR_PROTO(Impresora_RXISR);
    #endif /* Impresora_RX_INTERRUPT_ENABLED */

    void Impresora_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Impresora_SetRxAddress1(uint8 address) ;
    void Impresora_SetRxAddress2(uint8 address) ;

    void  Impresora_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Impresora_ReadRxData(void) ;
    uint8 Impresora_ReadRxStatus(void) ;
    uint8 Impresora_GetChar(void) ;
    uint16 Impresora_GetByte(void) ;
    uint8 Impresora_GetRxBufferSize(void)
                                                            ;
    void Impresora_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Impresora_GetRxInterruptSource   Impresora_ReadRxStatus

#endif /* End (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) */

/* Only if TX is enabled */
#if(Impresora_TX_ENABLED || Impresora_HD_ENABLED)

    #if(Impresora_TX_INTERRUPT_ENABLED)
        #define Impresora_EnableTxInt()  CyIntEnable (Impresora_TX_VECT_NUM)
        #define Impresora_DisableTxInt() CyIntDisable(Impresora_TX_VECT_NUM)
        #define Impresora_SetPendingTxInt() CyIntSetPending(Impresora_TX_VECT_NUM)
        #define Impresora_ClearPendingTxInt() CyIntClearPending(Impresora_TX_VECT_NUM)
        CY_ISR_PROTO(Impresora_TXISR);
    #endif /* Impresora_TX_INTERRUPT_ENABLED */

    void Impresora_SetTxInterruptMode(uint8 intSrc) ;
    void Impresora_WriteTxData(uint8 txDataByte) ;
    uint8 Impresora_ReadTxStatus(void) ;
    void Impresora_PutChar(uint8 txDataByte) ;
    void Impresora_PutString(const char8 string[]) ;
    void Impresora_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Impresora_PutCRLF(uint8 txDataByte) ;
    void Impresora_ClearTxBuffer(void) ;
    void Impresora_SetTxAddressMode(uint8 addressMode) ;
    void Impresora_SendBreak(uint8 retMode) ;
    uint8 Impresora_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Impresora_PutStringConst         Impresora_PutString
    #define Impresora_PutArrayConst          Impresora_PutArray
    #define Impresora_GetTxInterruptSource   Impresora_ReadTxStatus

#endif /* End Impresora_TX_ENABLED || Impresora_HD_ENABLED */

#if(Impresora_HD_ENABLED)
    void Impresora_LoadRxConfig(void) ;
    void Impresora_LoadTxConfig(void) ;
#endif /* End Impresora_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Impresora) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Impresora_CyBtldrCommStart(void) CYSMALL ;
    void    Impresora_CyBtldrCommStop(void) CYSMALL ;
    void    Impresora_CyBtldrCommReset(void) CYSMALL ;
    cystatus Impresora_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Impresora_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Impresora)
        #define CyBtldrCommStart    Impresora_CyBtldrCommStart
        #define CyBtldrCommStop     Impresora_CyBtldrCommStop
        #define CyBtldrCommReset    Impresora_CyBtldrCommReset
        #define CyBtldrCommWrite    Impresora_CyBtldrCommWrite
        #define CyBtldrCommRead     Impresora_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Impresora) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Impresora_BYTE2BYTE_TIME_OUT (25u)
    #define Impresora_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define Impresora_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define Impresora_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Impresora_SET_SPACE      (0x00u)
#define Impresora_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (Impresora_TX_ENABLED) || (Impresora_HD_ENABLED) )
    #if(Impresora_TX_INTERRUPT_ENABLED)
        #define Impresora_TX_VECT_NUM            (uint8)Impresora_TXInternalInterrupt__INTC_NUMBER
        #define Impresora_TX_PRIOR_NUM           (uint8)Impresora_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Impresora_TX_INTERRUPT_ENABLED */

    #define Impresora_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define Impresora_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define Impresora_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(Impresora_TX_ENABLED)
        #define Impresora_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (Impresora_HD_ENABLED) */
        #define Impresora_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (Impresora_TX_ENABLED) */

    #define Impresora_TX_STS_COMPLETE            (uint8)(0x01u << Impresora_TX_STS_COMPLETE_SHIFT)
    #define Impresora_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Impresora_TX_STS_FIFO_EMPTY_SHIFT)
    #define Impresora_TX_STS_FIFO_FULL           (uint8)(0x01u << Impresora_TX_STS_FIFO_FULL_SHIFT)
    #define Impresora_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Impresora_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Impresora_TX_ENABLED) || (Impresora_HD_ENABLED)*/

#if( (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) )
    #if(Impresora_RX_INTERRUPT_ENABLED)
        #define Impresora_RX_VECT_NUM            (uint8)Impresora_RXInternalInterrupt__INTC_NUMBER
        #define Impresora_RX_PRIOR_NUM           (uint8)Impresora_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Impresora_RX_INTERRUPT_ENABLED */
    #define Impresora_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Impresora_RX_STS_BREAK_SHIFT             (0x01u)
    #define Impresora_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Impresora_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Impresora_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Impresora_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Impresora_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Impresora_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Impresora_RX_STS_MRKSPC           (uint8)(0x01u << Impresora_RX_STS_MRKSPC_SHIFT)
    #define Impresora_RX_STS_BREAK            (uint8)(0x01u << Impresora_RX_STS_BREAK_SHIFT)
    #define Impresora_RX_STS_PAR_ERROR        (uint8)(0x01u << Impresora_RX_STS_PAR_ERROR_SHIFT)
    #define Impresora_RX_STS_STOP_ERROR       (uint8)(0x01u << Impresora_RX_STS_STOP_ERROR_SHIFT)
    #define Impresora_RX_STS_OVERRUN          (uint8)(0x01u << Impresora_RX_STS_OVERRUN_SHIFT)
    #define Impresora_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Impresora_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Impresora_RX_STS_ADDR_MATCH       (uint8)(0x01u << Impresora_RX_STS_ADDR_MATCH_SHIFT)
    #define Impresora_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Impresora_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Impresora_RX_HW_MASK                     (0x7Fu)
#endif /* End (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) */

/* Control Register definitions */
#define Impresora_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Impresora_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Impresora_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Impresora_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Impresora_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Impresora_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Impresora_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Impresora_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Impresora_CTRL_HD_SEND               (uint8)(0x01u << Impresora_CTRL_HD_SEND_SHIFT)
#define Impresora_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Impresora_CTRL_HD_SEND_BREAK_SHIFT)
#define Impresora_CTRL_MARK                  (uint8)(0x01u << Impresora_CTRL_MARK_SHIFT)
#define Impresora_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Impresora_CTRL_PARITY_TYPE0_SHIFT)
#define Impresora_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Impresora_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Impresora_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Impresora_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Impresora_SEND_BREAK                         (0x00u)
#define Impresora_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Impresora_REINIT                             (0x02u)
#define Impresora_SEND_WAIT_REINIT                   (0x03u)

#define Impresora_OVER_SAMPLE_8                      (8u)
#define Impresora_OVER_SAMPLE_16                     (16u)

#define Impresora_BIT_CENTER                         (Impresora_OVER_SAMPLE_COUNT - 2u)

#define Impresora_FIFO_LENGTH                        (4u)
#define Impresora_NUMBER_OF_START_BIT                (1u)
#define Impresora_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define Impresora_TXBITCTR_BREAKBITS8X   ((Impresora_BREAK_BITS_TX * Impresora_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define Impresora_TXBITCTR_BREAKBITS ((Impresora_BREAK_BITS_TX * Impresora_OVER_SAMPLE_COUNT) - 1u)

#define Impresora_HALF_BIT_COUNT   \
                            (((Impresora_OVER_SAMPLE_COUNT / 2u) + (Impresora_USE23POLLING * 1u)) - 2u)
#if (Impresora_OVER_SAMPLE_COUNT == Impresora_OVER_SAMPLE_8)
    #define Impresora_HD_TXBITCTR_INIT   (((Impresora_BREAK_BITS_TX + \
                            Impresora_NUMBER_OF_START_BIT) * Impresora_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Impresora_RXBITCTR_INIT  ((((Impresora_BREAK_BITS_RX + Impresora_NUMBER_OF_START_BIT) \
                            * Impresora_OVER_SAMPLE_COUNT) + Impresora_HALF_BIT_COUNT) - 1u)

#else /* Impresora_OVER_SAMPLE_COUNT == Impresora_OVER_SAMPLE_16 */
    #define Impresora_HD_TXBITCTR_INIT   ((8u * Impresora_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define Impresora_RXBITCTR_INIT      (((7u * Impresora_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Impresora_HALF_BIT_COUNT)
#endif /* End Impresora_OVER_SAMPLE_COUNT */

#define Impresora_HD_RXBITCTR_INIT                   Impresora_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Impresora_initVar;
#if (Impresora_TX_INTERRUPT_ENABLED && Impresora_TX_ENABLED)
    extern volatile uint8 Impresora_txBuffer[Impresora_TX_BUFFER_SIZE];
    extern volatile uint8 Impresora_txBufferRead;
    extern uint8 Impresora_txBufferWrite;
#endif /* (Impresora_TX_INTERRUPT_ENABLED && Impresora_TX_ENABLED) */
#if (Impresora_RX_INTERRUPT_ENABLED && (Impresora_RX_ENABLED || Impresora_HD_ENABLED))
    extern uint8 Impresora_errorStatus;
    extern volatile uint8 Impresora_rxBuffer[Impresora_RX_BUFFER_SIZE];
    extern volatile uint8 Impresora_rxBufferRead;
    extern volatile uint8 Impresora_rxBufferWrite;
    extern volatile uint8 Impresora_rxBufferLoopDetect;
    extern volatile uint8 Impresora_rxBufferOverflow;
    #if (Impresora_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Impresora_rxAddressMode;
        extern volatile uint8 Impresora_rxAddressDetected;
    #endif /* (Impresora_RXHW_ADDRESS_ENABLED) */
#endif /* (Impresora_RX_INTERRUPT_ENABLED && (Impresora_RX_ENABLED || Impresora_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Impresora__B_UART__AM_SW_BYTE_BYTE 1
#define Impresora__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Impresora__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Impresora__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Impresora__B_UART__AM_NONE 0

#define Impresora__B_UART__NONE_REVB 0
#define Impresora__B_UART__EVEN_REVB 1
#define Impresora__B_UART__ODD_REVB 2
#define Impresora__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Impresora_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Impresora_NUMBER_OF_STOP_BITS    (1u)

#if (Impresora_RXHW_ADDRESS_ENABLED)
    #define Impresora_RX_ADDRESS_MODE    (0u)
    #define Impresora_RX_HW_ADDRESS1     (0u)
    #define Impresora_RX_HW_ADDRESS2     (0u)
#endif /* (Impresora_RXHW_ADDRESS_ENABLED) */

#define Impresora_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Impresora_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Impresora_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Impresora_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Impresora_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Impresora_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Impresora_RX_STS_BREAK_SHIFT) \
                                        | (0 << Impresora_RX_STS_OVERRUN_SHIFT))

#define Impresora_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Impresora_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Impresora_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Impresora_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Impresora_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Impresora_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Impresora_CONTROL_REG \
                            (* (reg8 *) Impresora_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Impresora_CONTROL_PTR \
                            (  (reg8 *) Impresora_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Impresora_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Impresora_TX_ENABLED)
    #define Impresora_TXDATA_REG          (* (reg8 *) Impresora_BUART_sTX_TxShifter_u0__F0_REG)
    #define Impresora_TXDATA_PTR          (  (reg8 *) Impresora_BUART_sTX_TxShifter_u0__F0_REG)
    #define Impresora_TXDATA_AUX_CTL_REG  (* (reg8 *) Impresora_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Impresora_TXDATA_AUX_CTL_PTR  (  (reg8 *) Impresora_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Impresora_TXSTATUS_REG        (* (reg8 *) Impresora_BUART_sTX_TxSts__STATUS_REG)
    #define Impresora_TXSTATUS_PTR        (  (reg8 *) Impresora_BUART_sTX_TxSts__STATUS_REG)
    #define Impresora_TXSTATUS_MASK_REG   (* (reg8 *) Impresora_BUART_sTX_TxSts__MASK_REG)
    #define Impresora_TXSTATUS_MASK_PTR   (  (reg8 *) Impresora_BUART_sTX_TxSts__MASK_REG)
    #define Impresora_TXSTATUS_ACTL_REG   (* (reg8 *) Impresora_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Impresora_TXSTATUS_ACTL_PTR   (  (reg8 *) Impresora_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Impresora_TXCLKGEN_DP)
        #define Impresora_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Impresora_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Impresora_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Impresora_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Impresora_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Impresora_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Impresora_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Impresora_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Impresora_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Impresora_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Impresora_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Impresora_TXCLKGEN_DP */

#endif /* End Impresora_TX_ENABLED */

#if(Impresora_HD_ENABLED)

    #define Impresora_TXDATA_REG             (* (reg8 *) Impresora_BUART_sRX_RxShifter_u0__F1_REG )
    #define Impresora_TXDATA_PTR             (  (reg8 *) Impresora_BUART_sRX_RxShifter_u0__F1_REG )
    #define Impresora_TXDATA_AUX_CTL_REG     (* (reg8 *) Impresora_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Impresora_TXDATA_AUX_CTL_PTR     (  (reg8 *) Impresora_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Impresora_TXSTATUS_REG           (* (reg8 *) Impresora_BUART_sRX_RxSts__STATUS_REG )
    #define Impresora_TXSTATUS_PTR           (  (reg8 *) Impresora_BUART_sRX_RxSts__STATUS_REG )
    #define Impresora_TXSTATUS_MASK_REG      (* (reg8 *) Impresora_BUART_sRX_RxSts__MASK_REG )
    #define Impresora_TXSTATUS_MASK_PTR      (  (reg8 *) Impresora_BUART_sRX_RxSts__MASK_REG )
    #define Impresora_TXSTATUS_ACTL_REG      (* (reg8 *) Impresora_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Impresora_TXSTATUS_ACTL_PTR      (  (reg8 *) Impresora_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Impresora_HD_ENABLED */

#if( (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) )
    #define Impresora_RXDATA_REG             (* (reg8 *) Impresora_BUART_sRX_RxShifter_u0__F0_REG )
    #define Impresora_RXDATA_PTR             (  (reg8 *) Impresora_BUART_sRX_RxShifter_u0__F0_REG )
    #define Impresora_RXADDRESS1_REG         (* (reg8 *) Impresora_BUART_sRX_RxShifter_u0__D0_REG )
    #define Impresora_RXADDRESS1_PTR         (  (reg8 *) Impresora_BUART_sRX_RxShifter_u0__D0_REG )
    #define Impresora_RXADDRESS2_REG         (* (reg8 *) Impresora_BUART_sRX_RxShifter_u0__D1_REG )
    #define Impresora_RXADDRESS2_PTR         (  (reg8 *) Impresora_BUART_sRX_RxShifter_u0__D1_REG )
    #define Impresora_RXDATA_AUX_CTL_REG     (* (reg8 *) Impresora_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Impresora_RXBITCTR_PERIOD_REG    (* (reg8 *) Impresora_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Impresora_RXBITCTR_PERIOD_PTR    (  (reg8 *) Impresora_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Impresora_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Impresora_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Impresora_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Impresora_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Impresora_RXBITCTR_COUNTER_REG   (* (reg8 *) Impresora_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Impresora_RXBITCTR_COUNTER_PTR   (  (reg8 *) Impresora_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Impresora_RXSTATUS_REG           (* (reg8 *) Impresora_BUART_sRX_RxSts__STATUS_REG )
    #define Impresora_RXSTATUS_PTR           (  (reg8 *) Impresora_BUART_sRX_RxSts__STATUS_REG )
    #define Impresora_RXSTATUS_MASK_REG      (* (reg8 *) Impresora_BUART_sRX_RxSts__MASK_REG )
    #define Impresora_RXSTATUS_MASK_PTR      (  (reg8 *) Impresora_BUART_sRX_RxSts__MASK_REG )
    #define Impresora_RXSTATUS_ACTL_REG      (* (reg8 *) Impresora_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Impresora_RXSTATUS_ACTL_PTR      (  (reg8 *) Impresora_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) */

#if(Impresora_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Impresora_INTCLOCK_CLKEN_REG     (* (reg8 *) Impresora_IntClock__PM_ACT_CFG)
    #define Impresora_INTCLOCK_CLKEN_PTR     (  (reg8 *) Impresora_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Impresora_INTCLOCK_CLKEN_MASK    Impresora_IntClock__PM_ACT_MSK
#endif /* End Impresora_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Impresora_TX_ENABLED)
    #define Impresora_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Impresora_TX_ENABLED */

#if(Impresora_HD_ENABLED)
    #define Impresora_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Impresora_HD_ENABLED */

#if( (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) )
    #define Impresora_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define Impresora_WAIT_1_MS      Impresora_BL_CHK_DELAY_MS   

#define Impresora_TXBUFFERSIZE   Impresora_TX_BUFFER_SIZE
#define Impresora_RXBUFFERSIZE   Impresora_RX_BUFFER_SIZE

#if (Impresora_RXHW_ADDRESS_ENABLED)
    #define Impresora_RXADDRESSMODE  Impresora_RX_ADDRESS_MODE
    #define Impresora_RXHWADDRESS1   Impresora_RX_HW_ADDRESS1
    #define Impresora_RXHWADDRESS2   Impresora_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define Impresora_RXAddressMode  Impresora_RXADDRESSMODE
#endif /* (Impresora_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define Impresora_initvar                    Impresora_initVar

#define Impresora_RX_Enabled                 Impresora_RX_ENABLED
#define Impresora_TX_Enabled                 Impresora_TX_ENABLED
#define Impresora_HD_Enabled                 Impresora_HD_ENABLED
#define Impresora_RX_IntInterruptEnabled     Impresora_RX_INTERRUPT_ENABLED
#define Impresora_TX_IntInterruptEnabled     Impresora_TX_INTERRUPT_ENABLED
#define Impresora_InternalClockUsed          Impresora_INTERNAL_CLOCK_USED
#define Impresora_RXHW_Address_Enabled       Impresora_RXHW_ADDRESS_ENABLED
#define Impresora_OverSampleCount            Impresora_OVER_SAMPLE_COUNT
#define Impresora_ParityType                 Impresora_PARITY_TYPE

#if( Impresora_TX_ENABLED && (Impresora_TXBUFFERSIZE > Impresora_FIFO_LENGTH))
    #define Impresora_TXBUFFER               Impresora_txBuffer
    #define Impresora_TXBUFFERREAD           Impresora_txBufferRead
    #define Impresora_TXBUFFERWRITE          Impresora_txBufferWrite
#endif /* End Impresora_TX_ENABLED */
#if( ( Impresora_RX_ENABLED || Impresora_HD_ENABLED ) && \
     (Impresora_RXBUFFERSIZE > Impresora_FIFO_LENGTH) )
    #define Impresora_RXBUFFER               Impresora_rxBuffer
    #define Impresora_RXBUFFERREAD           Impresora_rxBufferRead
    #define Impresora_RXBUFFERWRITE          Impresora_rxBufferWrite
    #define Impresora_RXBUFFERLOOPDETECT     Impresora_rxBufferLoopDetect
    #define Impresora_RXBUFFER_OVERFLOW      Impresora_rxBufferOverflow
#endif /* End Impresora_RX_ENABLED */

#ifdef Impresora_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Impresora_CONTROL                Impresora_CONTROL_REG
#endif /* End Impresora_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Impresora_TX_ENABLED)
    #define Impresora_TXDATA                 Impresora_TXDATA_REG
    #define Impresora_TXSTATUS               Impresora_TXSTATUS_REG
    #define Impresora_TXSTATUS_MASK          Impresora_TXSTATUS_MASK_REG
    #define Impresora_TXSTATUS_ACTL          Impresora_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Impresora_TXCLKGEN_DP)
        #define Impresora_TXBITCLKGEN_CTR        Impresora_TXBITCLKGEN_CTR_REG
        #define Impresora_TXBITCLKTX_COMPLETE    Impresora_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Impresora_TXBITCTR_PERIOD        Impresora_TXBITCTR_PERIOD_REG
        #define Impresora_TXBITCTR_CONTROL       Impresora_TXBITCTR_CONTROL_REG
        #define Impresora_TXBITCTR_COUNTER       Impresora_TXBITCTR_COUNTER_REG
    #endif /* Impresora_TXCLKGEN_DP */
#endif /* End Impresora_TX_ENABLED */

#if(Impresora_HD_ENABLED)
    #define Impresora_TXDATA                 Impresora_TXDATA_REG
    #define Impresora_TXSTATUS               Impresora_TXSTATUS_REG
    #define Impresora_TXSTATUS_MASK          Impresora_TXSTATUS_MASK_REG
    #define Impresora_TXSTATUS_ACTL          Impresora_TXSTATUS_ACTL_REG
#endif /* End Impresora_HD_ENABLED */

#if( (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) )
    #define Impresora_RXDATA                 Impresora_RXDATA_REG
    #define Impresora_RXADDRESS1             Impresora_RXADDRESS1_REG
    #define Impresora_RXADDRESS2             Impresora_RXADDRESS2_REG
    #define Impresora_RXBITCTR_PERIOD        Impresora_RXBITCTR_PERIOD_REG
    #define Impresora_RXBITCTR_CONTROL       Impresora_RXBITCTR_CONTROL_REG
    #define Impresora_RXBITCTR_COUNTER       Impresora_RXBITCTR_COUNTER_REG
    #define Impresora_RXSTATUS               Impresora_RXSTATUS_REG
    #define Impresora_RXSTATUS_MASK          Impresora_RXSTATUS_MASK_REG
    #define Impresora_RXSTATUS_ACTL          Impresora_RXSTATUS_ACTL_REG
#endif /* End  (Impresora_RX_ENABLED) || (Impresora_HD_ENABLED) */

#if(Impresora_INTERNAL_CLOCK_USED)
    #define Impresora_INTCLOCK_CLKEN         Impresora_INTCLOCK_CLKEN_REG
#endif /* End Impresora_INTERNAL_CLOCK_USED */

#define Impresora_WAIT_FOR_COMLETE_REINIT    Impresora_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Impresora_H */


/* [] END OF FILE */
