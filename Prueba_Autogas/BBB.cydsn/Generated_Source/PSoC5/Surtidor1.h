/*******************************************************************************
* File Name: Surtidor1.h
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


#if !defined(CY_UART_Surtidor1_H)
#define CY_UART_Surtidor1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Surtidor1_RX_ENABLED                     (1u)
#define Surtidor1_TX_ENABLED                     (1u)
#define Surtidor1_HD_ENABLED                     (0u)
#define Surtidor1_RX_INTERRUPT_ENABLED           (1u)
#define Surtidor1_TX_INTERRUPT_ENABLED           (0u)
#define Surtidor1_INTERNAL_CLOCK_USED            (0u)
#define Surtidor1_RXHW_ADDRESS_ENABLED           (0u)
#define Surtidor1_OVER_SAMPLE_COUNT              (8u)
#define Surtidor1_PARITY_TYPE                    (1u)
#define Surtidor1_PARITY_TYPE_SW                 (0u)
#define Surtidor1_BREAK_DETECT                   (0u)
#define Surtidor1_BREAK_BITS_TX                  (13u)
#define Surtidor1_BREAK_BITS_RX                  (13u)
#define Surtidor1_TXCLKGEN_DP                    (1u)
#define Surtidor1_USE23POLLING                   (1u)
#define Surtidor1_FLOW_CONTROL                   (0u)
#define Surtidor1_CLK_FREQ                       (0u)
#define Surtidor1_TXBUFFERSIZE                   (4u)
#define Surtidor1_RXBUFFERSIZE                   (10u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_30 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#ifdef Surtidor1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Surtidor1_CONTROL_REG_REMOVED            (0u)
#else
    #define Surtidor1_CONTROL_REG_REMOVED            (1u)
#endif /* End Surtidor1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Surtidor1_backupStruct_
{
    uint8 enableState;

    #if(Surtidor1_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Surtidor1_CONTROL_REG_REMOVED */
    #if( (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) )
        uint8 rx_period;
        #if (CY_UDB_V0)
            uint8 rx_mask;
            #if (Surtidor1_RXHW_ADDRESS_ENABLED)
                uint8 rx_addr1;
                uint8 rx_addr2;
            #endif /* End Surtidor1_RXHW_ADDRESS_ENABLED */
        #endif /* End CY_UDB_V0 */
    #endif  /* End (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED)*/

    #if(Surtidor1_TX_ENABLED)
        #if(Surtidor1_TXCLKGEN_DP)
            uint8 tx_clk_ctr;
            #if (CY_UDB_V0)
                uint8 tx_clk_compl;
            #endif  /* End CY_UDB_V0 */
        #else
            uint8 tx_period;
        #endif /*End Surtidor1_TXCLKGEN_DP */
        #if (CY_UDB_V0)
            uint8 tx_mask;
        #endif  /* End CY_UDB_V0 */
    #endif /*End Surtidor1_TX_ENABLED */
} Surtidor1_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Surtidor1_Start(void) ;
void Surtidor1_Stop(void) ;
uint8 Surtidor1_ReadControlRegister(void) ;
void Surtidor1_WriteControlRegister(uint8 control) ;

void Surtidor1_Init(void) ;
void Surtidor1_Enable(void) ;
void Surtidor1_SaveConfig(void) ;
void Surtidor1_RestoreConfig(void) ;
void Surtidor1_Sleep(void) ;
void Surtidor1_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) )

    #if(Surtidor1_RX_INTERRUPT_ENABLED)
        void  Surtidor1_EnableRxInt(void) ;
        void  Surtidor1_DisableRxInt(void) ;
        CY_ISR_PROTO(Surtidor1_RXISR);
    #endif /* Surtidor1_RX_INTERRUPT_ENABLED */

    void Surtidor1_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Surtidor1_SetRxAddress1(uint8 address) ;
    void Surtidor1_SetRxAddress2(uint8 address) ;

    void  Surtidor1_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Surtidor1_ReadRxData(void) ;
    uint8 Surtidor1_ReadRxStatus(void) ;
    uint8 Surtidor1_GetChar(void) ;
    uint16 Surtidor1_GetByte(void) ;
    uint8 Surtidor1_GetRxBufferSize(void)
                                                            ;
    void Surtidor1_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Surtidor1_GetRxInterruptSource   Surtidor1_ReadRxStatus

