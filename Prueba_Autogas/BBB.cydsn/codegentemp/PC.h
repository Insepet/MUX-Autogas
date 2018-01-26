/*******************************************************************************
* File Name: PC.h
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


#if !defined(CY_UART_PC_H)
#define CY_UART_PC_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define PC_RX_ENABLED                     (1u)
#define PC_TX_ENABLED                     (1u)
#define PC_HD_ENABLED                     (0u)
#define PC_RX_INTERRUPT_ENABLED           (1u)
#define PC_TX_INTERRUPT_ENABLED           (0u)
#define PC_INTERNAL_CLOCK_USED            (1u)
#define PC_RXHW_ADDRESS_ENABLED           (0u)
#define PC_OVER_SAMPLE_COUNT              (8u)
#define PC_PARITY_TYPE                    (0u)
#define PC_PARITY_TYPE_SW                 (0u)
#define PC_BREAK_DETECT                   (0u)
#define PC_BREAK_BITS_TX                  (13u)
#define PC_BREAK_BITS_RX                  (13u)
#define PC_TXCLKGEN_DP                    (1u)
#define PC_USE23POLLING                   (1u)
#define PC_FLOW_CONTROL                   (0u)
#define PC_CLK_FREQ                       (0u)
#define PC_TX_BUFFER_SIZE                 (4u)
#define PC_RX_BUFFER_SIZE                 (1500u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(PC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define PC_CONTROL_REG_REMOVED            (0u)
#else
    #define PC_CONTROL_REG_REMOVED            (1u)
#endif /* End PC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct PC_backupStruct_
{
    uint8 enableState;

    #if(PC_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End PC_CONTROL_REG_REMOVED */

} PC_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void PC_Start(void) ;
void PC_Stop(void) ;
uint8 PC_ReadControlRegister(void) ;
void PC_WriteControlRegister(uint8 control) ;

void PC_Init(void) ;
void PC_Enable(void) ;
void PC_SaveConfig(void) ;
void PC_RestoreConfig(void) ;
void PC_Sleep(void) ;
void PC_Wakeup(void) ;

/* Only if RX is enabled */
#if( (PC_RX_ENABLED) || (PC_HD_ENABLED) )

    #if (PC_RX_INTERRUPT_ENABLED)
        #define PC_EnableRxInt()  CyIntEnable (PC_RX_VECT_NUM)
        #define PC_DisableRxInt() CyIntDisable(PC_RX_VECT_NUM)
        CY_ISR_PROTO(PC_RXISR);
    #endif /* PC_RX_INTERRUPT_ENABLED */

    void PC_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void PC_SetRxAddress1(uint8 address) ;
    void PC_SetRxAddress2(uint8 address) ;

    void  PC_SetRxInterruptMode(uint8 intSrc) ;
    uint8 PC_ReadRxData(void) ;
    uint8 PC_ReadRxStatus(void) ;
    uint8 PC_GetChar(void) ;
    uint16 PC_GetByte(void) ;
    uint16 PC_GetRxBufferSize(void)
                                                            ;
    void PC_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define PC_GetRxInterruptSource   PC_ReadRxStatus

#endif /* End (PC_RX_ENABLED) || (PC_HD_ENABLED) */

/* Only if TX is enabled */
#if(PC_TX_ENABLED || PC_HD_ENABLED)

    #if(PC_TX_INTERRUPT_ENABLED)
        #define PC_EnableTxInt()  CyIntEnable (PC_TX_VECT_NUM)
        #define PC_DisableTxInt() CyIntDisable(PC_TX_VECT_NUM)
        #define PC_SetPendingTxInt() CyIntSetPending(PC_TX_VECT_NUM)
        #define PC_ClearPendingTxInt() CyIntClearPending(PC_TX_VECT_NUM)
        CY_ISR_PROTO(PC_TXISR);
    #endif /* PC_TX_INTERRUPT_ENABLED */

    void PC_SetTxInterruptMode(uint8 intSrc) ;
    void PC_WriteTxData(uint8 txDataByte) ;
    uint8 PC_ReadTxStatus(void) ;
    void PC_PutChar(uint8 txDataByte) ;
    void PC_PutString(const char8 string[]) ;
    void PC_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void PC_PutCRLF(uint8 txDataByte) ;
    void PC_ClearTxBuffer(void) ;
    void PC_SetTxAddressMode(uint8 addressMode) ;
    void PC_SendBreak(uint8 retMode) ;
    uint8 PC_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define PC_PutStringConst         PC_PutString
    #define PC_PutArrayConst          PC_PutArray
    #define PC_GetTxInterruptSource   PC_ReadTxStatus

