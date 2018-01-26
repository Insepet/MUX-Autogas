/*******************************************************************************
* File Name: LP.h
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


#if !defined(CY_UART_LP_H)
#define CY_UART_LP_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define LP_RX_ENABLED                     (1u)
#define LP_TX_ENABLED                     (1u)
#define LP_HD_ENABLED                     (0u)
#define LP_RX_INTERRUPT_ENABLED           (1u)
#define LP_TX_INTERRUPT_ENABLED           (0u)
#define LP_INTERNAL_CLOCK_USED            (0u)
#define LP_RXHW_ADDRESS_ENABLED           (0u)
#define LP_OVER_SAMPLE_COUNT              (8u)
#define LP_PARITY_TYPE                    (1u)
#define LP_PARITY_TYPE_SW                 (0u)
#define LP_BREAK_DETECT                   (0u)
#define LP_BREAK_BITS_TX                  (13u)
#define LP_BREAK_BITS_RX                  (13u)
#define LP_TXCLKGEN_DP                    (1u)
#define LP_USE23POLLING                   (1u)
#define LP_FLOW_CONTROL                   (0u)
#define LP_CLK_FREQ                       (0u)
#define LP_TX_BUFFER_SIZE                 (4u)
#define LP_RX_BUFFER_SIZE                 (200u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(LP_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define LP_CONTROL_REG_REMOVED            (0u)
#else
    #define LP_CONTROL_REG_REMOVED            (1u)
#endif /* End LP_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct LP_backupStruct_
{
    uint8 enableState;

    #if(LP_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End LP_CONTROL_REG_REMOVED */

} LP_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void LP_Start(void) ;
void LP_Stop(void) ;
uint8 LP_ReadControlRegister(void) ;
void LP_WriteControlRegister(uint8 control) ;

void LP_Init(void) ;
void LP_Enable(void) ;
void LP_SaveConfig(void) ;
void LP_RestoreConfig(void) ;
void LP_Sleep(void) ;
void LP_Wakeup(void) ;

/* Only if RX is enabled */
#if( (LP_RX_ENABLED) || (LP_HD_ENABLED) )

    #if (LP_RX_INTERRUPT_ENABLED)
        #define LP_EnableRxInt()  CyIntEnable (LP_RX_VECT_NUM)
        #define LP_DisableRxInt() CyIntDisable(LP_RX_VECT_NUM)
        CY_ISR_PROTO(LP_RXISR);
    #endif /* LP_RX_INTERRUPT_ENABLED */

    void LP_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void LP_SetRxAddress1(uint8 address) ;
    void LP_SetRxAddress2(uint8 address) ;

    void  LP_SetRxInterruptMode(uint8 intSrc) ;
    uint8 LP_ReadRxData(void) ;
    uint8 LP_ReadRxStatus(void) ;
    uint8 LP_GetChar(void) ;
    uint16 LP_GetByte(void) ;
    uint8 LP_GetRxBufferSize(void)
                                                            ;
    void LP_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define LP_GetRxInterruptSource   LP_ReadRxStatus

#endif /* End (LP_RX_ENABLED) || (LP_HD_ENABLED) */

/* Only if TX is enabled */
#if(LP_TX_ENABLED || LP_HD_ENABLED)

    #if(LP_TX_INTERRUPT_ENABLED)
        #define LP_EnableTxInt()  CyIntEnable (LP_TX_VECT_NUM)
        #define LP_DisableTxInt() CyIntDisable(LP_TX_VECT_NUM)
        #define LP_SetPendingTxInt() CyIntSetPending(LP_TX_VECT_NUM)
        #define LP_ClearPendingTxInt() CyIntClearPending(LP_TX_VECT_NUM)
        CY_ISR_PROTO(LP_TXISR);
    #endif /* LP_TX_INTERRUPT_ENABLED */

    void LP_SetTxInterruptMode(uint8 intSrc) ;
    void LP_WriteTxData(uint8 txDataByte) ;
    uint8 LP_ReadTxStatus(void) ;
    void LP_PutChar(uint8 txDataByte) ;
    void LP_PutString(const char8 string[]) ;
    void LP_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void LP_PutCRLF(uint8 txDataByte) ;
    void LP_ClearTxBuffer(void) ;
    void LP_SetTxAddressMode(uint8 addressMode) ;
    void LP_SendBreak(uint8 retMode) ;
    uint8 LP_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define LP_PutStringConst         LP_PutString
    #define LP_PutArrayConst          LP_PutArray
    #define LP_GetTxInterruptSource   LP_ReadTxStatus