#endif /* End (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) */

/* Only if TX is enabled */
#if(Surtidor1_TX_ENABLED || Surtidor1_HD_ENABLED)

    #if(Surtidor1_TX_INTERRUPT_ENABLED)
        void Surtidor1_EnableTxInt(void) ;
        void Surtidor1_DisableTxInt(void) ;
        CY_ISR_PROTO(Surtidor1_TXISR);
    #endif /* Surtidor1_TX_INTERRUPT_ENABLED */

    void Surtidor1_SetTxInterruptMode(uint8 intSrc) ;
    void Surtidor1_WriteTxData(uint8 txDataByte) ;
    uint8 Surtidor1_ReadTxStatus(void) ;
    void Surtidor1_PutChar(uint8 txDataByte) ;
    void Surtidor1_PutString(const char8 string[]) ;
    void Surtidor1_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Surtidor1_PutCRLF(uint8 txDataByte) ;
    void Surtidor1_ClearTxBuffer(void) ;
    void Surtidor1_SetTxAddressMode(uint8 addressMode) ;
    void Surtidor1_SendBreak(uint8 retMode) ;
    uint8 Surtidor1_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Surtidor1_PutStringConst         Surtidor1_PutString
    #define Surtidor1_PutArrayConst          Surtidor1_PutArray
    #define Surtidor1_GetTxInterruptSource   Surtidor1_ReadTxStatus

#endif /* End Surtidor1_TX_ENABLED || Surtidor1_HD_ENABLED */

#if(Surtidor1_HD_ENABLED)
    void Surtidor1_LoadRxConfig(void) ;
    void Surtidor1_LoadTxConfig(void) ;
#endif /* End Surtidor1_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Surtidor1) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Surtidor1_CyBtldrCommStart(void) CYSMALL ;
    void    Surtidor1_CyBtldrCommStop(void) CYSMALL ;
    void    Surtidor1_CyBtldrCommReset(void) CYSMALL ;
    cystatus Surtidor1_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Surtidor1_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Surtidor1)
        #define CyBtldrCommStart    Surtidor1_CyBtldrCommStart
        #define CyBtldrCommStop     Surtidor1_CyBtldrCommStop
        #define CyBtldrCommReset    Surtidor1_CyBtldrCommReset
        #define CyBtldrCommWrite    Surtidor1_CyBtldrCommWrite
        #define CyBtldrCommRead     Surtidor1_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Surtidor1) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Surtidor1_BYTE2BYTE_TIME_OUT (25u)

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Surtidor1_SET_SPACE                              (0x00u)
#define Surtidor1_SET_MARK                               (0x01u)

