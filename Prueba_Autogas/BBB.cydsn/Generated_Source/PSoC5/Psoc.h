/*******************************************************************************
* File Name: Psoc.h
* Version 2.30
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_Psoc_H)
#define CY_UART_Psoc_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Psoc_RX_ENABLED                     (1u)
#define Psoc_TX_ENABLED                     (1u)
#define Psoc_HD_ENABLED                     (0u)
#define Psoc_RX_INTERRUPT_ENABLED           (0u)
#define Psoc_TX_INTERRUPT_ENABLED           (0u)
#define Psoc_INTERNAL_CLOCK_USED            (1u)
#define Psoc_RXHW_ADDRESS_ENABLED           (0u)
#define Psoc_OVER_SAMPLE_COUNT              (8u)
#define Psoc_PARITY_TYPE                    (0u)
#define Psoc_PARITY_TYPE_SW                 (0u)
#define Psoc_BREAK_DETECT                   (0u)
#define Psoc_BREAK_BITS_TX                  (13u)
#define Psoc_BREAK_BITS_RX                  (13u)
#define Psoc_TXCLKGEN_DP                    (1u)
#define Psoc_USE23POLLING                   (1u)
#define Psoc_FLOW_CONTROL                   (0u)
#define Psoc_CLK_FREQ                       (0u)
#define Psoc_TXBUFFERSIZE                   (4u)
#define Psoc_RXBUFFERSIZE                   (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_30 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#ifdef Psoc_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Psoc_CONTROL_REG_REMOVED            (0u)
#else
    #define Psoc_CONTROL_REG_REMOVED            (1u)
#endif /* End Psoc_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Psoc_backupStruct_
{
    uint8 enableState;

    #if(Psoc_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Psoc_CONTROL_REG_REMOVED */
    #if( (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) )
        uint8 rx_period;
        #if (CY_UDB_V0)
            uint8 rx_mask;
            #if (Psoc_RXHW_ADDRESS_ENABLED)
                uint8 rx_addr1;
                uint8 rx_addr2;
            #endif /* End Psoc_RXHW_ADDRESS_ENABLED */
        #endif /* End CY_UDB_V0 */
    #endif  /* End (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED)*/

    #if(Psoc_TX_ENABLED)
        #if(Psoc_TXCLKGEN_DP)
            uint8 tx_clk_ctr;
            #if (CY_UDB_V0)
                uint8 tx_clk_compl;
            #endif  /* End CY_UDB_V0 */
        #else
            uint8 tx_period;
        #endif /*End Psoc_TXCLKGEN_DP */
        #if (CY_UDB_V0)
            uint8 tx_mask;
        #endif  /* End CY_UDB_V0 */
    #endif /*End Psoc_TX_ENABLED */
} Psoc_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Psoc_Start(void) ;
void Psoc_Stop(void) ;
uint8 Psoc_ReadControlRegister(void) ;
void Psoc_WriteControlRegister(uint8 control) ;

void Psoc_Init(void) ;
void Psoc_Enable(void) ;
void Psoc_SaveConfig(void) ;
void Psoc_RestoreConfig(void) ;
void Psoc_Sleep(void) ;
void Psoc_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) )

    #if(Psoc_RX_INTERRUPT_ENABLED)
        void  Psoc_EnableRxInt(void) ;
        void  Psoc_DisableRxInt(void) ;
        CY_ISR_PROTO(Psoc_RXISR);
    #endif /* Psoc_RX_INTERRUPT_ENABLED */

    void Psoc_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Psoc_SetRxAddress1(uint8 address) ;
    void Psoc_SetRxAddress2(uint8 address) ;

    void  Psoc_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Psoc_ReadRxData(void) ;
    uint8 Psoc_ReadRxStatus(void) ;
    uint8 Psoc_GetChar(void) ;
    uint16 Psoc_GetByte(void) ;
    uint8 Psoc_GetRxBufferSize(void)
                                                            ;
    void Psoc_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Psoc_GetRxInterruptSource   Psoc_ReadRxStatus

#endif /* End (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) */

