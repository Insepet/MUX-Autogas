/*******************************************************************************
* File Name: Surtidor.h
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


#if !defined(CY_UART_Surtidor_H)
#define CY_UART_Surtidor_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Surtidor_RX_ENABLED                     (1u)
#define Surtidor_TX_ENABLED                     (1u)
#define Surtidor_HD_ENABLED                     (0u)
#define Surtidor_RX_INTERRUPT_ENABLED           (1u)
#define Surtidor_TX_INTERRUPT_ENABLED           (0u)
#define Surtidor_INTERNAL_CLOCK_USED            (0u)
#define Surtidor_RXHW_ADDRESS_ENABLED           (0u)
#define Surtidor_OVER_SAMPLE_COUNT              (8u)
#define Surtidor_PARITY_TYPE                    (1u)
#define Surtidor_PARITY_TYPE_SW                 (0u)
#define Surtidor_BREAK_DETECT                   (0u)
#define Surtidor_BREAK_BITS_TX                  (13u)
#define Surtidor_BREAK_BITS_RX                  (13u)
#define Surtidor_TXCLKGEN_DP                    (1u)
#define Surtidor_USE23POLLING                   (1u)
#define Surtidor_FLOW_CONTROL                   (0u)
#define Surtidor_CLK_FREQ                       (0u)
#define Surtidor_TX_BUFFER_SIZE                 (4u)
#define Surtidor_RX_BUFFER_SIZE                 (256u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(Surtidor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define Surtidor_CONTROL_REG_REMOVED            (0u)
#else
    #define Surtidor_CONTROL_REG_REMOVED            (1u)
#endif /* End Surtidor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Surtidor_backupStruct_
{
    uint8 enableState;

    #if(Surtidor_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Surtidor_CONTROL_REG_REMOVED */

} Surtidor_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Surtidor_Start(void) ;
void Surtidor_Stop(void) ;
uint8 Surtidor_ReadControlRegister(void) ;
void Surtidor_WriteControlRegister(uint8 control) ;

void Surtidor_Init(void) ;
void Surtidor_Enable(void) ;
void Surtidor_SaveConfig(void) ;
void Surtidor_RestoreConfig(void) ;
void Surtidor_Sleep(void) ;
void Surtidor_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) )

    #if (Surtidor_RX_INTERRUPT_ENABLED)
        #define Surtidor_EnableRxInt()  CyIntEnable (Surtidor_RX_VECT_NUM)
        #define Surtidor_DisableRxInt() CyIntDisable(Surtidor_RX_VECT_NUM)
        CY_ISR_PROTO(Surtidor_RXISR);
    #endif /* Surtidor_RX_INTERRUPT_ENABLED */

    void Surtidor_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Surtidor_SetRxAddress1(uint8 address) ;
    void Surtidor_SetRxAddress2(uint8 address) ;

    void  Surtidor_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Surtidor_ReadRxData(void) ;
    uint8 Surtidor_ReadRxStatus(void) ;
    uint8 Surtidor_GetChar(void) ;
    uint16 Surtidor_GetByte(void) ;
    uint16 Surtidor_GetRxBufferSize(void)
                                                            ;
    void Surtidor_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Surtidor_GetRxInterruptSource   Surtidor_ReadRxStatus

#endif /* End (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) */

/* Only if TX is enabled */
#if(Surtidor_TX_ENABLED || Surtidor_HD_ENABLED)

    #if(Surtidor_TX_INTERRUPT_ENABLED)
        #define Surtidor_EnableTxInt()  CyIntEnable (Surtidor_TX_VECT_NUM)
        #define Surtidor_DisableTxInt() CyIntDisable(Surtidor_TX_VECT_NUM)
        #define Surtidor_SetPendingTxInt() CyIntSetPending(Surtidor_TX_VECT_NUM)
        #define Surtidor_ClearPendingTxInt() CyIntClearPending(Surtidor_TX_VECT_NUM)
        CY_ISR_PROTO(Surtidor_TXISR);
    #endif /* Surtidor_TX_INTERRUPT_ENABLED */

    void Surtidor_SetTxInterruptMode(uint8 intSrc) ;
    void Surtidor_WriteTxData(uint8 txDataByte) ;
    uint8 Surtidor_ReadTxStatus(void) ;
    void Surtidor_PutChar(uint8 txDataByte) ;
    void Surtidor_PutString(const char8 string[]) ;
    void Surtidor_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Surtidor_PutCRLF(uint8 txDataByte) ;
    void Surtidor_ClearTxBuffer(void) ;
    void Surtidor_SetTxAddressMode(uint8 addressMode) ;
    void Surtidor_SendBreak(uint8 retMode) ;
    uint8 Surtidor_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Surtidor_PutStringConst         Surtidor_PutString
    #define Surtidor_PutArrayConst          Surtidor_PutArray
    #define Surtidor_GetTxInterruptSource   Surtidor_ReadTxStatus