#endif /* End PC_TX_ENABLED || PC_HD_ENABLED */

#if(PC_HD_ENABLED)
    void PC_LoadRxConfig(void) ;
    void PC_LoadTxConfig(void) ;
#endif /* End PC_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_PC) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    PC_CyBtldrCommStart(void) CYSMALL ;
    void    PC_CyBtldrCommStop(void) CYSMALL ;
    void    PC_CyBtldrCommReset(void) CYSMALL ;
    cystatus PC_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus PC_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_PC)
        #define CyBtldrCommStart    PC_CyBtldrCommStart
        #define CyBtldrCommStop     PC_CyBtldrCommStop
        #define CyBtldrCommReset    PC_CyBtldrCommReset
        #define CyBtldrCommWrite    PC_CyBtldrCommWrite
        #define CyBtldrCommRead     PC_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_PC) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define PC_BYTE2BYTE_TIME_OUT (25u)
    #define PC_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define PC_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define PC_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define PC_SET_SPACE      (0x00u)
#define PC_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (PC_TX_ENABLED) || (PC_HD_ENABLED) )
    #if(PC_TX_INTERRUPT_ENABLED)
        #define PC_TX_VECT_NUM            (uint8)PC_TXInternalInterrupt__INTC_NUMBER
        #define PC_TX_PRIOR_NUM           (uint8)PC_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* PC_TX_INTERRUPT_ENABLED */

    #define PC_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define PC_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define PC_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(PC_TX_ENABLED)
        #define PC_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (PC_HD_ENABLED) */
        #define PC_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (PC_TX_ENABLED) */

    #define PC_TX_STS_COMPLETE            (uint8)(0x01u << PC_TX_STS_COMPLETE_SHIFT)
    #define PC_TX_STS_FIFO_EMPTY          (uint8)(0x01u << PC_TX_STS_FIFO_EMPTY_SHIFT)
    #define PC_TX_STS_FIFO_FULL           (uint8)(0x01u << PC_TX_STS_FIFO_FULL_SHIFT)
    #define PC_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << PC_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (PC_TX_ENABLED) || (PC_HD_ENABLED)*/

#if( (PC_RX_ENABLED) || (PC_HD_ENABLED) )
    #if(PC_RX_INTERRUPT_ENABLED)
        #define PC_RX_VECT_NUM            (uint8)PC_RXInternalInterrupt__INTC_NUMBER
        #define PC_RX_PRIOR_NUM           (uint8)PC_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* PC_RX_INTERRUPT_ENABLED */
    #define PC_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define PC_RX_STS_BREAK_SHIFT             (0x01u)
    #define PC_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define PC_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define PC_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define PC_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define PC_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define PC_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define PC_RX_STS_MRKSPC           (uint8)(0x01u << PC_RX_STS_MRKSPC_SHIFT)
    #define PC_RX_STS_BREAK            (uint8)(0x01u << PC_RX_STS_BREAK_SHIFT)
    #define PC_RX_STS_PAR_ERROR        (uint8)(0x01u << PC_RX_STS_PAR_ERROR_SHIFT)
    #define PC_RX_STS_STOP_ERROR       (uint8)(0x01u << PC_RX_STS_STOP_ERROR_SHIFT)
    #define PC_RX_STS_OVERRUN          (uint8)(0x01u << PC_RX_STS_OVERRUN_SHIFT)
    #define PC_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << PC_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define PC_RX_STS_ADDR_MATCH       (uint8)(0x01u << PC_RX_STS_ADDR_MATCH_SHIFT)
    #define PC_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << PC_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define PC_RX_HW_MASK                     (0x7Fu)