#endif /* End LP_TX_ENABLED || LP_HD_ENABLED */

#if(LP_HD_ENABLED)
    void LP_LoadRxConfig(void) ;
    void LP_LoadTxConfig(void) ;
#endif /* End LP_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LP) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    LP_CyBtldrCommStart(void) CYSMALL ;
    void    LP_CyBtldrCommStop(void) CYSMALL ;
    void    LP_CyBtldrCommReset(void) CYSMALL ;
    cystatus LP_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus LP_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LP)
        #define CyBtldrCommStart    LP_CyBtldrCommStart
        #define CyBtldrCommStop     LP_CyBtldrCommStop
        #define CyBtldrCommReset    LP_CyBtldrCommReset
        #define CyBtldrCommWrite    LP_CyBtldrCommWrite
        #define CyBtldrCommRead     LP_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LP) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define LP_BYTE2BYTE_TIME_OUT (25u)
    #define LP_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define LP_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define LP_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define LP_SET_SPACE      (0x00u)
#define LP_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (LP_TX_ENABLED) || (LP_HD_ENABLED) )
    #if(LP_TX_INTERRUPT_ENABLED)
        #define LP_TX_VECT_NUM            (uint8)LP_TXInternalInterrupt__INTC_NUMBER
        #define LP_TX_PRIOR_NUM           (uint8)LP_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* LP_TX_INTERRUPT_ENABLED */

    #define LP_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define LP_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define LP_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(LP_TX_ENABLED)
        #define LP_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (LP_HD_ENABLED) */
        #define LP_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (LP_TX_ENABLED) */

    #define LP_TX_STS_COMPLETE            (uint8)(0x01u << LP_TX_STS_COMPLETE_SHIFT)
    #define LP_TX_STS_FIFO_EMPTY          (uint8)(0x01u << LP_TX_STS_FIFO_EMPTY_SHIFT)
    #define LP_TX_STS_FIFO_FULL           (uint8)(0x01u << LP_TX_STS_FIFO_FULL_SHIFT)
    #define LP_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << LP_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (LP_TX_ENABLED) || (LP_HD_ENABLED)*/

#if( (LP_RX_ENABLED) || (LP_HD_ENABLED) )
    #if(LP_RX_INTERRUPT_ENABLED)
        #define LP_RX_VECT_NUM            (uint8)LP_RXInternalInterrupt__INTC_NUMBER
        #define LP_RX_PRIOR_NUM           (uint8)LP_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* LP_RX_INTERRUPT_ENABLED */
    #define LP_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define LP_RX_STS_BREAK_SHIFT             (0x01u)
    #define LP_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define LP_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define LP_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define LP_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define LP_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define LP_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define LP_RX_STS_MRKSPC           (uint8)(0x01u << LP_RX_STS_MRKSPC_SHIFT)
    #define LP_RX_STS_BREAK            (uint8)(0x01u << LP_RX_STS_BREAK_SHIFT)
    #define LP_RX_STS_PAR_ERROR        (uint8)(0x01u << LP_RX_STS_PAR_ERROR_SHIFT)
    #define LP_RX_STS_STOP_ERROR       (uint8)(0x01u << LP_RX_STS_STOP_ERROR_SHIFT)
    #define LP_RX_STS_OVERRUN          (uint8)(0x01u << LP_RX_STS_OVERRUN_SHIFT)
    #define LP_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << LP_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define LP_RX_STS_ADDR_MATCH       (uint8)(0x01u << LP_RX_STS_ADDR_MATCH_SHIFT)
    #define LP_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << LP_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define LP_RX_HW_MASK                     (0x7Fu)