#endif /* End Surtidor_TX_ENABLED || Surtidor_HD_ENABLED */

#if(Surtidor_HD_ENABLED)
    void Surtidor_LoadRxConfig(void) ;
    void Surtidor_LoadTxConfig(void) ;
#endif /* End Surtidor_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Surtidor) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Surtidor_CyBtldrCommStart(void) CYSMALL ;
    void    Surtidor_CyBtldrCommStop(void) CYSMALL ;
    void    Surtidor_CyBtldrCommReset(void) CYSMALL ;
    cystatus Surtidor_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Surtidor_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Surtidor)
        #define CyBtldrCommStart    Surtidor_CyBtldrCommStart
        #define CyBtldrCommStop     Surtidor_CyBtldrCommStop
        #define CyBtldrCommReset    Surtidor_CyBtldrCommReset
        #define CyBtldrCommWrite    Surtidor_CyBtldrCommWrite
        #define CyBtldrCommRead     Surtidor_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Surtidor) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Surtidor_BYTE2BYTE_TIME_OUT (25u)
    #define Surtidor_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define Surtidor_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define Surtidor_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Surtidor_SET_SPACE      (0x00u)
#define Surtidor_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (Surtidor_TX_ENABLED) || (Surtidor_HD_ENABLED) )
    #if(Surtidor_TX_INTERRUPT_ENABLED)
        #define Surtidor_TX_VECT_NUM            (uint8)Surtidor_TXInternalInterrupt__INTC_NUMBER
        #define Surtidor_TX_PRIOR_NUM           (uint8)Surtidor_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Surtidor_TX_INTERRUPT_ENABLED */

    #define Surtidor_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define Surtidor_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define Surtidor_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(Surtidor_TX_ENABLED)
        #define Surtidor_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (Surtidor_HD_ENABLED) */
        #define Surtidor_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (Surtidor_TX_ENABLED) */

    #define Surtidor_TX_STS_COMPLETE            (uint8)(0x01u << Surtidor_TX_STS_COMPLETE_SHIFT)
    #define Surtidor_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Surtidor_TX_STS_FIFO_EMPTY_SHIFT)
    #define Surtidor_TX_STS_FIFO_FULL           (uint8)(0x01u << Surtidor_TX_STS_FIFO_FULL_SHIFT)
    #define Surtidor_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Surtidor_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Surtidor_TX_ENABLED) || (Surtidor_HD_ENABLED)*/

#if( (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) )
    #if(Surtidor_RX_INTERRUPT_ENABLED)
        #define Surtidor_RX_VECT_NUM            (uint8)Surtidor_RXInternalInterrupt__INTC_NUMBER
        #define Surtidor_RX_PRIOR_NUM           (uint8)Surtidor_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Surtidor_RX_INTERRUPT_ENABLED */
    #define Surtidor_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Surtidor_RX_STS_BREAK_SHIFT             (0x01u)
    #define Surtidor_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Surtidor_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Surtidor_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Surtidor_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Surtidor_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Surtidor_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Surtidor_RX_STS_MRKSPC           (uint8)(0x01u << Surtidor_RX_STS_MRKSPC_SHIFT)
    #define Surtidor_RX_STS_BREAK            (uint8)(0x01u << Surtidor_RX_STS_BREAK_SHIFT)
    #define Surtidor_RX_STS_PAR_ERROR        (uint8)(0x01u << Surtidor_RX_STS_PAR_ERROR_SHIFT)
    #define Surtidor_RX_STS_STOP_ERROR       (uint8)(0x01u << Surtidor_RX_STS_STOP_ERROR_SHIFT)
    #define Surtidor_RX_STS_OVERRUN          (uint8)(0x01u << Surtidor_RX_STS_OVERRUN_SHIFT)
    #define Surtidor_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Surtidor_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Surtidor_RX_STS_ADDR_MATCH       (uint8)(0x01u << Surtidor_RX_STS_ADDR_MATCH_SHIFT)
    #define Surtidor_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Surtidor_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Surtidor_RX_HW_MASK                     (0x7Fu)