/* Only if TX is enabled */
#if(Psoc_TX_ENABLED || Psoc_HD_ENABLED)

    #if(Psoc_TX_INTERRUPT_ENABLED)
        void Psoc_EnableTxInt(void) ;
        void Psoc_DisableTxInt(void) ;
        CY_ISR_PROTO(Psoc_TXISR);
    #endif /* Psoc_TX_INTERRUPT_ENABLED */

    void Psoc_SetTxInterruptMode(uint8 intSrc) ;
    void Psoc_WriteTxData(uint8 txDataByte) ;
    uint8 Psoc_ReadTxStatus(void) ;
    void Psoc_PutChar(uint8 txDataByte) ;
    void Psoc_PutString(const char8 string[]) ;
    void Psoc_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Psoc_PutCRLF(uint8 txDataByte) ;
    void Psoc_ClearTxBuffer(void) ;
    void Psoc_SetTxAddressMode(uint8 addressMode) ;
    void Psoc_SendBreak(uint8 retMode) ;
    uint8 Psoc_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Psoc_PutStringConst         Psoc_PutString
    #define Psoc_PutArrayConst          Psoc_PutArray
    #define Psoc_GetTxInterruptSource   Psoc_ReadTxStatus

#endif /* End Psoc_TX_ENABLED || Psoc_HD_ENABLED */

#if(Psoc_HD_ENABLED)
    void Psoc_LoadRxConfig(void) ;
    void Psoc_LoadTxConfig(void) ;
#endif /* End Psoc_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Psoc) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Psoc_CyBtldrCommStart(void) CYSMALL ;
    void    Psoc_CyBtldrCommStop(void) CYSMALL ;
    void    Psoc_CyBtldrCommReset(void) CYSMALL ;
    cystatus Psoc_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Psoc_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Psoc)
        #define CyBtldrCommStart    Psoc_CyBtldrCommStart
        #define CyBtldrCommStop     Psoc_CyBtldrCommStop
        #define CyBtldrCommReset    Psoc_CyBtldrCommReset
        #define CyBtldrCommWrite    Psoc_CyBtldrCommWrite
        #define CyBtldrCommRead     Psoc_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Psoc) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Psoc_BYTE2BYTE_TIME_OUT (25u)

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Psoc_SET_SPACE                              (0x00u)
#define Psoc_SET_MARK                               (0x01u)

/* Status Register definitions */
#if( (Psoc_TX_ENABLED) || (Psoc_HD_ENABLED) )
    #if(Psoc_TX_INTERRUPT_ENABLED)
        #define Psoc_TX_VECT_NUM            (uint8)Psoc_TXInternalInterrupt__INTC_NUMBER
        #define Psoc_TX_PRIOR_NUM           (uint8)Psoc_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Psoc_TX_INTERRUPT_ENABLED */
    #if(Psoc_TX_ENABLED)
        #define Psoc_TX_STS_COMPLETE_SHIFT          (0x00u)
        #define Psoc_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
        #define Psoc_TX_STS_FIFO_FULL_SHIFT         (0x02u)
        #define Psoc_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #endif /* Psoc_TX_ENABLED */
    #if(Psoc_HD_ENABLED)
        #define Psoc_TX_STS_COMPLETE_SHIFT          (0x00u)
        #define Psoc_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
        #define Psoc_TX_STS_FIFO_FULL_SHIFT         (0x05u)  /*needs MD=0*/
        #define Psoc_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #endif /* Psoc_HD_ENABLED */
    #define Psoc_TX_STS_COMPLETE            (uint8)(0x01u << Psoc_TX_STS_COMPLETE_SHIFT)
    #define Psoc_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Psoc_TX_STS_FIFO_EMPTY_SHIFT)
    #define Psoc_TX_STS_FIFO_FULL           (uint8)(0x01u << Psoc_TX_STS_FIFO_FULL_SHIFT)
    #define Psoc_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Psoc_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Psoc_TX_ENABLED) || (Psoc_HD_ENABLED)*/