/* Status Register definitions */
#if( (Surtidor1_TX_ENABLED) || (Surtidor1_HD_ENABLED) )
    #if(Surtidor1_TX_INTERRUPT_ENABLED)
        #define Surtidor1_TX_VECT_NUM            (uint8)Surtidor1_TXInternalInterrupt__INTC_NUMBER
        #define Surtidor1_TX_PRIOR_NUM           (uint8)Surtidor1_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Surtidor1_TX_INTERRUPT_ENABLED */
    #if(Surtidor1_TX_ENABLED)
        #define Surtidor1_TX_STS_COMPLETE_SHIFT          (0x00u)
        #define Surtidor1_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
        #define Surtidor1_TX_STS_FIFO_FULL_SHIFT         (0x02u)
        #define Surtidor1_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #endif /* Surtidor1_TX_ENABLED */
    #if(Surtidor1_HD_ENABLED)
        #define Surtidor1_TX_STS_COMPLETE_SHIFT          (0x00u)
        #define Surtidor1_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
        #define Surtidor1_TX_STS_FIFO_FULL_SHIFT         (0x05u)  /*needs MD=0*/
        #define Surtidor1_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #endif /* Surtidor1_HD_ENABLED */
    #define Surtidor1_TX_STS_COMPLETE            (uint8)(0x01u << Surtidor1_TX_STS_COMPLETE_SHIFT)
    #define Surtidor1_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Surtidor1_TX_STS_FIFO_EMPTY_SHIFT)
    #define Surtidor1_TX_STS_FIFO_FULL           (uint8)(0x01u << Surtidor1_TX_STS_FIFO_FULL_SHIFT)
    #define Surtidor1_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Surtidor1_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Surtidor1_TX_ENABLED) || (Surtidor1_HD_ENABLED)*/

#if( (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) )
    #if(Surtidor1_RX_INTERRUPT_ENABLED)
        #define Surtidor1_RX_VECT_NUM            (uint8)Surtidor1_RXInternalInterrupt__INTC_NUMBER
        #define Surtidor1_RX_PRIOR_NUM           (uint8)Surtidor1_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Surtidor1_RX_INTERRUPT_ENABLED */
    #define Surtidor1_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Surtidor1_RX_STS_BREAK_SHIFT             (0x01u)
    #define Surtidor1_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Surtidor1_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Surtidor1_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Surtidor1_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Surtidor1_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Surtidor1_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Surtidor1_RX_STS_MRKSPC           (uint8)(0x01u << Surtidor1_RX_STS_MRKSPC_SHIFT)
    #define Surtidor1_RX_STS_BREAK            (uint8)(0x01u << Surtidor1_RX_STS_BREAK_SHIFT)
    #define Surtidor1_RX_STS_PAR_ERROR        (uint8)(0x01u << Surtidor1_RX_STS_PAR_ERROR_SHIFT)
    #define Surtidor1_RX_STS_STOP_ERROR       (uint8)(0x01u << Surtidor1_RX_STS_STOP_ERROR_SHIFT)
    #define Surtidor1_RX_STS_OVERRUN          (uint8)(0x01u << Surtidor1_RX_STS_OVERRUN_SHIFT)
    #define Surtidor1_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Surtidor1_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Surtidor1_RX_STS_ADDR_MATCH       (uint8)(0x01u << Surtidor1_RX_STS_ADDR_MATCH_SHIFT)
    #define Surtidor1_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Surtidor1_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Surtidor1_RX_HW_MASK                     (0x7Fu)
#endif /* End (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) */

/* Control Register definitions */
#define Surtidor1_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Surtidor1_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Surtidor1_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Surtidor1_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Surtidor1_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Surtidor1_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Surtidor1_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Surtidor1_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Surtidor1_CTRL_HD_SEND               (uint8)(0x01u << Surtidor1_CTRL_HD_SEND_SHIFT)
#define Surtidor1_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Surtidor1_CTRL_HD_SEND_BREAK_SHIFT)
#define Surtidor1_CTRL_MARK                  (uint8)(0x01u << Surtidor1_CTRL_MARK_SHIFT)
#define Surtidor1_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Surtidor1_CTRL_PARITY_TYPE0_SHIFT)
#define Surtidor1_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Surtidor1_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Surtidor1_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Surtidor1_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Surtidor1_SEND_BREAK                         (0x00u)
#define Surtidor1_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Surtidor1_REINIT                             (0x02u)
#define Surtidor1_SEND_WAIT_REINIT                   (0x03u)

#define Surtidor1_OVER_SAMPLE_8                      (8u)
#define Surtidor1_OVER_SAMPLE_16                     (16u)

#define Surtidor1_BIT_CENTER                         (Surtidor1_OVER_SAMPLE_COUNT - 1u)