#endif /* End (PC_RX_ENABLED) || (PC_HD_ENABLED) */

/* Control Register definitions */
#define PC_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define PC_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define PC_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define PC_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define PC_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define PC_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define PC_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define PC_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define PC_CTRL_HD_SEND               (uint8)(0x01u << PC_CTRL_HD_SEND_SHIFT)
#define PC_CTRL_HD_SEND_BREAK         (uint8)(0x01u << PC_CTRL_HD_SEND_BREAK_SHIFT)
#define PC_CTRL_MARK                  (uint8)(0x01u << PC_CTRL_MARK_SHIFT)
#define PC_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << PC_CTRL_PARITY_TYPE0_SHIFT)
#define PC_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << PC_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define PC_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define PC_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define PC_SEND_BREAK                         (0x00u)
#define PC_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define PC_REINIT                             (0x02u)
#define PC_SEND_WAIT_REINIT                   (0x03u)

#define PC_OVER_SAMPLE_8                      (8u)
#define PC_OVER_SAMPLE_16                     (16u)

#define PC_BIT_CENTER                         (PC_OVER_SAMPLE_COUNT - 2u)

#define PC_FIFO_LENGTH                        (4u)
#define PC_NUMBER_OF_START_BIT                (1u)
#define PC_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define PC_TXBITCTR_BREAKBITS8X   ((PC_BREAK_BITS_TX * PC_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define PC_TXBITCTR_BREAKBITS ((PC_BREAK_BITS_TX * PC_OVER_SAMPLE_COUNT) - 1u)

#define PC_HALF_BIT_COUNT   \
                            (((PC_OVER_SAMPLE_COUNT / 2u) + (PC_USE23POLLING * 1u)) - 2u)
#if (PC_OVER_SAMPLE_COUNT == PC_OVER_SAMPLE_8)
    #define PC_HD_TXBITCTR_INIT   (((PC_BREAK_BITS_TX + \
                            PC_NUMBER_OF_START_BIT) * PC_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define PC_RXBITCTR_INIT  ((((PC_BREAK_BITS_RX + PC_NUMBER_OF_START_BIT) \
                            * PC_OVER_SAMPLE_COUNT) + PC_HALF_BIT_COUNT) - 1u)

#else /* PC_OVER_SAMPLE_COUNT == PC_OVER_SAMPLE_16 */
    #define PC_HD_TXBITCTR_INIT   ((8u * PC_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define PC_RXBITCTR_INIT      (((7u * PC_OVER_SAMPLE_COUNT) - 1u) + \
                                                      PC_HALF_BIT_COUNT)
#endif /* End PC_OVER_SAMPLE_COUNT */

#define PC_HD_RXBITCTR_INIT                   PC_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 PC_initVar;
#if (PC_TX_INTERRUPT_ENABLED && PC_TX_ENABLED)
    extern volatile uint8 PC_txBuffer[PC_TX_BUFFER_SIZE];
    extern volatile uint8 PC_txBufferRead;
    extern uint8 PC_txBufferWrite;
#endif /* (PC_TX_INTERRUPT_ENABLED && PC_TX_ENABLED) */
#if (PC_RX_INTERRUPT_ENABLED && (PC_RX_ENABLED || PC_HD_ENABLED))
    extern uint8 PC_errorStatus;
    extern volatile uint8 PC_rxBuffer[PC_RX_BUFFER_SIZE];
    extern volatile uint16 PC_rxBufferRead;
    extern volatile uint16 PC_rxBufferWrite;
    extern volatile uint8 PC_rxBufferLoopDetect;
    extern volatile uint8 PC_rxBufferOverflow;
    #if (PC_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 PC_rxAddressMode;
        extern volatile uint8 PC_rxAddressDetected;
    #endif /* (PC_RXHW_ADDRESS_ENABLED) */