#if( (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) )
    #if(Psoc_RX_INTERRUPT_ENABLED)
        #define Psoc_RX_VECT_NUM            (uint8)Psoc_RXInternalInterrupt__INTC_NUMBER
        #define Psoc_RX_PRIOR_NUM           (uint8)Psoc_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Psoc_RX_INTERRUPT_ENABLED */
    #define Psoc_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Psoc_RX_STS_BREAK_SHIFT             (0x01u)
    #define Psoc_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Psoc_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Psoc_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Psoc_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Psoc_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Psoc_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Psoc_RX_STS_MRKSPC           (uint8)(0x01u << Psoc_RX_STS_MRKSPC_SHIFT)
    #define Psoc_RX_STS_BREAK            (uint8)(0x01u << Psoc_RX_STS_BREAK_SHIFT)
    #define Psoc_RX_STS_PAR_ERROR        (uint8)(0x01u << Psoc_RX_STS_PAR_ERROR_SHIFT)
    #define Psoc_RX_STS_STOP_ERROR       (uint8)(0x01u << Psoc_RX_STS_STOP_ERROR_SHIFT)
    #define Psoc_RX_STS_OVERRUN          (uint8)(0x01u << Psoc_RX_STS_OVERRUN_SHIFT)
    #define Psoc_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Psoc_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Psoc_RX_STS_ADDR_MATCH       (uint8)(0x01u << Psoc_RX_STS_ADDR_MATCH_SHIFT)
    #define Psoc_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Psoc_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Psoc_RX_HW_MASK                     (0x7Fu)
#endif /* End (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) */

/* Control Register definitions */
#define Psoc_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Psoc_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Psoc_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Psoc_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Psoc_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Psoc_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Psoc_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Psoc_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Psoc_CTRL_HD_SEND               (uint8)(0x01u << Psoc_CTRL_HD_SEND_SHIFT)
#define Psoc_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Psoc_CTRL_HD_SEND_BREAK_SHIFT)
#define Psoc_CTRL_MARK                  (uint8)(0x01u << Psoc_CTRL_MARK_SHIFT)
#define Psoc_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Psoc_CTRL_PARITY_TYPE0_SHIFT)
#define Psoc_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Psoc_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Psoc_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Psoc_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Psoc_SEND_BREAK                         (0x00u)
#define Psoc_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Psoc_REINIT                             (0x02u)
#define Psoc_SEND_WAIT_REINIT                   (0x03u)

#define Psoc_OVER_SAMPLE_8                      (8u)
#define Psoc_OVER_SAMPLE_16                     (16u)

#define Psoc_BIT_CENTER                         (Psoc_OVER_SAMPLE_COUNT - 1u)

#define Psoc_FIFO_LENGTH                        (4u)
#define Psoc_NUMBER_OF_START_BIT                (1u)
#define Psoc_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation*/
#define Psoc_TXBITCTR_BREAKBITS8X   ((Psoc_BREAK_BITS_TX * Psoc_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation*/
#define Psoc_TXBITCTR_BREAKBITS ((Psoc_BREAK_BITS_TX * Psoc_OVER_SAMPLE_COUNT) - 1u)

#define Psoc_HALF_BIT_COUNT   \
                            (((Psoc_OVER_SAMPLE_COUNT / 2u) + (Psoc_USE23POLLING * 1u)) - 2u)
#if (Psoc_OVER_SAMPLE_COUNT == Psoc_OVER_SAMPLE_8)
    #define Psoc_HD_TXBITCTR_INIT   (((Psoc_BREAK_BITS_TX + \
                            Psoc_NUMBER_OF_START_BIT) * Psoc_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Psoc_RXBITCTR_INIT  ((((Psoc_BREAK_BITS_RX + Psoc_NUMBER_OF_START_BIT) \
                            * Psoc_OVER_SAMPLE_COUNT) + Psoc_HALF_BIT_COUNT) - 1u)


#else /* Psoc_OVER_SAMPLE_COUNT == Psoc_OVER_SAMPLE_16 */
    #define Psoc_HD_TXBITCTR_INIT   ((8u * Psoc_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount=16 */
    #define Psoc_RXBITCTR_INIT      (((7u * Psoc_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Psoc_HALF_BIT_COUNT)
#endif /* End Psoc_OVER_SAMPLE_COUNT */
#define Psoc_HD_RXBITCTR_INIT                   Psoc_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Psoc_initVar;
#if( Psoc_TX_ENABLED && (Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH))
    extern volatile uint8 Psoc_txBuffer[Psoc_TXBUFFERSIZE];
    extern volatile uint8 Psoc_txBufferRead;
    extern uint8 Psoc_txBufferWrite;