#endif /* End (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) */

/* Control Register definitions */
#define Surtidor_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Surtidor_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Surtidor_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Surtidor_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Surtidor_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Surtidor_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Surtidor_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Surtidor_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Surtidor_CTRL_HD_SEND               (uint8)(0x01u << Surtidor_CTRL_HD_SEND_SHIFT)
#define Surtidor_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Surtidor_CTRL_HD_SEND_BREAK_SHIFT)
#define Surtidor_CTRL_MARK                  (uint8)(0x01u << Surtidor_CTRL_MARK_SHIFT)
#define Surtidor_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Surtidor_CTRL_PARITY_TYPE0_SHIFT)
#define Surtidor_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Surtidor_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Surtidor_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Surtidor_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Surtidor_SEND_BREAK                         (0x00u)
#define Surtidor_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Surtidor_REINIT                             (0x02u)
#define Surtidor_SEND_WAIT_REINIT                   (0x03u)

#define Surtidor_OVER_SAMPLE_8                      (8u)
#define Surtidor_OVER_SAMPLE_16                     (16u)

#define Surtidor_BIT_CENTER                         (Surtidor_OVER_SAMPLE_COUNT - 2u)

#define Surtidor_FIFO_LENGTH                        (4u)
#define Surtidor_NUMBER_OF_START_BIT                (1u)
#define Surtidor_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define Surtidor_TXBITCTR_BREAKBITS8X   ((Surtidor_BREAK_BITS_TX * Surtidor_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define Surtidor_TXBITCTR_BREAKBITS ((Surtidor_BREAK_BITS_TX * Surtidor_OVER_SAMPLE_COUNT) - 1u)

#define Surtidor_HALF_BIT_COUNT   \
                            (((Surtidor_OVER_SAMPLE_COUNT / 2u) + (Surtidor_USE23POLLING * 1u)) - 2u)
#if (Surtidor_OVER_SAMPLE_COUNT == Surtidor_OVER_SAMPLE_8)
    #define Surtidor_HD_TXBITCTR_INIT   (((Surtidor_BREAK_BITS_TX + \
                            Surtidor_NUMBER_OF_START_BIT) * Surtidor_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Surtidor_RXBITCTR_INIT  ((((Surtidor_BREAK_BITS_RX + Surtidor_NUMBER_OF_START_BIT) \
                            * Surtidor_OVER_SAMPLE_COUNT) + Surtidor_HALF_BIT_COUNT) - 1u)

#else /* Surtidor_OVER_SAMPLE_COUNT == Surtidor_OVER_SAMPLE_16 */
    #define Surtidor_HD_TXBITCTR_INIT   ((8u * Surtidor_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define Surtidor_RXBITCTR_INIT      (((7u * Surtidor_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Surtidor_HALF_BIT_COUNT)
#endif /* End Surtidor_OVER_SAMPLE_COUNT */

#define Surtidor_HD_RXBITCTR_INIT                   Surtidor_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Surtidor_initVar;
#if (Surtidor_TX_INTERRUPT_ENABLED && Surtidor_TX_ENABLED)
    extern volatile uint8 Surtidor_txBuffer[Surtidor_TX_BUFFER_SIZE];
    extern volatile uint8 Surtidor_txBufferRead;
    extern uint8 Surtidor_txBufferWrite;
#endif /* (Surtidor_TX_INTERRUPT_ENABLED && Surtidor_TX_ENABLED) */
#if (Surtidor_RX_INTERRUPT_ENABLED && (Surtidor_RX_ENABLED || Surtidor_HD_ENABLED))
    extern uint8 Surtidor_errorStatus;
    extern volatile uint8 Surtidor_rxBuffer[Surtidor_RX_BUFFER_SIZE];
    extern volatile uint16 Surtidor_rxBufferRead;
    extern volatile uint16 Surtidor_rxBufferWrite;
    extern volatile uint8 Surtidor_rxBufferLoopDetect;
    extern volatile uint8 Surtidor_rxBufferOverflow;
    #if (Surtidor_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Surtidor_rxAddressMode;
        extern volatile uint8 Surtidor_rxAddressDetected;
    #endif /* (Surtidor_RXHW_ADDRESS_ENABLED) */