#define Surtidor1_FIFO_LENGTH                        (4u)
#define Surtidor1_NUMBER_OF_START_BIT                (1u)
#define Surtidor1_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation*/
#define Surtidor1_TXBITCTR_BREAKBITS8X   ((Surtidor1_BREAK_BITS_TX * Surtidor1_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation*/
#define Surtidor1_TXBITCTR_BREAKBITS ((Surtidor1_BREAK_BITS_TX * Surtidor1_OVER_SAMPLE_COUNT) - 1u)

#define Surtidor1_HALF_BIT_COUNT   \
                            (((Surtidor1_OVER_SAMPLE_COUNT / 2u) + (Surtidor1_USE23POLLING * 1u)) - 2u)
#if (Surtidor1_OVER_SAMPLE_COUNT == Surtidor1_OVER_SAMPLE_8)
    #define Surtidor1_HD_TXBITCTR_INIT   (((Surtidor1_BREAK_BITS_TX + \
                            Surtidor1_NUMBER_OF_START_BIT) * Surtidor1_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Surtidor1_RXBITCTR_INIT  ((((Surtidor1_BREAK_BITS_RX + Surtidor1_NUMBER_OF_START_BIT) \
                            * Surtidor1_OVER_SAMPLE_COUNT) + Surtidor1_HALF_BIT_COUNT) - 1u)


#else /* Surtidor1_OVER_SAMPLE_COUNT == Surtidor1_OVER_SAMPLE_16 */
    #define Surtidor1_HD_TXBITCTR_INIT   ((8u * Surtidor1_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount=16 */
    #define Surtidor1_RXBITCTR_INIT      (((7u * Surtidor1_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Surtidor1_HALF_BIT_COUNT)
#endif /* End Surtidor1_OVER_SAMPLE_COUNT */
#define Surtidor1_HD_RXBITCTR_INIT                   Surtidor1_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Surtidor1_initVar;
#if( Surtidor1_TX_ENABLED && (Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
    extern volatile uint8 Surtidor1_txBuffer[Surtidor1_TXBUFFERSIZE];
    extern volatile uint8 Surtidor1_txBufferRead;
    extern uint8 Surtidor1_txBufferWrite;
#endif /* End Surtidor1_TX_ENABLED */
#if( ( Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED ) && \
     (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH) )
    extern volatile uint8 Surtidor1_rxBuffer[Surtidor1_RXBUFFERSIZE];
    extern volatile uint8 Surtidor1_rxBufferRead;
    extern volatile uint8 Surtidor1_rxBufferWrite;
    extern volatile uint8 Surtidor1_rxBufferLoopDetect;
    extern volatile uint8 Surtidor1_rxBufferOverflow;
    #if (Surtidor1_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Surtidor1_rxAddressMode;
        extern volatile uint8 Surtidor1_rxAddressDetected;
    #endif /* End EnableHWAddress */
#endif /* End Surtidor1_RX_ENABLED */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Surtidor1__B_UART__AM_SW_BYTE_BYTE 1
#define Surtidor1__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Surtidor1__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Surtidor1__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Surtidor1__B_UART__AM_NONE 0

#define Surtidor1__B_UART__NONE_REVB 0
#define Surtidor1__B_UART__EVEN_REVB 1
#define Surtidor1__B_UART__ODD_REVB 2
#define Surtidor1__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Surtidor1_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Surtidor1_NUMBER_OF_STOP_BITS    (1u)

#if (Surtidor1_RXHW_ADDRESS_ENABLED)
    #define Surtidor1_RXADDRESSMODE      (0u)
    #define Surtidor1_RXHWADDRESS1       (0u)
    #define Surtidor1_RXHWADDRESS2       (0u)
    /* Backward compatible define */
    #define Surtidor1_RXAddressMode      Surtidor1_RXADDRESSMODE
#endif /* End EnableHWAddress */