#endif /* End Psoc_TX_ENABLED */
#if( ( Psoc_RX_ENABLED || Psoc_HD_ENABLED ) && \
     (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH) )
    extern volatile uint8 Psoc_rxBuffer[Psoc_RXBUFFERSIZE];
    extern volatile uint8 Psoc_rxBufferRead;
    extern volatile uint8 Psoc_rxBufferWrite;
    extern volatile uint8 Psoc_rxBufferLoopDetect;
    extern volatile uint8 Psoc_rxBufferOverflow;
    #if (Psoc_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Psoc_rxAddressMode;
        extern volatile uint8 Psoc_rxAddressDetected;
    #endif /* End EnableHWAddress */
#endif /* End Psoc_RX_ENABLED */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Psoc__B_UART__AM_SW_BYTE_BYTE 1
#define Psoc__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Psoc__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Psoc__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Psoc__B_UART__AM_NONE 0

#define Psoc__B_UART__NONE_REVB 0
#define Psoc__B_UART__EVEN_REVB 1
#define Psoc__B_UART__ODD_REVB 2
#define Psoc__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Psoc_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Psoc_NUMBER_OF_STOP_BITS    (1u)

#if (Psoc_RXHW_ADDRESS_ENABLED)
    #define Psoc_RXADDRESSMODE      (0u)
    #define Psoc_RXHWADDRESS1       (0u)
    #define Psoc_RXHWADDRESS2       (0u)
    /* Backward compatible define */
    #define Psoc_RXAddressMode      Psoc_RXADDRESSMODE
#endif /* End EnableHWAddress */

#define Psoc_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Psoc_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Psoc_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Psoc_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Psoc_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Psoc_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Psoc_RX_STS_BREAK_SHIFT) \
                                        | (0 << Psoc_RX_STS_OVERRUN_SHIFT))