#endif /* (Surtidor_RX_INTERRUPT_ENABLED && (Surtidor_RX_ENABLED || Surtidor_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Surtidor__B_UART__AM_SW_BYTE_BYTE 1
#define Surtidor__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Surtidor__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Surtidor__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Surtidor__B_UART__AM_NONE 0

#define Surtidor__B_UART__NONE_REVB 0
#define Surtidor__B_UART__EVEN_REVB 1
#define Surtidor__B_UART__ODD_REVB 2
#define Surtidor__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Surtidor_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Surtidor_NUMBER_OF_STOP_BITS    (1u)

#if (Surtidor_RXHW_ADDRESS_ENABLED)
    #define Surtidor_RX_ADDRESS_MODE    (0u)
    #define Surtidor_RX_HW_ADDRESS1     (0u)
    #define Surtidor_RX_HW_ADDRESS2     (0u)
#endif /* (Surtidor_RXHW_ADDRESS_ENABLED) */

#define Surtidor_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Surtidor_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Surtidor_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Surtidor_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Surtidor_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Surtidor_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Surtidor_RX_STS_BREAK_SHIFT) \
                                        | (0 << Surtidor_RX_STS_OVERRUN_SHIFT))

#define Surtidor_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Surtidor_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Surtidor_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Surtidor_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Surtidor_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Surtidor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Surtidor_CONTROL_REG \
                            (* (reg8 *) Surtidor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Surtidor_CONTROL_PTR \
                            (  (reg8 *) Surtidor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Surtidor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Surtidor_TX_ENABLED)
    #define Surtidor_TXDATA_REG          (* (reg8 *) Surtidor_BUART_sTX_TxShifter_u0__F0_REG)
    #define Surtidor_TXDATA_PTR          (  (reg8 *) Surtidor_BUART_sTX_TxShifter_u0__F0_REG)
    #define Surtidor_TXDATA_AUX_CTL_REG  (* (reg8 *) Surtidor_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Surtidor_TXDATA_AUX_CTL_PTR  (  (reg8 *) Surtidor_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Surtidor_TXSTATUS_REG        (* (reg8 *) Surtidor_BUART_sTX_TxSts__STATUS_REG)
    #define Surtidor_TXSTATUS_PTR        (  (reg8 *) Surtidor_BUART_sTX_TxSts__STATUS_REG)
    #define Surtidor_TXSTATUS_MASK_REG   (* (reg8 *) Surtidor_BUART_sTX_TxSts__MASK_REG)
    #define Surtidor_TXSTATUS_MASK_PTR   (  (reg8 *) Surtidor_BUART_sTX_TxSts__MASK_REG)
    #define Surtidor_TXSTATUS_ACTL_REG   (* (reg8 *) Surtidor_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Surtidor_TXSTATUS_ACTL_PTR   (  (reg8 *) Surtidor_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Surtidor_TXCLKGEN_DP)
        #define Surtidor_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Surtidor_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Surtidor_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Surtidor_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Surtidor_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Surtidor_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Surtidor_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Surtidor_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Surtidor_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Surtidor_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Surtidor_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Surtidor_TXCLKGEN_DP */

#endif /* End Surtidor_TX_ENABLED */

#if(Surtidor_HD_ENABLED)

    #define Surtidor_TXDATA_REG             (* (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__F1_REG )
    #define Surtidor_TXDATA_PTR             (  (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__F1_REG )
    #define Surtidor_TXDATA_AUX_CTL_REG     (* (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Surtidor_TXDATA_AUX_CTL_PTR     (  (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Surtidor_TXSTATUS_REG           (* (reg8 *) Surtidor_BUART_sRX_RxSts__STATUS_REG )
    #define Surtidor_TXSTATUS_PTR           (  (reg8 *) Surtidor_BUART_sRX_RxSts__STATUS_REG )
    #define Surtidor_TXSTATUS_MASK_REG      (* (reg8 *) Surtidor_BUART_sRX_RxSts__MASK_REG )
    #define Surtidor_TXSTATUS_MASK_PTR      (  (reg8 *) Surtidor_BUART_sRX_RxSts__MASK_REG )
    #define Surtidor_TXSTATUS_ACTL_REG      (* (reg8 *) Surtidor_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Surtidor_TXSTATUS_ACTL_PTR      (  (reg8 *) Surtidor_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Surtidor_HD_ENABLED */

#if( (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) )
    #define Surtidor_RXDATA_REG             (* (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__F0_REG )
    #define Surtidor_RXDATA_PTR             (  (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__F0_REG )
    #define Surtidor_RXADDRESS1_REG         (* (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__D0_REG )
    #define Surtidor_RXADDRESS1_PTR         (  (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__D0_REG )
    #define Surtidor_RXADDRESS2_REG         (* (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__D1_REG )
    #define Surtidor_RXADDRESS2_PTR         (  (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__D1_REG )
    #define Surtidor_RXDATA_AUX_CTL_REG     (* (reg8 *) Surtidor_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Surtidor_RXBITCTR_PERIOD_REG    (* (reg8 *) Surtidor_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Surtidor_RXBITCTR_PERIOD_PTR    (  (reg8 *) Surtidor_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Surtidor_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Surtidor_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Surtidor_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Surtidor_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Surtidor_RXBITCTR_COUNTER_REG   (* (reg8 *) Surtidor_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Surtidor_RXBITCTR_COUNTER_PTR   (  (reg8 *) Surtidor_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Surtidor_RXSTATUS_REG           (* (reg8 *) Surtidor_BUART_sRX_RxSts__STATUS_REG )
    #define Surtidor_RXSTATUS_PTR           (  (reg8 *) Surtidor_BUART_sRX_RxSts__STATUS_REG )
    #define Surtidor_RXSTATUS_MASK_REG      (* (reg8 *) Surtidor_BUART_sRX_RxSts__MASK_REG )
    #define Surtidor_RXSTATUS_MASK_PTR      (  (reg8 *) Surtidor_BUART_sRX_RxSts__MASK_REG )
    #define Surtidor_RXSTATUS_ACTL_REG      (* (reg8 *) Surtidor_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Surtidor_RXSTATUS_ACTL_PTR      (  (reg8 *) Surtidor_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) */

#if(Surtidor_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Surtidor_INTCLOCK_CLKEN_REG     (* (reg8 *) Surtidor_IntClock__PM_ACT_CFG)
    #define Surtidor_INTCLOCK_CLKEN_PTR     (  (reg8 *) Surtidor_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Surtidor_INTCLOCK_CLKEN_MASK    Surtidor_IntClock__PM_ACT_MSK
#endif /* End Surtidor_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Surtidor_TX_ENABLED)
    #define Surtidor_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Surtidor_TX_ENABLED */

#if(Surtidor_HD_ENABLED)
    #define Surtidor_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Surtidor_HD_ENABLED */

#if( (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) )
    #define Surtidor_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define Surtidor_WAIT_1_MS      Surtidor_BL_CHK_DELAY_MS   

#define Surtidor_TXBUFFERSIZE   Surtidor_TX_BUFFER_SIZE
#define Surtidor_RXBUFFERSIZE   Surtidor_RX_BUFFER_SIZE

#if (Surtidor_RXHW_ADDRESS_ENABLED)
    #define Surtidor_RXADDRESSMODE  Surtidor_RX_ADDRESS_MODE
    #define Surtidor_RXHWADDRESS1   Surtidor_RX_HW_ADDRESS1
    #define Surtidor_RXHWADDRESS2   Surtidor_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define Surtidor_RXAddressMode  Surtidor_RXADDRESSMODE
#endif /* (Surtidor_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define Surtidor_initvar                    Surtidor_initVar

#define Surtidor_RX_Enabled                 Surtidor_RX_ENABLED
#define Surtidor_TX_Enabled                 Surtidor_TX_ENABLED
#define Surtidor_HD_Enabled                 Surtidor_HD_ENABLED
#define Surtidor_RX_IntInterruptEnabled     Surtidor_RX_INTERRUPT_ENABLED
#define Surtidor_TX_IntInterruptEnabled     Surtidor_TX_INTERRUPT_ENABLED
#define Surtidor_InternalClockUsed          Surtidor_INTERNAL_CLOCK_USED
#define Surtidor_RXHW_Address_Enabled       Surtidor_RXHW_ADDRESS_ENABLED
#define Surtidor_OverSampleCount            Surtidor_OVER_SAMPLE_COUNT
#define Surtidor_ParityType                 Surtidor_PARITY_TYPE

#if( Surtidor_TX_ENABLED && (Surtidor_TXBUFFERSIZE > Surtidor_FIFO_LENGTH))
    #define Surtidor_TXBUFFER               Surtidor_txBuffer
    #define Surtidor_TXBUFFERREAD           Surtidor_txBufferRead
    #define Surtidor_TXBUFFERWRITE          Surtidor_txBufferWrite
#endif /* End Surtidor_TX_ENABLED */
#if( ( Surtidor_RX_ENABLED || Surtidor_HD_ENABLED ) && \
     (Surtidor_RXBUFFERSIZE > Surtidor_FIFO_LENGTH) )
    #define Surtidor_RXBUFFER               Surtidor_rxBuffer
    #define Surtidor_RXBUFFERREAD           Surtidor_rxBufferRead
    #define Surtidor_RXBUFFERWRITE          Surtidor_rxBufferWrite
    #define Surtidor_RXBUFFERLOOPDETECT     Surtidor_rxBufferLoopDetect
    #define Surtidor_RXBUFFER_OVERFLOW      Surtidor_rxBufferOverflow