#endif /* End (LP_RX_ENABLED) || (LP_HD_ENABLED) */

/* Control Register definitions */
#define LP_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define LP_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define LP_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define LP_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define LP_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define LP_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define LP_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define LP_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define LP_CTRL_HD_SEND               (uint8)(0x01u << LP_CTRL_HD_SEND_SHIFT)
#define LP_CTRL_HD_SEND_BREAK         (uint8)(0x01u << LP_CTRL_HD_SEND_BREAK_SHIFT)
#define LP_CTRL_MARK                  (uint8)(0x01u << LP_CTRL_MARK_SHIFT)
#define LP_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << LP_CTRL_PARITY_TYPE0_SHIFT)
#define LP_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << LP_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define LP_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define LP_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define LP_SEND_BREAK                         (0x00u)
#define LP_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define LP_REINIT                             (0x02u)
#define LP_SEND_WAIT_REINIT                   (0x03u)

#define LP_OVER_SAMPLE_8                      (8u)
#define LP_OVER_SAMPLE_16                     (16u)

#define LP_BIT_CENTER                         (LP_OVER_SAMPLE_COUNT - 2u)

#define LP_FIFO_LENGTH                        (4u)
#define LP_NUMBER_OF_START_BIT                (1u)
#define LP_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define LP_TXBITCTR_BREAKBITS8X   ((LP_BREAK_BITS_TX * LP_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define LP_TXBITCTR_BREAKBITS ((LP_BREAK_BITS_TX * LP_OVER_SAMPLE_COUNT) - 1u)

#define LP_HALF_BIT_COUNT   \
                            (((LP_OVER_SAMPLE_COUNT / 2u) + (LP_USE23POLLING * 1u)) - 2u)
#if (LP_OVER_SAMPLE_COUNT == LP_OVER_SAMPLE_8)
    #define LP_HD_TXBITCTR_INIT   (((LP_BREAK_BITS_TX + \
                            LP_NUMBER_OF_START_BIT) * LP_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define LP_RXBITCTR_INIT  ((((LP_BREAK_BITS_RX + LP_NUMBER_OF_START_BIT) \
                            * LP_OVER_SAMPLE_COUNT) + LP_HALF_BIT_COUNT) - 1u)

#else /* LP_OVER_SAMPLE_COUNT == LP_OVER_SAMPLE_16 */
    #define LP_HD_TXBITCTR_INIT   ((8u * LP_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define LP_RXBITCTR_INIT      (((7u * LP_OVER_SAMPLE_COUNT) - 1u) + \
                                                      LP_HALF_BIT_COUNT)
#endif /* End LP_OVER_SAMPLE_COUNT */

#define LP_HD_RXBITCTR_INIT                   LP_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 LP_initVar;
#if (LP_TX_INTERRUPT_ENABLED && LP_TX_ENABLED)
    extern volatile uint8 LP_txBuffer[LP_TX_BUFFER_SIZE];
    extern volatile uint8 LP_txBufferRead;
    extern uint8 LP_txBufferWrite;
#endif /* (LP_TX_INTERRUPT_ENABLED && LP_TX_ENABLED) */
#if (LP_RX_INTERRUPT_ENABLED && (LP_RX_ENABLED || LP_HD_ENABLED))
    extern uint8 LP_errorStatus;
    extern volatile uint8 LP_rxBuffer[LP_RX_BUFFER_SIZE];
    extern volatile uint8 LP_rxBufferRead;
    extern volatile uint8 LP_rxBufferWrite;
    extern volatile uint8 LP_rxBufferLoopDetect;
    extern volatile uint8 LP_rxBufferOverflow;
    #if (LP_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 LP_rxAddressMode;
        extern volatile uint8 LP_rxAddressDetected;
    #endif /* (LP_RXHW_ADDRESS_ENABLED) */