#define Surtidor1_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Surtidor1_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Surtidor1_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Surtidor1_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Surtidor1_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Surtidor1_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Surtidor1_RX_STS_BREAK_SHIFT) \
                                        | (0 << Surtidor1_RX_STS_OVERRUN_SHIFT))

#define Surtidor1_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Surtidor1_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Surtidor1_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Surtidor1_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Surtidor1_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Surtidor1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Surtidor1_CONTROL_REG \
                            (* (reg8 *) Surtidor1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Surtidor1_CONTROL_PTR \
                            (  (reg8 *) Surtidor1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Surtidor1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Surtidor1_TX_ENABLED)
    #define Surtidor1_TXDATA_REG          (* (reg8 *) Surtidor1_BUART_sTX_TxShifter_u0__F0_REG)
    #define Surtidor1_TXDATA_PTR          (  (reg8 *) Surtidor1_BUART_sTX_TxShifter_u0__F0_REG)
    #define Surtidor1_TXDATA_AUX_CTL_REG  (* (reg8 *) Surtidor1_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Surtidor1_TXDATA_AUX_CTL_PTR  (  (reg8 *) Surtidor1_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Surtidor1_TXSTATUS_REG        (* (reg8 *) Surtidor1_BUART_sTX_TxSts__STATUS_REG)
    #define Surtidor1_TXSTATUS_PTR        (  (reg8 *) Surtidor1_BUART_sTX_TxSts__STATUS_REG)
    #define Surtidor1_TXSTATUS_MASK_REG   (* (reg8 *) Surtidor1_BUART_sTX_TxSts__MASK_REG)
    #define Surtidor1_TXSTATUS_MASK_PTR   (  (reg8 *) Surtidor1_BUART_sTX_TxSts__MASK_REG)
    #define Surtidor1_TXSTATUS_ACTL_REG   (* (reg8 *) Surtidor1_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Surtidor1_TXSTATUS_ACTL_PTR   (  (reg8 *) Surtidor1_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Surtidor1_TXCLKGEN_DP)
        #define Surtidor1_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Surtidor1_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Surtidor1_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Surtidor1_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Surtidor1_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Surtidor1_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Surtidor1_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Surtidor1_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Surtidor1_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Surtidor1_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Surtidor1_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Surtidor1_TXCLKGEN_DP */

#endif /* End Surtidor1_TX_ENABLED */

#if(Surtidor1_HD_ENABLED)

    #define Surtidor1_TXDATA_REG             (* (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__F1_REG )
    #define Surtidor1_TXDATA_PTR             (  (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__F1_REG )
    #define Surtidor1_TXDATA_AUX_CTL_REG     (* (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Surtidor1_TXDATA_AUX_CTL_PTR     (  (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Surtidor1_TXSTATUS_REG           (* (reg8 *) Surtidor1_BUART_sRX_RxSts__STATUS_REG )
    #define Surtidor1_TXSTATUS_PTR           (  (reg8 *) Surtidor1_BUART_sRX_RxSts__STATUS_REG )
    #define Surtidor1_TXSTATUS_MASK_REG      (* (reg8 *) Surtidor1_BUART_sRX_RxSts__MASK_REG )
    #define Surtidor1_TXSTATUS_MASK_PTR      (  (reg8 *) Surtidor1_BUART_sRX_RxSts__MASK_REG )
    #define Surtidor1_TXSTATUS_ACTL_REG      (* (reg8 *) Surtidor1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Surtidor1_TXSTATUS_ACTL_PTR      (  (reg8 *) Surtidor1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Surtidor1_HD_ENABLED */

#if( (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) )
    #define Surtidor1_RXDATA_REG             (* (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__F0_REG )
    #define Surtidor1_RXDATA_PTR             (  (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__F0_REG )
    #define Surtidor1_RXADDRESS1_REG         (* (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__D0_REG )
    #define Surtidor1_RXADDRESS1_PTR         (  (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__D0_REG )
    #define Surtidor1_RXADDRESS2_REG         (* (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__D1_REG )
    #define Surtidor1_RXADDRESS2_PTR         (  (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__D1_REG )
    #define Surtidor1_RXDATA_AUX_CTL_REG     (* (reg8 *) Surtidor1_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Surtidor1_RXBITCTR_PERIOD_REG    (* (reg8 *) Surtidor1_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Surtidor1_RXBITCTR_PERIOD_PTR    (  (reg8 *) Surtidor1_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Surtidor1_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Surtidor1_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Surtidor1_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Surtidor1_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Surtidor1_RXBITCTR_COUNTER_REG   (* (reg8 *) Surtidor1_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Surtidor1_RXBITCTR_COUNTER_PTR   (  (reg8 *) Surtidor1_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Surtidor1_RXSTATUS_REG           (* (reg8 *) Surtidor1_BUART_sRX_RxSts__STATUS_REG )
    #define Surtidor1_RXSTATUS_PTR           (  (reg8 *) Surtidor1_BUART_sRX_RxSts__STATUS_REG )
    #define Surtidor1_RXSTATUS_MASK_REG      (* (reg8 *) Surtidor1_BUART_sRX_RxSts__MASK_REG )
    #define Surtidor1_RXSTATUS_MASK_PTR      (  (reg8 *) Surtidor1_BUART_sRX_RxSts__MASK_REG )
    #define Surtidor1_RXSTATUS_ACTL_REG      (* (reg8 *) Surtidor1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Surtidor1_RXSTATUS_ACTL_PTR      (  (reg8 *) Surtidor1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) */

#if(Surtidor1_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Surtidor1_INTCLOCK_CLKEN_REG     (* (reg8 *) Surtidor1_IntClock__PM_ACT_CFG)
    #define Surtidor1_INTCLOCK_CLKEN_PTR     (  (reg8 *) Surtidor1_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Surtidor1_INTCLOCK_CLKEN_MASK    Surtidor1_IntClock__PM_ACT_MSK
#endif /* End Surtidor1_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Surtidor1_TX_ENABLED)
    #define Surtidor1_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Surtidor1_TX_ENABLED */

#if(Surtidor1_HD_ENABLED)
    #define Surtidor1_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Surtidor1_HD_ENABLED */

#if( (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) )
    #define Surtidor1_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) */


/***************************************
* Renamed global variables or defines
* for backward compatible
***************************************/

#define Surtidor1_initvar                    Surtidor1_initVar

#define Surtidor1_RX_Enabled                 Surtidor1_RX_ENABLED
#define Surtidor1_TX_Enabled                 Surtidor1_TX_ENABLED
#define Surtidor1_HD_Enabled                 Surtidor1_HD_ENABLED
#define Surtidor1_RX_IntInterruptEnabled     Surtidor1_RX_INTERRUPT_ENABLED
#define Surtidor1_TX_IntInterruptEnabled     Surtidor1_TX_INTERRUPT_ENABLED
#define Surtidor1_InternalClockUsed          Surtidor1_INTERNAL_CLOCK_USED
#define Surtidor1_RXHW_Address_Enabled       Surtidor1_RXHW_ADDRESS_ENABLED
#define Surtidor1_OverSampleCount            Surtidor1_OVER_SAMPLE_COUNT
#define Surtidor1_ParityType                 Surtidor1_PARITY_TYPE

#if( Surtidor1_TX_ENABLED && (Surtidor1_TXBUFFERSIZE > Surtidor1_FIFO_LENGTH))
    #define Surtidor1_TXBUFFER               Surtidor1_txBuffer
    #define Surtidor1_TXBUFFERREAD           Surtidor1_txBufferRead
    #define Surtidor1_TXBUFFERWRITE          Surtidor1_txBufferWrite
#endif /* End Surtidor1_TX_ENABLED */
#if( ( Surtidor1_RX_ENABLED || Surtidor1_HD_ENABLED ) && \
     (Surtidor1_RXBUFFERSIZE > Surtidor1_FIFO_LENGTH) )
    #define Surtidor1_RXBUFFER               Surtidor1_rxBuffer
    #define Surtidor1_RXBUFFERREAD           Surtidor1_rxBufferRead
    #define Surtidor1_RXBUFFERWRITE          Surtidor1_rxBufferWrite
    #define Surtidor1_RXBUFFERLOOPDETECT     Surtidor1_rxBufferLoopDetect
    #define Surtidor1_RXBUFFER_OVERFLOW      Surtidor1_rxBufferOverflow