#endif /* End Surtidor_RX_ENABLED */

#ifdef Surtidor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Surtidor_CONTROL                Surtidor_CONTROL_REG
#endif /* End Surtidor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Surtidor_TX_ENABLED)
    #define Surtidor_TXDATA                 Surtidor_TXDATA_REG
    #define Surtidor_TXSTATUS               Surtidor_TXSTATUS_REG
    #define Surtidor_TXSTATUS_MASK          Surtidor_TXSTATUS_MASK_REG
    #define Surtidor_TXSTATUS_ACTL          Surtidor_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Surtidor_TXCLKGEN_DP)
        #define Surtidor_TXBITCLKGEN_CTR        Surtidor_TXBITCLKGEN_CTR_REG
        #define Surtidor_TXBITCLKTX_COMPLETE    Surtidor_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Surtidor_TXBITCTR_PERIOD        Surtidor_TXBITCTR_PERIOD_REG
        #define Surtidor_TXBITCTR_CONTROL       Surtidor_TXBITCTR_CONTROL_REG
        #define Surtidor_TXBITCTR_COUNTER       Surtidor_TXBITCTR_COUNTER_REG
    #endif /* Surtidor_TXCLKGEN_DP */
#endif /* End Surtidor_TX_ENABLED */

#if(Surtidor_HD_ENABLED)
    #define Surtidor_TXDATA                 Surtidor_TXDATA_REG
    #define Surtidor_TXSTATUS               Surtidor_TXSTATUS_REG
    #define Surtidor_TXSTATUS_MASK          Surtidor_TXSTATUS_MASK_REG
    #define Surtidor_TXSTATUS_ACTL          Surtidor_TXSTATUS_ACTL_REG
