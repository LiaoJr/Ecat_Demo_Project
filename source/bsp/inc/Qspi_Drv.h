/*
 * Describe: QSPI读写接口
 * Author:   wangqingsheng
 * Date:     2024-03-14
 */

#ifndef __QSIP_DRV_H__
#define __QSIP_DRV_H__

#include "hc32_ddl.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    
extern uint8_t gau8DummyCntArr[39];
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
 
#ifndef LOBYTE
    #define LOBYTE(x)              ((x) & 0xFF)
#endif
    
#ifndef HIBYTE
    #define HIBYTE(x)              (((x)&0xFF00) >> 8)
#endif   
    
/* QSPCK Port/Pin definition */
#define QSPCK_PORT                      (PortB)
#define QSPCK_PIN                       (Pin14)

/* QSNSS Port/Pin definition */
#define QSNSS_PORT                      (PortB)
#define QSNSS_PIN                       (Pin01)

/* QSIO0 Port/Pin definition */
#define QSIO0_PORT                      (PortB)
#define QSIO0_PIN                       (Pin13)

/* QSIO1 Port/Pin definition */
#define QSIO1_PORT                      (PortB)
#define QSIO1_PIN                       (Pin12)

/* QSIO2 Port/Pin definition */
#define QSIO2_PORT                      (PortB)
#define QSIO2_PIN                       (Pin10)

/* QSIO3 Port/Pin definition */
#define QSIO3_PORT                      (PortB)
#define QSIO3_PIN                       (Pin02)

/* QSPI memory bus address definition */
#define QSPI_BUS_ADDRESS                (0x98000000ul)

#define CMD_SPI_SETCFG           0x01
#define CMD_SERIAL_READ          0x03
#define CMD_FAST_READ            0x0B
#define CMD_DUAL_OP_READ         0x3B
#define CMD_DUAL_IO_READ         0xBB
#define CMD_QUAD_OP_READ         0x6B
#define CMD_QUAD_IO_READ         0xEB
#define CMD_SERIAL_WRITE         0x02
#define CMD_DUAL_DATA_WRITE      0x32
#define CMD_DUAL_ADDR_DATA_WRITE 0xB2
#define CMD_QUAD_DATA_WRITE      0x62
#define CMD_QUAD_ADDR_DARA_WRITE 0xE2

#define CMD_SERIAL_READ_DUMMY 0
#define CMD_FAST_READ_DUMMY 1
#define CMD_DUAL_OP_READ_DUMMY 1
#define CMD_DUAL_IO_READ_DUMMY 2
#define CMD_QUAD_OP_READ_DUMMY 1
#define CMD_QUAD_IO_READ_DUMMY 4
#define CMD_SERIAL_WRITE_DUMMY 0
#define CMD_DUAL_DATA_WRITE_DUMMY 0
#define CMD_DUAL_ADDR_DATA_WRITE_DUMMY 0
#define CMD_QUAD_DATA_WRITE_DUMMY 0
#define CMD_QUAD_ADDR_DARA_WRITE_DUMMY 0

#define ESC_CSR_CMD_REG		0x304
#define ESC_CSR_DATA_REG	0x300
#define ESC_WRITE_BYTE 		0x80
#define ESC_READ_BYTE 		0xC0
#define ESC_CSR_BUSY		0x80

#define READ_TERMINATION_BYTE 0xFF
#define SPI_WRITE_SETUP_BYTES   3
#define ETHERCAT_IS_SUPPORT_DUMMY_CYCLE  1

#define ONE_BYTE_MAX_XFR_LEN 127

#define CLK_PERIOD_1MHZ		(1000)
#define DRV_LAN9253_BAUDRATE_PDI_FREQ 50

// Internal Access Time (IAT) in Nano seconds (ns) based on Hardware Design
#define IAT_NULL		0
#define IAT_BYRD		360
#define IAT_BYWR		265
#define IAT_DWRD		600
#define IAT_DWDWR		505