#endif /* (PC_RX_INTERRUPT_ENABLED && (PC_RX_ENABLED || PC_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define PC__B_UART__AM_SW_BYTE_BYTE 1
#define PC__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define PC__B_UART__AM_HW_BYTE_BY_BYTE 3
#define PC__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define PC__B_UART__AM_NONE 0

#define PC__B_UART__NONE_REVB 0
#define PC__B_UART__EVEN_REVB 1
#define PC__B_UART__ODD_REVB 2
#define PC__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define PC_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define PC_NUMBER_OF_STOP_BITS    (1u)

#if (PC_RXHW_ADDRESS_ENABLED)
    #define PC_RX_ADDRESS_MODE    (0u)
    #define PC_RX_HW_ADDRESS1     (0u)
    #define PC_RX_HW_ADDRESS2     (0u)
#endif /* (PC_RXHW_ADDRESS_ENABLED) */

#define PC_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << PC_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << PC_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << PC_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << PC_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << PC_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << PC_RX_STS_BREAK_SHIFT) \
                                        | (0 << PC_RX_STS_OVERRUN_SHIFT))

#define PC_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << PC_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << PC_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << PC_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << PC_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef PC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define PC_CONTROL_REG \
                            (* (reg8 *) PC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define PC_CONTROL_PTR \
                            (  (reg8 *) PC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End PC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(PC_TX_ENABLED)
    #define PC_TXDATA_REG          (* (reg8 *) PC_BUART_sTX_TxShifter_u0__F0_REG)
    #define PC_TXDATA_PTR          (  (reg8 *) PC_BUART_sTX_TxShifter_u0__F0_REG)
    #define PC_TXDATA_AUX_CTL_REG  (* (reg8 *) PC_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define PC_TXDATA_AUX_CTL_PTR  (  (reg8 *) PC_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define PC_TXSTATUS_REG        (* (reg8 *) PC_BUART_sTX_TxSts__STATUS_REG)
    #define PC_TXSTATUS_PTR        (  (reg8 *) PC_BUART_sTX_TxSts__STATUS_REG)
    #define PC_TXSTATUS_MASK_REG   (* (reg8 *) PC_BUART_sTX_TxSts__MASK_REG)
    #define PC_TXSTATUS_MASK_PTR   (  (reg8 *) PC_BUART_sTX_TxSts__MASK_REG)
    #define PC_TXSTATUS_ACTL_REG   (* (reg8 *) PC_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define PC_TXSTATUS_ACTL_PTR   (  (reg8 *) PC_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(PC_TXCLKGEN_DP)
        #define PC_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) PC_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define PC_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) PC_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define PC_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) PC_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define PC_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) PC_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define PC_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) PC_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define PC_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) PC_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define PC_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) PC_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define PC_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) PC_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define PC_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) PC_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define PC_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) PC_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* PC_TXCLKGEN_DP */

#endif /* End PC_TX_ENABLED */

#if(PC_HD_ENABLED)

    #define PC_TXDATA_REG             (* (reg8 *) PC_BUART_sRX_RxShifter_u0__F1_REG )
    #define PC_TXDATA_PTR             (  (reg8 *) PC_BUART_sRX_RxShifter_u0__F1_REG )
    #define PC_TXDATA_AUX_CTL_REG     (* (reg8 *) PC_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define PC_TXDATA_AUX_CTL_PTR     (  (reg8 *) PC_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define PC_TXSTATUS_REG           (* (reg8 *) PC_BUART_sRX_RxSts__STATUS_REG )
    #define PC_TXSTATUS_PTR           (  (reg8 *) PC_BUART_sRX_RxSts__STATUS_REG )
    #define PC_TXSTATUS_MASK_REG      (* (reg8 *) PC_BUART_sRX_RxSts__MASK_REG )
    #define PC_TXSTATUS_MASK_PTR      (  (reg8 *) PC_BUART_sRX_RxSts__MASK_REG )
    #define PC_TXSTATUS_ACTL_REG      (* (reg8 *) PC_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define PC_TXSTATUS_ACTL_PTR      (  (reg8 *) PC_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End PC_HD_ENABLED */

#if( (PC_RX_ENABLED) || (PC_HD_ENABLED) )
    #define PC_RXDATA_REG             (* (reg8 *) PC_BUART_sRX_RxShifter_u0__F0_REG )
    #define PC_RXDATA_PTR             (  (reg8 *) PC_BUART_sRX_RxShifter_u0__F0_REG )
    #define PC_RXADDRESS1_REG         (* (reg8 *) PC_BUART_sRX_RxShifter_u0__D0_REG )
    #define PC_RXADDRESS1_PTR         (  (reg8 *) PC_BUART_sRX_RxShifter_u0__D0_REG )
    #define PC_RXADDRESS2_REG         (* (reg8 *) PC_BUART_sRX_RxShifter_u0__D1_REG )
    #define PC_RXADDRESS2_PTR         (  (reg8 *) PC_BUART_sRX_RxShifter_u0__D1_REG )
    #define PC_RXDATA_AUX_CTL_REG     (* (reg8 *) PC_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define PC_RXBITCTR_PERIOD_REG    (* (reg8 *) PC_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define PC_RXBITCTR_PERIOD_PTR    (  (reg8 *) PC_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define PC_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) PC_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define PC_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) PC_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define PC_RXBITCTR_COUNTER_REG   (* (reg8 *) PC_BUART_sRX_RxBitCounter__COUNT_REG )
    #define PC_RXBITCTR_COUNTER_PTR   (  (reg8 *) PC_BUART_sRX_RxBitCounter__COUNT_REG )

    #define PC_RXSTATUS_REG           (* (reg8 *) PC_BUART_sRX_RxSts__STATUS_REG )
    #define PC_RXSTATUS_PTR           (  (reg8 *) PC_BUART_sRX_RxSts__STATUS_REG )
    #define PC_RXSTATUS_MASK_REG      (* (reg8 *) PC_BUART_sRX_RxSts__MASK_REG )
    #define PC_RXSTATUS_MASK_PTR      (  (reg8 *) PC_BUART_sRX_RxSts__MASK_REG )
    #define PC_RXSTATUS_ACTL_REG      (* (reg8 *) PC_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define PC_RXSTATUS_ACTL_PTR      (  (reg8 *) PC_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (PC_RX_ENABLED) || (PC_HD_ENABLED) */

#if(PC_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define PC_INTCLOCK_CLKEN_REG     (* (reg8 *) PC_IntClock__PM_ACT_CFG)
    #define PC_INTCLOCK_CLKEN_PTR     (  (reg8 *) PC_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define PC_INTCLOCK_CLKEN_MASK    PC_IntClock__PM_ACT_MSK
#endif /* End PC_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(PC_TX_ENABLED)
    #define PC_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End PC_TX_ENABLED */

#if(PC_HD_ENABLED)
    #define PC_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End PC_HD_ENABLED */

#if( (PC_RX_ENABLED) || (PC_HD_ENABLED) )
    #define PC_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (PC_RX_ENABLED) || (PC_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define PC_WAIT_1_MS      PC_BL_CHK_DELAY_MS   

#define PC_TXBUFFERSIZE   PC_TX_BUFFER_SIZE
#define PC_RXBUFFERSIZE   PC_RX_BUFFER_SIZE

#if (PC_RXHW_ADDRESS_ENABLED)
    #define PC_RXADDRESSMODE  PC_RX_ADDRESS_MODE
    #define PC_RXHWADDRESS1   PC_RX_HW_ADDRESS1
    #define PC_RXHWADDRESS2   PC_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define PC_RXAddressMode  PC_RXADDRESSMODE
#endif /* (PC_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define PC_initvar                    PC_initVar

#define PC_RX_Enabled                 PC_RX_ENABLED
#define PC_TX_Enabled                 PC_TX_ENABLED
#define PC_HD_Enabled                 PC_HD_ENABLED
#define PC_RX_IntInterruptEnabled     PC_RX_INTERRUPT_ENABLED
#define PC_TX_IntInterruptEnabled     PC_TX_INTERRUPT_ENABLED
#define PC_InternalClockUsed          PC_INTERNAL_CLOCK_USED
#define PC_RXHW_Address_Enabled       PC_RXHW_ADDRESS_ENABLED
#define PC_OverSampleCount            PC_OVER_SAMPLE_COUNT
#define PC_ParityType                 PC_PARITY_TYPE

#if( PC_TX_ENABLED && (PC_TXBUFFERSIZE > PC_FIFO_LENGTH))
    #define PC_TXBUFFER               PC_txBuffer
    #define PC_TXBUFFERREAD           PC_txBufferRead
    #define PC_TXBUFFERWRITE          PC_txBufferWrite
#endif /* End PC_TX_ENABLED */
#if( ( PC_RX_ENABLED || PC_HD_ENABLED ) && \
     (PC_RXBUFFERSIZE > PC_FIFO_LENGTH) )
    #define PC_RXBUFFER               PC_rxBuffer
    #define PC_RXBUFFERREAD           PC_rxBufferRead
    #define PC_RXBUFFERWRITE          PC_rxBufferWrite
    #define PC_RXBUFFERLOOPDETECT     PC_rxBufferLoopDetect
    #define PC_RXBUFFER_OVERFLOW      PC_rxBufferOverflow
#endif /* End PC_RX_ENABLED */

#ifdef PC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define PC_CONTROL                PC_CONTROL_REG
#endif /* End PC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(PC_TX_ENABLED)
    #define PC_TXDATA                 PC_TXDATA_REG
    #define PC_TXSTATUS               PC_TXSTATUS_REG
    #define PC_TXSTATUS_MASK          PC_TXSTATUS_MASK_REG
    #define PC_TXSTATUS_ACTL          PC_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(PC_TXCLKGEN_DP)
        #define PC_TXBITCLKGEN_CTR        PC_TXBITCLKGEN_CTR_REG
        #define PC_TXBITCLKTX_COMPLETE    PC_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define PC_TXBITCTR_PERIOD        PC_TXBITCTR_PERIOD_REG
        #define PC_TXBITCTR_CONTROL       PC_TXBITCTR_CONTROL_REG
        #define PC_TXBITCTR_COUNTER       PC_TXBITCTR_COUNTER_REG
    #endif /* PC_TXCLKGEN_DP */
#endif /* End PC_TX_ENABLED */

#if(PC_HD_ENABLED)
    #define PC_TXDATA                 PC_TXDATA_REG
    #define PC_TXSTATUS               PC_TXSTATUS_REG
    #define PC_TXSTATUS_MASK          PC_TXSTATUS_MASK_REG
    #define PC_TXSTATUS_ACTL          PC_TXSTATUS_ACTL_REG
#endif /* End PC_HD_ENABLED */

#if( (PC_RX_ENABLED) || (PC_HD_ENABLED) )
    #define PC_RXDATA                 PC_RXDATA_REG
    #define PC_RXADDRESS1             PC_RXADDRESS1_REG
    #define PC_RXADDRESS2             PC_RXADDRESS2_REG
    #define PC_RXBITCTR_PERIOD        PC_RXBITCTR_PERIOD_REG
    #define PC_RXBITCTR_CONTROL       PC_RXBITCTR_CONTROL_REG
    #define PC_RXBITCTR_COUNTER       PC_RXBITCTR_COUNTER_REG
    #define PC_RXSTATUS               PC_RXSTATUS_REG
    #define PC_RXSTATUS_MASK          PC_RXSTATUS_MASK_REG
    #define PC_RXSTATUS_ACTL          PC_RXSTATUS_ACTL_REG
#endif /* End  (PC_RX_ENABLED) || (PC_HD_ENABLED) */

#if(PC_INTERNAL_CLOCK_USED)
    #define PC_INTCLOCK_CLKEN         PC_INTCLOCK_CLKEN_REG
#endif /* End PC_INTERNAL_CLOCK_USED */

#define PC_WAIT_FOR_COMLETE_REINIT    PC_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_PC_H */


/* [] END OF FILE */