#define Psoc_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Psoc_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Psoc_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Psoc_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Psoc_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Psoc_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Psoc_CONTROL_REG \
                            (* (reg8 *) Psoc_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Psoc_CONTROL_PTR \
                            (  (reg8 *) Psoc_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Psoc_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Psoc_TX_ENABLED)
    #define Psoc_TXDATA_REG          (* (reg8 *) Psoc_BUART_sTX_TxShifter_u0__F0_REG)
    #define Psoc_TXDATA_PTR          (  (reg8 *) Psoc_BUART_sTX_TxShifter_u0__F0_REG)
    #define Psoc_TXDATA_AUX_CTL_REG  (* (reg8 *) Psoc_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Psoc_TXDATA_AUX_CTL_PTR  (  (reg8 *) Psoc_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Psoc_TXSTATUS_REG        (* (reg8 *) Psoc_BUART_sTX_TxSts__STATUS_REG)
    #define Psoc_TXSTATUS_PTR        (  (reg8 *) Psoc_BUART_sTX_TxSts__STATUS_REG)
    #define Psoc_TXSTATUS_MASK_REG   (* (reg8 *) Psoc_BUART_sTX_TxSts__MASK_REG)
    #define Psoc_TXSTATUS_MASK_PTR   (  (reg8 *) Psoc_BUART_sTX_TxSts__MASK_REG)
    #define Psoc_TXSTATUS_ACTL_REG   (* (reg8 *) Psoc_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Psoc_TXSTATUS_ACTL_PTR   (  (reg8 *) Psoc_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Psoc_TXCLKGEN_DP)
        #define Psoc_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Psoc_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Psoc_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Psoc_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Psoc_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Psoc_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Psoc_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Psoc_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Psoc_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Psoc_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Psoc_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Psoc_TXCLKGEN_DP */

#endif /* End Psoc_TX_ENABLED */

#if(Psoc_HD_ENABLED)

    #define Psoc_TXDATA_REG             (* (reg8 *) Psoc_BUART_sRX_RxShifter_u0__F1_REG )
    #define Psoc_TXDATA_PTR             (  (reg8 *) Psoc_BUART_sRX_RxShifter_u0__F1_REG )
    #define Psoc_TXDATA_AUX_CTL_REG     (* (reg8 *) Psoc_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Psoc_TXDATA_AUX_CTL_PTR     (  (reg8 *) Psoc_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Psoc_TXSTATUS_REG           (* (reg8 *) Psoc_BUART_sRX_RxSts__STATUS_REG )
    #define Psoc_TXSTATUS_PTR           (  (reg8 *) Psoc_BUART_sRX_RxSts__STATUS_REG )
    #define Psoc_TXSTATUS_MASK_REG      (* (reg8 *) Psoc_BUART_sRX_RxSts__MASK_REG )
    #define Psoc_TXSTATUS_MASK_PTR      (  (reg8 *) Psoc_BUART_sRX_RxSts__MASK_REG )
    #define Psoc_TXSTATUS_ACTL_REG      (* (reg8 *) Psoc_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Psoc_TXSTATUS_ACTL_PTR      (  (reg8 *) Psoc_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Psoc_HD_ENABLED */

#if( (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) )
    #define Psoc_RXDATA_REG             (* (reg8 *) Psoc_BUART_sRX_RxShifter_u0__F0_REG )
    #define Psoc_RXDATA_PTR             (  (reg8 *) Psoc_BUART_sRX_RxShifter_u0__F0_REG )
    #define Psoc_RXADDRESS1_REG         (* (reg8 *) Psoc_BUART_sRX_RxShifter_u0__D0_REG )
    #define Psoc_RXADDRESS1_PTR         (  (reg8 *) Psoc_BUART_sRX_RxShifter_u0__D0_REG )
    #define Psoc_RXADDRESS2_REG         (* (reg8 *) Psoc_BUART_sRX_RxShifter_u0__D1_REG )
    #define Psoc_RXADDRESS2_PTR         (  (reg8 *) Psoc_BUART_sRX_RxShifter_u0__D1_REG )
    #define Psoc_RXDATA_AUX_CTL_REG     (* (reg8 *) Psoc_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Psoc_RXBITCTR_PERIOD_REG    (* (reg8 *) Psoc_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Psoc_RXBITCTR_PERIOD_PTR    (  (reg8 *) Psoc_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Psoc_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Psoc_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Psoc_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Psoc_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Psoc_RXBITCTR_COUNTER_REG   (* (reg8 *) Psoc_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Psoc_RXBITCTR_COUNTER_PTR   (  (reg8 *) Psoc_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Psoc_RXSTATUS_REG           (* (reg8 *) Psoc_BUART_sRX_RxSts__STATUS_REG )
    #define Psoc_RXSTATUS_PTR           (  (reg8 *) Psoc_BUART_sRX_RxSts__STATUS_REG )
    #define Psoc_RXSTATUS_MASK_REG      (* (reg8 *) Psoc_BUART_sRX_RxSts__MASK_REG )
    #define Psoc_RXSTATUS_MASK_PTR      (  (reg8 *) Psoc_BUART_sRX_RxSts__MASK_REG )
    #define Psoc_RXSTATUS_ACTL_REG      (* (reg8 *) Psoc_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Psoc_RXSTATUS_ACTL_PTR      (  (reg8 *) Psoc_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) */

#if(Psoc_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Psoc_INTCLOCK_CLKEN_REG     (* (reg8 *) Psoc_IntClock__PM_ACT_CFG)
    #define Psoc_INTCLOCK_CLKEN_PTR     (  (reg8 *) Psoc_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Psoc_INTCLOCK_CLKEN_MASK    Psoc_IntClock__PM_ACT_MSK
#endif /* End Psoc_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Psoc_TX_ENABLED)
    #define Psoc_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Psoc_TX_ENABLED */

#if(Psoc_HD_ENABLED)
    #define Psoc_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Psoc_HD_ENABLED */

#if( (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) )
    #define Psoc_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) */


/***************************************
* Renamed global variables or defines
* for backward compatible
***************************************/

#define Psoc_initvar                    Psoc_initVar

#define Psoc_RX_Enabled                 Psoc_RX_ENABLED
#define Psoc_TX_Enabled                 Psoc_TX_ENABLED
#define Psoc_HD_Enabled                 Psoc_HD_ENABLED
#define Psoc_RX_IntInterruptEnabled     Psoc_RX_INTERRUPT_ENABLED
#define Psoc_TX_IntInterruptEnabled     Psoc_TX_INTERRUPT_ENABLED
#define Psoc_InternalClockUsed          Psoc_INTERNAL_CLOCK_USED
#define Psoc_RXHW_Address_Enabled       Psoc_RXHW_ADDRESS_ENABLED
#define Psoc_OverSampleCount            Psoc_OVER_SAMPLE_COUNT
#define Psoc_ParityType                 Psoc_PARITY_TYPE

#if( Psoc_TX_ENABLED && (Psoc_TXBUFFERSIZE > Psoc_FIFO_LENGTH))
    #define Psoc_TXBUFFER               Psoc_txBuffer
    #define Psoc_TXBUFFERREAD           Psoc_txBufferRead
    #define Psoc_TXBUFFERWRITE          Psoc_txBufferWrite
#endif /* End Psoc_TX_ENABLED */
#if( ( Psoc_RX_ENABLED || Psoc_HD_ENABLED ) && \
     (Psoc_RXBUFFERSIZE > Psoc_FIFO_LENGTH) )
    #define Psoc_RXBUFFER               Psoc_rxBuffer
    #define Psoc_RXBUFFERREAD           Psoc_rxBufferRead
    #define Psoc_RXBUFFERWRITE          Psoc_rxBufferWrite
    #define Psoc_RXBUFFERLOOPDETECT     Psoc_rxBufferLoopDetect
    #define Psoc_RXBUFFER_OVERFLOW      Psoc_rxBufferOverflow