typedef union {
    uint32_t Val;
    uint16_t w[2] __attribute__((packed));
    uint8_t  v[4];
    struct __attribute__((packed)) {
        uint16_t LW;
        uint16_t HW;
    } word;
    struct __attribute__((packed)) {
        uint8_t LB;
        uint8_t HB;
        uint8_t UB;
        uint8_t MB;
    } byte;

} UINT32_VAL;

typedef union
{
	uint16_t Val;
	struct
	{
		uint8_t LB;
		uint8_t HB;	
	}byte;
}UINT16_VAL;

typedef enum _SPI_TYPE{
    SINGLE_SPI = 1,
    DUAL_SPI = 2,
    QUAD_SPI = 3,
} SPI_TYPE;

typedef enum _SET_CFG_DATA_BYTE_ORDER {
    SPI_READ_INITIAL_OFFSET = 0,
    SPI_READ_INTRA_DWORD_OFFSET,
    SPI_READ_INTER_DWORD_OFFSET,
    SPI_FASTREAD_INITIAL_OFFSET = 3,
    SPI_FASTREAD_INTRA_DWORD_OFFSET,
    SPI_FASTREAD_INTER_DWORD_OFFSET,
    SDOR_INITIAL_OFFSET = 6,
    SDOR_INTRA_DWORD_OFFSET,
    SDOR_INTER_DWORD_OFFSET,
    SDIOR_INITIAL_OFFSET = 9,
    SDIOR_INTRA_DWORD_OFFSET,
    SDIOR_INTER_DWORD_OFFSET,
    SQOR_INITIAL_OFFSET = 12,
    SQOR_INTRA_DWORD_OFFSET,
    SQOR_INTER_DWORD_OFFSET,
    SQIOR_INITIAL_OFFSET = 15,
    SQIOR_INTRA_DWORD_OFFSET,
    SQIOR_INTER_DWORD_OFFSET,
    SPI_WRITE_INITIAL_OFFSET = 18,
    SPI_WRITE_INTRA_DWORD_OFFSET,
    SPI_WRITE_INTER_DWORD_OFFSET,
    SDDW_INITIAL_OFFSET = 21,
    SDDW_INTRA_DWORD_OFFSET,
    SDDW_INTER_DWORD_OFFSET,
    SDADW_INITIAL_OFFSET = 24,
    SDADW_INTRA_DWORD_OFFSET,
    SDADW_INTER_DWORD_OFFSET,
    SQDW_INITIAL_OFFSET = 27,
    SQDW_INTRA_DWORD_OFFSET,
    SQDW_INTER_DWORD_OFFSET,
    SQADW_INITIAL_OFFSET = 30,
    SQADW_INTRA_DWORD_OFFSET,
    SQADW_INTER_DWORD_OFFSET,
    SQI_FASTREAD_INITIAL_OFFSET = 33,
    SQI_FASTREAD_INTRA_DWORD_OFFSET,
    SQI_FASTREAD_INTER_DWORD_OFFSET,
    SQI_WRITE_INITIAL_OFFSET = 36,
    SQI_WRITE_INTRA_DWORD_OFFSET,
    SQI_WRITE_INTER_DWORD_OFFSET,
} SET_CFG_DATA_BYTE_ORDER_T;

void BSP_CLK_Init(void);
void SystemClockConfig(void);

void Qspi_Init(void);

void ECAT_Lan9253_SPIWrite(uint16_t u16Addr, uint8_t *pu8Data, uint32_t u32Length);
void ECAT_Lan9253_SPIRead(uint16_t u16Addr, uint8_t *pu8Data, uint32_t u32Length);
void ECAT_Lan9253_SPIFastRead(uint16_t u16Addr, uint8_t *pu8Data, uint32_t u32Length);

void pdi_isr_Callback(void);
void sync0_isr_Callback(void);
void sync1_isr_Callback(void);

void pdi_isr_init(void);
void sync0_isr_init(void);
void sync1_isr_init(void);

void ECAT_Lan9253_IsPDIFunctional(uint8_t *pu8Data);
void ECAT_SPI_DisableQuadMode(void);
void ECAT_SPI_SetCfg_dataInit(void);
void ECAT_SPI_SetConfiguration(uint8_t *pu8DummyByteCnt);

void Lan9253_Init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __QSIP_DRV_H__ */