#endif /* End Surtidor1_RX_ENABLED */

#ifdef Surtidor1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Surtidor1_CONTROL                Surtidor1_CONTROL_REG
#endif /* End Surtidor1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Surtidor1_TX_ENABLED)
    #define Surtidor1_TXDATA                 Surtidor1_TXDATA_REG
    #define Surtidor1_TXSTATUS               Surtidor1_TXSTATUS_REG
    #define Surtidor1_TXSTATUS_MASK          Surtidor1_TXSTATUS_MASK_REG
    #define Surtidor1_TXSTATUS_ACTL          Surtidor1_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Surtidor1_TXCLKGEN_DP)
        #define Surtidor1_TXBITCLKGEN_CTR        Surtidor1_TXBITCLKGEN_CTR_REG
        #define Surtidor1_TXBITCLKTX_COMPLETE    Surtidor1_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Surtidor1_TXBITCTR_PERIOD        Surtidor1_TXBITCTR_PERIOD_REG
        #define Surtidor1_TXBITCTR_CONTROL       Surtidor1_TXBITCTR_CONTROL_REG
        #define Surtidor1_TXBITCTR_COUNTER       Surtidor1_TXBITCTR_COUNTER_REG
    #endif /* Surtidor1_TXCLKGEN_DP */
#endif /* End Surtidor1_TX_ENABLED */