#endif /* (LP_RX_INTERRUPT_ENABLED && (LP_RX_ENABLED || LP_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define LP__B_UART__AM_SW_BYTE_BYTE 1
#define LP__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define LP__B_UART__AM_HW_BYTE_BY_BYTE 3
#define LP__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define LP__B_UART__AM_NONE 0

#define LP__B_UART__NONE_REVB 0
#define LP__B_UART__EVEN_REVB 1
#define LP__B_UART__ODD_REVB 2
#define LP__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define LP_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define LP_NUMBER_OF_STOP_BITS    (1u)

#if (LP_RXHW_ADDRESS_ENABLED)
    #define LP_RX_ADDRESS_MODE    (0u)
    #define LP_RX_HW_ADDRESS1     (0u)
    #define LP_RX_HW_ADDRESS2     (0u)
#endif /* (LP_RXHW_ADDRESS_ENABLED) */

#define LP_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << LP_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << LP_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << LP_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << LP_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << LP_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << LP_RX_STS_BREAK_SHIFT) \
                                        | (0 << LP_RX_STS_OVERRUN_SHIFT))

#define LP_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << LP_TX_STS_COMPLETE_SHIFT) \
                                        | (1 << LP_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << LP_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << LP_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef LP_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define LP_CONTROL_REG \
                            (* (reg8 *) LP_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define LP_CONTROL_PTR \
                            (  (reg8 *) LP_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End LP_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(LP_TX_ENABLED)
    #define LP_TXDATA_REG          (* (reg8 *) LP_BUART_sTX_TxShifter_u0__F0_REG)
    #define LP_TXDATA_PTR          (  (reg8 *) LP_BUART_sTX_TxShifter_u0__F0_REG)
    #define LP_TXDATA_AUX_CTL_REG  (* (reg8 *) LP_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define LP_TXDATA_AUX_CTL_PTR  (  (reg8 *) LP_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define LP_TXSTATUS_REG        (* (reg8 *) LP_BUART_sTX_TxSts__STATUS_REG)
    #define LP_TXSTATUS_PTR        (  (reg8 *) LP_BUART_sTX_TxSts__STATUS_REG)
    #define LP_TXSTATUS_MASK_REG   (* (reg8 *) LP_BUART_sTX_TxSts__MASK_REG)
    #define LP_TXSTATUS_MASK_PTR   (  (reg8 *) LP_BUART_sTX_TxSts__MASK_REG)
    #define LP_TXSTATUS_ACTL_REG   (* (reg8 *) LP_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define LP_TXSTATUS_ACTL_PTR   (  (reg8 *) LP_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(LP_TXCLKGEN_DP)
        #define LP_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) LP_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define LP_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) LP_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define LP_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) LP_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define LP_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) LP_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define LP_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) LP_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define LP_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) LP_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define LP_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) LP_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define LP_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) LP_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define LP_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) LP_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define LP_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) LP_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* LP_TXCLKGEN_DP */

#endif /* End LP_TX_ENABLED */

#if(LP_HD_ENABLED)

    #define LP_TXDATA_REG             (* (reg8 *) LP_BUART_sRX_RxShifter_u0__F1_REG )
    #define LP_TXDATA_PTR             (  (reg8 *) LP_BUART_sRX_RxShifter_u0__F1_REG )
    #define LP_TXDATA_AUX_CTL_REG     (* (reg8 *) LP_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define LP_TXDATA_AUX_CTL_PTR     (  (reg8 *) LP_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define LP_TXSTATUS_REG           (* (reg8 *) LP_BUART_sRX_RxSts__STATUS_REG )
    #define LP_TXSTATUS_PTR           (  (reg8 *) LP_BUART_sRX_RxSts__STATUS_REG )
    #define LP_TXSTATUS_MASK_REG      (* (reg8 *) LP_BUART_sRX_RxSts__MASK_REG )
    #define LP_TXSTATUS_MASK_PTR      (  (reg8 *) LP_BUART_sRX_RxSts__MASK_REG )
    #define LP_TXSTATUS_ACTL_REG      (* (reg8 *) LP_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define LP_TXSTATUS_ACTL_PTR      (  (reg8 *) LP_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End LP_HD_ENABLED */

#if( (LP_RX_ENABLED) || (LP_HD_ENABLED) )
    #define LP_RXDATA_REG             (* (reg8 *) LP_BUART_sRX_RxShifter_u0__F0_REG )
    #define LP_RXDATA_PTR             (  (reg8 *) LP_BUART_sRX_RxShifter_u0__F0_REG )
    #define LP_RXADDRESS1_REG         (* (reg8 *) LP_BUART_sRX_RxShifter_u0__D0_REG )
    #define LP_RXADDRESS1_PTR         (  (reg8 *) LP_BUART_sRX_RxShifter_u0__D0_REG )
    #define LP_RXADDRESS2_REG         (* (reg8 *) LP_BUART_sRX_RxShifter_u0__D1_REG )
    #define LP_RXADDRESS2_PTR         (  (reg8 *) LP_BUART_sRX_RxShifter_u0__D1_REG )
    #define LP_RXDATA_AUX_CTL_REG     (* (reg8 *) LP_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define LP_RXBITCTR_PERIOD_REG    (* (reg8 *) LP_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define LP_RXBITCTR_PERIOD_PTR    (  (reg8 *) LP_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define LP_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) LP_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define LP_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) LP_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define LP_RXBITCTR_COUNTER_REG   (* (reg8 *) LP_BUART_sRX_RxBitCounter__COUNT_REG )
    #define LP_RXBITCTR_COUNTER_PTR   (  (reg8 *) LP_BUART_sRX_RxBitCounter__COUNT_REG )

    #define LP_RXSTATUS_REG           (* (reg8 *) LP_BUART_sRX_RxSts__STATUS_REG )
    #define LP_RXSTATUS_PTR           (  (reg8 *) LP_BUART_sRX_RxSts__STATUS_REG )
    #define LP_RXSTATUS_MASK_REG      (* (reg8 *) LP_BUART_sRX_RxSts__MASK_REG )
    #define LP_RXSTATUS_MASK_PTR      (  (reg8 *) LP_BUART_sRX_RxSts__MASK_REG )
    #define LP_RXSTATUS_ACTL_REG      (* (reg8 *) LP_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define LP_RXSTATUS_ACTL_PTR      (  (reg8 *) LP_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (LP_RX_ENABLED) || (LP_HD_ENABLED) */

#if(LP_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define LP_INTCLOCK_CLKEN_REG     (* (reg8 *) LP_IntClock__PM_ACT_CFG)
    #define LP_INTCLOCK_CLKEN_PTR     (  (reg8 *) LP_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define LP_INTCLOCK_CLKEN_MASK    LP_IntClock__PM_ACT_MSK
#endif /* End LP_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(LP_TX_ENABLED)
    #define LP_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End LP_TX_ENABLED */

#if(LP_HD_ENABLED)
    #define LP_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End LP_HD_ENABLED */

#if( (LP_RX_ENABLED) || (LP_HD_ENABLED) )
    #define LP_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (LP_RX_ENABLED) || (LP_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define LP_WAIT_1_MS      LP_BL_CHK_DELAY_MS   

#define LP_TXBUFFERSIZE   LP_TX_BUFFER_SIZE
#define LP_RXBUFFERSIZE   LP_RX_BUFFER_SIZE

#if (LP_RXHW_ADDRESS_ENABLED)
    #define LP_RXADDRESSMODE  LP_RX_ADDRESS_MODE
    #define LP_RXHWADDRESS1   LP_RX_HW_ADDRESS1
    #define LP_RXHWADDRESS2   LP_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define LP_RXAddressMode  LP_RXADDRESSMODE
#endif /* (LP_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define LP_initvar                    LP_initVar

#define LP_RX_Enabled                 LP_RX_ENABLED
#define LP_TX_Enabled                 LP_TX_ENABLED
#define LP_HD_Enabled                 LP_HD_ENABLED
#define LP_RX_IntInterruptEnabled     LP_RX_INTERRUPT_ENABLED
#define LP_TX_IntInterruptEnabled     LP_TX_INTERRUPT_ENABLED
#define LP_InternalClockUsed          LP_INTERNAL_CLOCK_USED
#define LP_RXHW_Address_Enabled       LP_RXHW_ADDRESS_ENABLED
#define LP_OverSampleCount            LP_OVER_SAMPLE_COUNT
#define LP_ParityType                 LP_PARITY_TYPE

#if( LP_TX_ENABLED && (LP_TXBUFFERSIZE > LP_FIFO_LENGTH))
    #define LP_TXBUFFER               LP_txBuffer
    #define LP_TXBUFFERREAD           LP_txBufferRead
    #define LP_TXBUFFERWRITE          LP_txBufferWrite
#endif /* End LP_TX_ENABLED */
#if( ( LP_RX_ENABLED || LP_HD_ENABLED ) && \
     (LP_RXBUFFERSIZE > LP_FIFO_LENGTH) )
    #define LP_RXBUFFER               LP_rxBuffer
    #define LP_RXBUFFERREAD           LP_rxBufferRead
    #define LP_RXBUFFERWRITE          LP_rxBufferWrite
    #define LP_RXBUFFERLOOPDETECT     LP_rxBufferLoopDetect
    #define LP_RXBUFFER_OVERFLOW      LP_rxBufferOverflow
#endif /* End LP_RX_ENABLED */

#ifdef LP_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define LP_CONTROL                LP_CONTROL_REG
#endif /* End LP_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(LP_TX_ENABLED)
    #define LP_TXDATA                 LP_TXDATA_REG
    #define LP_TXSTATUS               LP_TXSTATUS_REG
    #define LP_TXSTATUS_MASK          LP_TXSTATUS_MASK_REG
    #define LP_TXSTATUS_ACTL          LP_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(LP_TXCLKGEN_DP)
        #define LP_TXBITCLKGEN_CTR        LP_TXBITCLKGEN_CTR_REG
        #define LP_TXBITCLKTX_COMPLETE    LP_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define LP_TXBITCTR_PERIOD        LP_TXBITCTR_PERIOD_REG
        #define LP_TXBITCTR_CONTROL       LP_TXBITCTR_CONTROL_REG
        #define LP_TXBITCTR_COUNTER       LP_TXBITCTR_COUNTER_REG
    #endif /* LP_TXCLKGEN_DP */
#endif /* End LP_TX_ENABLED */

#if(LP_HD_ENABLED)
    #define LP_TXDATA                 LP_TXDATA_REG
    #define LP_TXSTATUS               LP_TXSTATUS_REG
    #define LP_TXSTATUS_MASK          LP_TXSTATUS_MASK_REG
    #define LP_TXSTATUS_ACTL          LP_TXSTATUS_ACTL_REG
#endif /* End LP_HD_ENABLED */

#if( (LP_RX_ENABLED) || (LP_HD_ENABLED) )
    #define LP_RXDATA                 LP_RXDATA_REG
    #define LP_RXADDRESS1             LP_RXADDRESS1_REG
    #define LP_RXADDRESS2             LP_RXADDRESS2_REG
    #define LP_RXBITCTR_PERIOD        LP_RXBITCTR_PERIOD_REG
    #define LP_RXBITCTR_CONTROL       LP_RXBITCTR_CONTROL_REG
    #define LP_RXBITCTR_COUNTER       LP_RXBITCTR_COUNTER_REG
    #define LP_RXSTATUS               LP_RXSTATUS_REG
    #define LP_RXSTATUS_MASK          LP_RXSTATUS_MASK_REG
    #define LP_RXSTATUS_ACTL          LP_RXSTATUS_ACTL_REG
#endif /* End  (LP_RX_ENABLED) || (LP_HD_ENABLED) */

#if(LP_INTERNAL_CLOCK_USED)
    #define LP_INTCLOCK_CLKEN         LP_INTCLOCK_CLKEN_REG
#endif /* End LP_INTERNAL_CLOCK_USED */

#define LP_WAIT_FOR_COMLETE_REINIT    LP_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_LP_H */


/* [] END OF FILE */