#endif /* End Surtidor_HD_ENABLED */

#if( (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) )
    #define Surtidor_RXDATA                 Surtidor_RXDATA_REG
    #define Surtidor_RXADDRESS1             Surtidor_RXADDRESS1_REG
    #define Surtidor_RXADDRESS2             Surtidor_RXADDRESS2_REG
    #define Surtidor_RXBITCTR_PERIOD        Surtidor_RXBITCTR_PERIOD_REG
    #define Surtidor_RXBITCTR_CONTROL       Surtidor_RXBITCTR_CONTROL_REG
    #define Surtidor_RXBITCTR_COUNTER       Surtidor_RXBITCTR_COUNTER_REG
    #define Surtidor_RXSTATUS               Surtidor_RXSTATUS_REG
    #define Surtidor_RXSTATUS_MASK          Surtidor_RXSTATUS_MASK_REG
    #define Surtidor_RXSTATUS_ACTL          Surtidor_RXSTATUS_ACTL_REG
#endif /* End  (Surtidor_RX_ENABLED) || (Surtidor_HD_ENABLED) */

#if(Surtidor_INTERNAL_CLOCK_USED)
    #define Surtidor_INTCLOCK_CLKEN         Surtidor_INTCLOCK_CLKEN_REG
#endif /* End Surtidor_INTERNAL_CLOCK_USED */

#define Surtidor_WAIT_FOR_COMLETE_REINIT    Surtidor_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Surtidor_H */


/* [] END OF FILE */