#endif /* End Psoc_RX_ENABLED */

#ifdef Psoc_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Psoc_CONTROL                Psoc_CONTROL_REG
#endif /* End Psoc_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Psoc_TX_ENABLED)
    #define Psoc_TXDATA                 Psoc_TXDATA_REG
    #define Psoc_TXSTATUS               Psoc_TXSTATUS_REG
    #define Psoc_TXSTATUS_MASK          Psoc_TXSTATUS_MASK_REG
    #define Psoc_TXSTATUS_ACTL          Psoc_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Psoc_TXCLKGEN_DP)
        #define Psoc_TXBITCLKGEN_CTR        Psoc_TXBITCLKGEN_CTR_REG
        #define Psoc_TXBITCLKTX_COMPLETE    Psoc_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Psoc_TXBITCTR_PERIOD        Psoc_TXBITCTR_PERIOD_REG
        #define Psoc_TXBITCTR_CONTROL       Psoc_TXBITCTR_CONTROL_REG
        #define Psoc_TXBITCTR_COUNTER       Psoc_TXBITCTR_COUNTER_REG
    #endif /* Psoc_TXCLKGEN_DP */
#endif /* End Psoc_TX_ENABLED */

#if(Psoc_HD_ENABLED)
    #define Psoc_TXDATA                 Psoc_TXDATA_REG
    #define Psoc_TXSTATUS               Psoc_TXSTATUS_REG
    #define Psoc_TXSTATUS_MASK          Psoc_TXSTATUS_MASK_REG
    #define Psoc_TXSTATUS_ACTL          Psoc_TXSTATUS_ACTL_REG
#endif /* End Psoc_HD_ENABLED */

#if( (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) )
    #define Psoc_RXDATA                 Psoc_RXDATA_REG
    #define Psoc_RXADDRESS1             Psoc_RXADDRESS1_REG
    #define Psoc_RXADDRESS2             Psoc_RXADDRESS2_REG
    #define Psoc_RXBITCTR_PERIOD        Psoc_RXBITCTR_PERIOD_REG
    #define Psoc_RXBITCTR_CONTROL       Psoc_RXBITCTR_CONTROL_REG
    #define Psoc_RXBITCTR_COUNTER       Psoc_RXBITCTR_COUNTER_REG
    #define Psoc_RXSTATUS               Psoc_RXSTATUS_REG
    #define Psoc_RXSTATUS_MASK          Psoc_RXSTATUS_MASK_REG
    #define Psoc_RXSTATUS_ACTL          Psoc_RXSTATUS_ACTL_REG
#endif /* End  (Psoc_RX_ENABLED) || (Psoc_HD_ENABLED) */

#if(Psoc_INTERNAL_CLOCK_USED)
    #define Psoc_INTCLOCK_CLKEN         Psoc_INTCLOCK_CLKEN_REG
#endif /* End Psoc_INTERNAL_CLOCK_USED */

#define Psoc_WAIT_FOR_COMLETE_REINIT    Psoc_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Psoc_H */


/* [] END OF FILE */