#if(Surtidor1_HD_ENABLED)
    #define Surtidor1_TXDATA                 Surtidor1_TXDATA_REG
    #define Surtidor1_TXSTATUS               Surtidor1_TXSTATUS_REG
    #define Surtidor1_TXSTATUS_MASK          Surtidor1_TXSTATUS_MASK_REG
    #define Surtidor1_TXSTATUS_ACTL          Surtidor1_TXSTATUS_ACTL_REG
#endif /* End Surtidor1_HD_ENABLED */

#if( (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) )
    #define Surtidor1_RXDATA                 Surtidor1_RXDATA_REG
    #define Surtidor1_RXADDRESS1             Surtidor1_RXADDRESS1_REG
    #define Surtidor1_RXADDRESS2             Surtidor1_RXADDRESS2_REG
    #define Surtidor1_RXBITCTR_PERIOD        Surtidor1_RXBITCTR_PERIOD_REG
    #define Surtidor1_RXBITCTR_CONTROL       Surtidor1_RXBITCTR_CONTROL_REG
    #define Surtidor1_RXBITCTR_COUNTER       Surtidor1_RXBITCTR_COUNTER_REG
    #define Surtidor1_RXSTATUS               Surtidor1_RXSTATUS_REG
    #define Surtidor1_RXSTATUS_MASK          Surtidor1_RXSTATUS_MASK_REG
    #define Surtidor1_RXSTATUS_ACTL          Surtidor1_RXSTATUS_ACTL_REG
#endif /* End  (Surtidor1_RX_ENABLED) || (Surtidor1_HD_ENABLED) */

#if(Surtidor1_INTERNAL_CLOCK_USED)
    #define Surtidor1_INTCLOCK_CLKEN         Surtidor1_INTCLOCK_CLKEN_REG
#endif /* End Surtidor1_INTERNAL_CLOCK_USED */

#define Surtidor1_WAIT_FOR_COMLETE_REINIT    Surtidor1_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Surtidor1_H */


/* [] END OF FILE */
