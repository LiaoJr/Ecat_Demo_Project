/*
 * Describe: QSPI读写接口
 * Author:   wangqingsheng
 * Date:     2024-03-14
 */

#include "Qspi_Drv.h"
#include "9253hw.h"
#include "ecatappl.h"

uint8_t gau8DmaBuff[256];
uint8_t gau8DummyCntArr[39] = {0,0,0,1,0,0,1,0,0,2,0,0,1,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,1,0,0};

/*
 * Make the u16X as multiple of 2 or 4 or 8, based on u16Type
 */
static uint16_t RoundUpMultipleOf (uint16_t u16X, uint16_t u16Type)
{
    uint16_t u16Val = 0;
    
    switch (u16Type)
    {
        case SINGLE_SPI:
            /* Single Bit SPI, Round up multiple of 8 */
            u16Val = (u16X % 8) ? (u16X + (8 - (u16X % 8))) : u16X; 
            break;
        case DUAL_SPI:
            /* Dual SPI, Round up multiple of 4 */
            u16Val = (u16X % 4) ? (u16X + (4 - (u16X % 4))) : u16X;
            break;
        case QUAD_SPI:
            /* QUAD SPI, Round up multiple of 2 */
            u16Val = (u16X % 2) ? (u16X + (2 - (u16X % 2))) : u16X;
            break;
        default:
            /* Invalid SPI type */
            u16Val = 1;
            break;
    }
	return u16Val;
}

uint8_t GetDummyBytesRequired (uint8_t u8SPIType, uint16_t u16IAT, uint16_t u16SPIClkCount, uint16_t u16SPIClkPeriodns)
{
	uint16_t u16DummyClkCount = 0;
    uint8_t u8DummyByte = 0;
	uint16_t u16SPIClkTime = 0, u16DummyTime = 0;
    
    /* SPI Clock time */
    u16SPIClkTime = u16SPIClkCount * u16SPIClkPeriodns;
	if (u16SPIClkTime >= u16IAT)
	{
		return 0;
	}
	/* Dummy Time */
	u16DummyTime = (u16IAT - u16SPIClkTime);

	// Express the Dummy time in terms of number of SPI clocks
    if (u16DummyTime <= 0) {
        u16DummyClkCount = 0;
    } else {
        u16DummyClkCount = u16DummyTime / u16SPIClkPeriodns;
        /* Getting the fractional part */
        if (u16DummyTime % u16SPIClkPeriodns) {
            /* Adding one clock count
             * So that fractional part will be considered
             * round of multiple value gets proper value
             */
            u16DummyClkCount++;
        }
    }

    /*
     * Make dummy clock count as Byte/WORD aligned
     * if (SPIType == QUAD_SPI), then (round to next multiple of 2)
     * if (SPIType == DUAL_SPI), then (round to next multiple of 4)
     * if (SPIType == SINGLE_SPI), then (round to next multiple of 8) 
     */
	u16DummyClkCount = RoundUpMultipleOf (u16DummyClkCount, u8SPIType);
    
    /* Convert the clock count to dummy byte */
    if (u8SPIType == QUAD_SPI) {
        u8DummyByte = u16DummyClkCount >> 1;
    } else if (u8SPIType == DUAL_SPI) {
        u8DummyByte = u16DummyClkCount >> 2;
    } else if (u8SPIType == SINGLE_SPI) {
        u8DummyByte = u16DummyClkCount >> 3;
    }
    return u8DummyByte;
}

void ECAT_SPI_SetCfg_dataInit(void)
{
    uint16_t u16SPIClkPeriod = 0, u16SPIClkfreq = 0;
    
    u16SPIClkfreq = DRV_LAN9253_BAUDRATE_PDI_FREQ;
    /* Get clock period in Nano seconds */
    u16SPIClkPeriod =  CLK_PERIOD_1MHZ / u16SPIClkfreq;
    
    /* Fill the SPI related part in gau8DummyCntArr */
    /* SPI read, index from 0, 1, 2 
     * as initial, intra DWORD  inter DWORD
     */
    gau8DummyCntArr[SPI_READ_INITIAL_OFFSET] = GetDummyBytesRequired (SINGLE_SPI, IAT_BYRD, 0, u16SPIClkPeriod);
    gau8DummyCntArr[SPI_READ_INTRA_DWORD_OFFSET] = GetDummyBytesRequired (SINGLE_SPI, IAT_BYRD, 6, u16SPIClkPeriod);
    gau8DummyCntArr[SPI_READ_INTER_DWORD_OFFSET] = GetDummyBytesRequired (SINGLE_SPI, IAT_BYRD, 6, u16SPIClkPeriod);
    
    /* SPI Fast read, index from 3, 4, 5 
     * as initial, intra DWORD  inter DWORD
     */
    gau8DummyCntArr[SPI_FASTREAD_INITIAL_OFFSET] = GetDummyBytesRequired (SINGLE_SPI, IAT_DWRD, 0, u16SPIClkPeriod);
    gau8DummyCntArr[SPI_FASTREAD_INTRA_DWORD_OFFSET] = GetDummyBytesRequired (SINGLE_SPI, IAT_DWRD, 30, u16SPIClkPeriod);
    gau8DummyCntArr[SPI_FASTREAD_INTER_DWORD_OFFSET] = GetDummyBytesRequired (SINGLE_SPI, IAT_DWRD, 30, u16SPIClkPeriod);
    /* SPI Write, index starts from 18, 19, 20  
     * as Initial, Intra DWORD  Inter DWORD
     */
    gau8DummyCntArr[SPI_WRITE_INITIAL_OFFSET] = GetDummyBytesRequired (SINGLE_SPI, IAT_NULL, 0, u16SPIClkPeriod);
    gau8DummyCntArr[SPI_WRITE_INTRA_DWORD_OFFSET] = GetDummyBytesRequired (SINGLE_SPI, IAT_BYWR, 8, u16SPIClkPeriod);
    gau8DummyCntArr[SPI_WRITE_INTER_DWORD_OFFSET] = GetDummyBytesRequired (SINGLE_SPI, IAT_DWDWR, 32, u16SPIClkPeriod);

}

void ECAT_SPI_SetConfiguration(uint8_t *pu8DummyByteCnt)
{
    uint8_t u8Len = 0, u8txData = 0;
    //uint8_t u8txLen = 1;
    u8Len = 39;
    u8txData = CMD_SPI_SETCFG;
    
    stc_qspi_comm_protocol_t stcQspiCommProtocol;
    /* configure structure initialization */
    MEM_ZERO_STRUCT(stcQspiCommProtocol);
    
    stcQspiCommProtocol.enReadMode = QspiReadModeStandard;
    QSPI_CommProtocolConfig(&stcQspiCommProtocol);
    
    QSPI_EnterDirectCommMode();
    
    QSPI_WriteDirectCommValue(u8txData);
    
    /* Now write set cfg data */
    for(int i = 0; i < u8Len; i++){
        QSPI_WriteDirectCommValue(*pu8DummyByteCnt);
        pu8DummyByteCnt++;
    }
    
    QSPI_ExitDirectCommMode();
}

void pdi_isr_Callback(void)
{
	//PORT_SetBits(PortB,Pin08);
    if (Set == EXINT_IrqFlgGet(ExtiCh06))
    {
		//PORT_SetBits(PortB,Pin08);
        PDI_Isr();
		//PORT_ResetBits(PortB,Pin08);
        /* clear int request flag */
        EXINT_IrqFlgClr(ExtiCh06);
    }
	//PORT_ResetBits(PortB,Pin08);
}

void sync0_isr_Callback(void)
{   
	//PORT_SetBits(PortB,Pin09);
    if (Set == EXINT_IrqFlgGet(ExtiCh00))
    {
        Sync0_Isr();
        /* clear int request flag */
        EXINT_IrqFlgClr(ExtiCh00);
    }
	//PORT_ResetBits(PortB,Pin09);
}

void sync1_isr_Callback(void)
{
    if (Set == EXINT_IrqFlgGet(ExtiCh07))
    {
        Sync1_Isr();
        /* clear int request flag */
        EXINT_IrqFlgClr(ExtiCh07);
    }
}

void pdi_isr_init(void)
{
    stc_exint_config_t stcExtiConfig;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcExtiConfig);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(stcPortInit);

    stcExtiConfig.enExitCh = ExtiCh06;
    stcExtiConfig.enFilterEn = Enable;
    stcExtiConfig.enFltClk = Pclk3Div8;
    stcExtiConfig.enExtiLvl = ExIntFallingEdge;
    EXINT_Init(&stcExtiConfig);

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enExInt = Enable;
    PORT_Init(PortA, Pin06, &stcPortInit);

    stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ6;
    stcIrqRegiConf.enIRQn = Int003_IRQn;
    stcIrqRegiConf.pfnCallback = &pdi_isr_Callback;

    enIrqRegistration(&stcIrqRegiConf);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_13);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);    
}

void sync0_isr_init(void)
{
    stc_exint_config_t stcExtiConfig;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcExtiConfig);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(stcPortInit);

    stcExtiConfig.enExitCh = ExtiCh00;
    stcExtiConfig.enFilterEn = Enable;
    stcExtiConfig.enFltClk = Pclk3Div1;
    stcExtiConfig.enExtiLvl = ExIntFallingEdge;
    EXINT_Init(&stcExtiConfig);

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enExInt = Enable;
    PORT_Init(PortB, Pin00, &stcPortInit);

    stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ0;
    stcIrqRegiConf.enIRQn = Int004_IRQn;
    stcIrqRegiConf.pfnCallback = &sync0_isr_Callback;

    enIrqRegistration(&stcIrqRegiConf);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_14);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn); 
}

void sync1_isr_init(void)
{
    stc_exint_config_t stcExtiConfig;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcExtiConfig);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(stcPortInit);

    stcExtiConfig.enExitCh = ExtiCh07;
    stcExtiConfig.enFilterEn = Enable;
    stcExtiConfig.enFltClk = Pclk3Div8;
    stcExtiConfig.enExtiLvl = ExIntFallingEdge;
    EXINT_Init(&stcExtiConfig);

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enExInt = Enable;
    PORT_Init(PortA, Pin07, &stcPortInit);

    stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ7;
    stcIrqRegiConf.enIRQn = Int005_IRQn;
    stcIrqRegiConf.pfnCallback = &sync1_isr_Callback;

    enIrqRegistration(&stcIrqRegiConf);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_14);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn); 
}

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 *******************************************************************************
 ** \brief QSPI flash init function
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
void Qspi_Init(void)
{
    stc_qspi_init_t stcQspiInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcQspiInit);

    /* Configuration peripheral clock */
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_QSPI, Enable);

    /* Configuration QSPI pin */
    PORT_SetFunc(QSPCK_PORT, QSPCK_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSNSS_PORT, QSNSS_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSIO0_PORT, QSIO0_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSIO1_PORT, QSIO1_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSIO2_PORT, QSIO2_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSIO3_PORT, QSIO3_PIN, Func_Qspi, Disable);

    /* Configuration QSPI structure */
    stcQspiInit.enClkDiv = QspiHclkDiv3;
    stcQspiInit.enSpiMode = QspiSpiMode3;
    stcQspiInit.enBusCommMode = QspiBusModeDirectAccess;
    stcQspiInit.enPrefetchMode = QspiPrefetchStopComplete;
    stcQspiInit.enPrefetchFuncEn = Disable;
    stcQspiInit.enQssnValidExtendTime = QspiQssnValidExtendSck32;
    stcQspiInit.enQssnIntervalTime = QspiQssnIntervalQsck8;
    stcQspiInit.enQsckDutyCorr = QspiQsckDutyCorrHalfHclk;
    stcQspiInit.enVirtualPeriod = QspiVirtualPeriodQsck16;
    stcQspiInit.enWpPinLevel = QspiWpPinOutputLow;
    stcQspiInit.enQssnSetupDelayTime = QspiQssnSetupDelay1Dot5Qsck;
    stcQspiInit.enQssnHoldDelayTime = QspiQssnHoldDelay1Dot5Qsck;
    stcQspiInit.enFourByteAddrReadEn = Disable;
    stcQspiInit.enAddrWidth = QspiAddressByteTwo;
    stcQspiInit.stcCommProtocol.enReadMode = QspiReadModeFourWiresIO;
    stcQspiInit.stcCommProtocol.enTransInstrProtocol = QspiProtocolExtendSpi;
    stcQspiInit.stcCommProtocol.enTransAddrProtocol = QspiProtocolExtendSpi;
    stcQspiInit.stcCommProtocol.enReceProtocol = QspiProtocolExtendSpi;
    stcQspiInit.u8RomAccessInstr = QSPI_3BINSTR_FOUR_WIRES_IO_READ;
    QSPI_Init(&stcQspiInit);
}

void ECAT_Lan9253_SPIWrite(uint16_t u16Addr, uint8_t *pu8Data, uint32_t u32Length)
{
	uint32_t    dwModLen = 0;
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE
	uint8_t 	u8dummy_clk_cnt = 0;
#endif 
    uint8_t  u8TxData[4]={0,0,0,0}; 
    //uint8_t  u8RxData[4]={0,0,0,0};
	//uint8_t  u8RxLen = 1;
    uint8_t  u8TxLen = 1;
    uint8_t	 u8TxOneByteData=0;
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE
    //uint8_t  u8RxOneByteData=0;
#endif
    
    stc_qspi_comm_protocol_t stcQspiCommProtocol;
    /* configure structure initialization */
    MEM_ZERO_STRUCT(stcQspiCommProtocol);    
    stcQspiCommProtocol.enReadMode = QspiReadModeStandard;
    QSPI_CommProtocolConfig(&stcQspiCommProtocol);
    
    /* Core CSR and Process RAM accesses can have any alignment and length */
	if (u16Addr < 0x3000)
	{
		/* DWORD Buffering - Applicable for Write only */
		if (u32Length > 1)
		{
			u16Addr |= 0xC000; 	/* addr[15:14]=11 */
        }
		else
		{
			/* Do Nothing */
		}
	}
	else
	{
		/* Non Core CSR length will be adjusted if it is not DWORD aligned */
		dwModLen = u32Length % 4;
		if (1 == dwModLen)
		{
			u32Length = u32Length + 3;
		}
		else if (2 == dwModLen)
		{
			u32Length = u32Length + 2;
		}
		else if (3 == dwModLen)
		{
			u32Length = u32Length + 1;
		}
		else
		{
			/* Do nothing if length is 0 since it is DWORD access */
		}
	}
    
    /* SPI read and write with 3 bytes of TX and RX length */
    //u8RxLen = 3;
	u8TxLen = 3;
    u8TxData[0] = CMD_SERIAL_WRITE;
    u8TxData[1] = (uint8_t)(u16Addr >> 8);
    u8TxData[2] = (uint8_t)u16Addr;
	    
    QSPI_EnterDirectCommMode();
    
    for(int i = 0; i < u8TxLen; i++){
        QSPI_WriteDirectCommValue(u8TxData[i]);
    }
//    for(int j = 0; j < u8RxLen; j++){
//        u8RxData[j] = QSPI_ReadDirectCommValue();
//    }
//    
//    // Update the gEscALEvent value which is required for the state change
//    EscALEvent.Byte[0] = u8RxData[1];
//    EscALEvent.Byte[1] = u8RxData[2];    
        
	/* SPI read and write with 1 bytes of TX and RX length */
    //u8RxLen = 1;
	u8TxLen = 1;
    	
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE 	
	/* Get the Intra DWORD dummy clock count */
	u8dummy_clk_cnt = gau8DummyCntArr[SPI_WRITE_INITIAL_OFFSET];
	/* Add Intra DWORD dummy clocks, avoid for last byte */
	while(u8dummy_clk_cnt--)
	{   
        //u8RxOneByteData = QSPI_ReadDirectCommValue();
	}
#endif
	
    do
	{
        u8TxOneByteData = *pu8Data++;
        QSPI_WriteDirectCommValue(u8TxOneByteData);
	
#ifdef ETHERCAT_SUPPORT_DUMMY_CYCLE  		
		/* Get the Intra DWORD dummy clock count */
        u8dummy_clk_cnt = 0;//gau8DummyCntArr[SPI_WRITE_INTRA_DWORD_OFFSET];
        /* Add Intra DWORD dummy clocks, avoid for last byte */
        if (1 != u32Length) 
		{
            while(u8dummy_clk_cnt--)
			{
                u8RxOneByteData = QSPI_ReadDirectCommValue();
            }
        }
#endif        
	} while(--u32Length);   
    
    QSPI_ExitDirectCommMode();
}

void ECAT_Lan9253_SPIRead(uint16_t u16Addr, uint8_t *pu8Data, uint32_t u32Length)
{	
	uint32_t    dwModLen = 0;
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE
	uint8_t 	u8dummy_clk_cnt = 0;
#endif
    
    uint8_t  u8TxData[4]={0,0,0,0}; 
    //uint8_t  u8RxData[4]={0,0,0,0};
	//uint8_t  u8RxLen = 1;
    uint8_t  u8TxLen = 1;
    //uint8_t	 u8TxOneByteData=0;
    uint8_t  u8RxOneByteData=0;  
    
    stc_qspi_comm_protocol_t stcQspiCommProtocol;
    /* configure structure initialization */
    MEM_ZERO_STRUCT(stcQspiCommProtocol);
    stcQspiCommProtocol.enReadMode = QspiReadModeStandard;
    QSPI_CommProtocolConfig(&stcQspiCommProtocol);
    
	/* Core CSR and Process RAM accesses can have any alignment and length */
	if (u16Addr < 0x3000)
	{
		/* DWORD Buffering - Applicable for Write only */
		if (u32Length > 1)
		{
			u16Addr |= 0xC000; 	/* addr[15:14]=11 */
        }
		else
		{
			/* Do Nothing */
		}
	}
	else
	{
		/* Non Core CSR length will be adjusted if it is not DWORD aligned */
		dwModLen = u32Length % 4;
		if (1 == dwModLen)
		{
			u32Length = u32Length + 3;
		}
		else if (2 == dwModLen)
		{
			u32Length = u32Length + 2;
		}
		else if (3 == dwModLen)
		{
			u32Length = u32Length + 1;
		}
		else
		{
			/* Do nothing if length is 0 since it is DWORD access */
		}
	}    
    
    /* SPI read and write with 3 bytes of TX and RX length */
	u8TxLen = 3;
    //u8RxLen = 3;
    u8TxData[0] = CMD_SERIAL_READ;
    u8TxData[1] = (uint8_t)(u16Addr >> 8);
    u8TxData[2] = (uint8_t)u16Addr;
    
    QSPI_EnterDirectCommMode();
    
    for(int i = 0; i < u8TxLen; i++){
        QSPI_WriteDirectCommValue(u8TxData[i]);
    }
//    for(int j = 0; j < u8RxLen; j++){
//        u8RxData[j] = QSPI_ReadDirectCommValue();
//    }
//    
//    // Update the gEscALEvent value which is required for the state change
//    EscALEvent.Byte[0] = u8RxData[1];
//    EscALEvent.Byte[1] = u8RxData[2];
    
    /* SPI read and write with 1 bytes of TX and RX length */
	u8TxLen = 1;
    //u8RxLen = 1;
    
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE  	
	/* Initial Dummy cycle added by dummy read */
	u8dummy_clk_cnt = gau8DummyCntArr[SPI_READ_INITIAL_OFFSET];
	while(u8dummy_clk_cnt--)
	{   
        u8RxOneByteData = QSPI_ReadDirectCommValue();
	}
#else
    u8RxOneByteData = QSPI_ReadDirectCommValue();
#endif
    
    do
	{
        if(1 == u32Length)
		{
			//u8TxOneByteData = READ_TERMINATION_BYTE;
		}
		else
		{
			//u8TxOneByteData = 0;
		}
        
        //QSPI_WriteDirectCommValue(u8TxOneByteData);
        //u8RxOneByteData = QSPI_ReadDirectCommValue();

		*pu8Data++ = u8RxOneByteData;		
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE 		
		/* Get the Intra DWORD dummy clock count */
        u8dummy_clk_cnt = gau8DummyCntArr[SPI_READ_INTRA_DWORD_OFFSET];
        /* Add Intra DWORD dummy clocks, avoid for last byte */
        if(1 != u32Length) 
		{
            while(u8dummy_clk_cnt--)
			{
				u8RxOneByteData = QSPI_ReadDirectCommValue();
            }
        }
#endif		
	} while(--u32Length);    
    
    QSPI_ExitDirectCommMode();
    
}

void ECAT_Lan9253_SPIFastRead(uint16_t u16Addr, uint8_t *pu8Data, uint32_t u32Length)
{ 		
	uint8_t u8StartAlignSize = 0, u8Itr;
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE	
	uint8_t u8dummy_clk_cnt=0;
#endif	
    uint16_t wXfrLen = 0;
	uint32_t dwModLen = 0;
    uint8_t  u8TxData[4]={0,0,0,0};
    //uint8_t  u8RxData[4]={0,0,0,0};
	uint8_t  u8TxLen = 1;
    //uint8_t  u8RxLen = 1;
    
	uint8_t	 u8TxOneByteData=0, u8RxOneByteData=0;
	
    stc_qspi_comm_protocol_t stcQspiCommProtocol;
    /* configure structure initialization */
    MEM_ZERO_STRUCT(stcQspiCommProtocol);
    stcQspiCommProtocol.enReadMode = QspiReadModeStandard;
    QSPI_CommProtocolConfig(&stcQspiCommProtocol);
    
	/* Core CSR and Process RAM accesses can have any alignment and length */
	if (u16Addr < 0x3000)
	{
		/* Use Auto-increment for incrementing byte address*/
		u16Addr |= 0x4000;			
		
		/* To calculate initial number of dummy bytes which is based on starting address */
		u8StartAlignSize = (u16Addr & 0x3); 
	}
	else 
	{  	/* System CSRs are DWORD aligned and are a DWORD in length. Non- DWORD aligned / non-DWORD length access 
	is not supported. */
		dwModLen = u32Length % 4;
		if(1 == dwModLen)
		{
			u32Length = u32Length + 3;
		}
		else if(2 == dwModLen)
		{
			u32Length = u32Length + 2;
		}
		else if(3 == dwModLen)
		{
			u32Length = u32Length + 1;
		}
		else
		{
			/* Do nothing is length is 0 */
		}		
	}
    /* From DOS, "For the one byte transfer length format,	bit 7 is low and bits 6-0 specify the 
	length up to 127 bytes. For the two byte transfer length format, bit 7 of the first byte
	is high and bits 6-0 specify the lower 7 bits of the length. Bits 6-0 of the of the second byte 
	field specify the upper 7 bits of the length with a maximum transfer length of 16,383 bytes (16K-1)" */ 
	if(u32Length <= ONE_BYTE_MAX_XFR_LEN)
	{
		wXfrLen = u32Length; 
	}
	else  
	{
		wXfrLen = (u32Length & 0x7F) | 0x80;
		wXfrLen |= ((u32Length & 0x3F80) << 1);
	}
        /* SPI read and write with 3 bytes of TX and RX length */
	u8TxLen = 3;
    //u8RxLen = 3;
	
    u8TxData[0] = CMD_FAST_READ;
    u8TxData[1] = (uint8_t)(u16Addr >> 8);
    u8TxData[2] = (uint8_t)u16Addr;
    
    QSPI_EnterDirectCommMode();
    
    for(int i = 0; i < u8TxLen; i++){
        QSPI_WriteDirectCommValue(u8TxData[i]);
    }
//    for(int j = 0; j < u8RxLen; j++){
//        u8RxData[j] = QSPI_ReadDirectCommValue();
//    }
//    
//    EscALEvent.Byte[0] = u8RxData[1];
//    EscALEvent.Byte[1] = u8RxData[2];
    
	/* SPI read and write with 1 bytes of TX and RX length */
	u8TxLen = 1;
    //u8RxLen = 1;
	
	/* Send Transfer length */
    u8TxOneByteData = (uint8_t)(LOBYTE(wXfrLen));
    QSPI_WriteDirectCommValue(u8TxOneByteData);
    
    if (u32Length > ONE_BYTE_MAX_XFR_LEN)    /* Two byte Xfr length */
    {
        u8TxOneByteData = (uint8_t)(HIBYTE(wXfrLen));
        QSPI_WriteDirectCommValue(u8TxOneByteData);
	}
	
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE	
	/* Initial Dummy cycle added by dummy read */
	u8dummy_clk_cnt = gau8DummyCntArr[SPI_FASTREAD_INITIAL_OFFSET];
	while (u8dummy_clk_cnt--)
	{
       u8RxOneByteData = QSPI_ReadDirectCommValue();
	}
#else
	 u8RxOneByteData = QSPI_ReadDirectCommValue();
#endif	
	/* 1 default dummy + extra dummies based on address that needs to be accessed. 
	   "For Fast reads with Non DWORD aligned address, Dummy data will be sent 
       before the actual data. 
	   So to read 2001 you will get a dummy byte and then data in address 2001. 
	   SW needs to handle dummy data in case of non DWORD address reads" */
	for (u8Itr = 0; u8Itr < u8StartAlignSize; u8Itr++) 
	{
        u8TxOneByteData = QSPI_ReadDirectCommValue();
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE	
		/* Initial Dummy cycle added by dummy read */
		u8dummy_clk_cnt = gau8DummyCntArr[SPI_FASTREAD_INTRA_DWORD_OFFSET];
		while (u8dummy_clk_cnt--)
		{
			u8RxOneByteData = QSPI_ReadDirectCommValue();
		}
#endif
	}
    
    do
	{
        //QSPI_WriteDirectCommValue(u8TxOneByteData);
        u8RxOneByteData = QSPI_ReadDirectCommValue();
       
		*pu8Data++ = u8RxOneByteData;
		/* Poll for wait ack or add dummy after each byte if needed  (based on SETCFG) */
		
#ifdef ETHERCAT_IS_SUPPORT_DUMMY_CYCLE			
		/* Get the Intra DWORD dummy clock count */
        u8dummy_clk_cnt = gau8DummyCntArr[SPI_FASTREAD_INTRA_DWORD_OFFSET];
        /* Add Intra DWORD dummy clocks, avoid for last byte */
        if (1 != wXfrLen) 
		{
            while (u8dummy_clk_cnt--)
			{
                u8RxOneByteData = QSPI_ReadDirectCommValue();
            }
        }
#endif		
	} while (--wXfrLen);
    
    QSPI_ExitDirectCommMode();
}

void ECAT_Lan9253_IsPDIFunctional(uint8_t *pu8Data)
{
	/* Before device initialization, the SPI/SQI interface will not return valid data.
    To determine when the SPI/SQI interface is functional, the Byte Order Test 
    Register (BYTE_TEST) should be polled. Once the correct pattern is read, the interface
	can be considered functional */
    uint8_t  u8Len = 4;
    uint8_t  u8TxData[4]={0,0,0,0};
    uint8_t  u8RxData[4]={0,0,0,0};
    uint8_t  u8TxLen=0;
    //uint8_t  u8RxLen=0;
    
    stc_qspi_comm_protocol_t stcQspiCommProtocol;
    /* configure structure initialization */
    MEM_ZERO_STRUCT(stcQspiCommProtocol);
    stcQspiCommProtocol.enReadMode = QspiReadModeStandard;
    QSPI_CommProtocolConfig(&stcQspiCommProtocol);
    
    u8TxLen=3;
    //u8RxLen=3;
    
	u8TxData[0] = CMD_SERIAL_READ;
    u8TxData[1] = (uint8_t)HIBYTE(0x3064);
    u8TxData[2] = (uint8_t)LOBYTE(0x3064);
    
    QSPI_EnterDirectCommMode();
    for(int i = 0; i < u8TxLen; i++){
        QSPI_WriteDirectCommValue(u8TxData[i]);
    }
//    for(int j = 0; j < u8RxLen; j++){
//        u8RxData[j] = QSPI_ReadDirectCommValue();
//    }
// 
//    // Update the EscALEvent value which is required for the state change
//    EscALEvent.Byte[0] = u8RxData[1];
//    EscALEvent.Byte[1] = u8RxData[2];
    
    for(int i = 0; i < u8Len ; i++){
      u8RxData[i] = QSPI_ReadDirectCommValue();  
    }
    
	memcpy(pu8Data,u8RxData,u8Len);	
    
    QSPI_ExitDirectCommMode();
}

void ECAT_SPI_DisableQuadMode(void)
{
    uint8_t u8txData = 0;
    //uint8_t u8txLen = 1;
    
    stc_qspi_comm_protocol_t stcQspiCommProtocol;
    /* configure structure initialization */
    MEM_ZERO_STRUCT(stcQspiCommProtocol);
    stcQspiCommProtocol.enReadMode = QspiReadModeStandard;
    QSPI_CommProtocolConfig(&stcQspiCommProtocol);
    
    u8txData = 0xFF;
    
	QSPI_EnterDirectCommMode();

    QSPI_WriteDirectCommValue(u8txData);

    QSPI_ExitDirectCommMode();
}

void Lan9253_Init(void)
{
   	UINT32 u32intMask;
	UINT32 u32data;
    
    do
	{
        ECAT_Lan9253_IsPDIFunctional((uint8_t *) &u32data);
        //ECAT_Lan9253_SPIRead(0x3064, (UINT8*)&u32data, 4);
//        if( u32data == 0xFFFFFFFF){
//            ECAT_SPI_DisableQuadMode();
//        }
	} while (0x87654321 != u32data);
    
    
    // Disable interrupt Interrupt Enable register -->
	// Write 0x305c - 0x00000001
	u32data = 0x00000000;
	ECAT_Lan9253_SPIWrite(0x305C, (UINT8*)&u32data, 4);

//    do {
//		u32intMask = 0x21;
//		HW_EscWriteDWord(u32intMask, ESC_AL_STATUS_OFFSET);

//		u32intMask = 0;
//		HW_EscReadDWord(u32intMask, ESC_AL_STATUS_OFFSET);
//	} while (u32intMask != 0x21);  
	
	do {
		u32intMask = 0x93;
		HW_EscWriteDWord(u32intMask, ESC_AL_EVENTMASK_OFFSET);

		u32intMask = 0;
		HW_EscReadDWord(u32intMask, ESC_AL_EVENTMASK_OFFSET);
	} while (u32intMask != 0x93);
	
	/* Read 0x150 register to check if AL Event output is enabled */
	u32intMask = 0;
	HW_EscReadDWord(u32intMask, ESC_PDI_CONFIGURATION);

//	if (u32intMask & 0x04)
//	{
//		u32intMask = 0;
//	}
	
	u32intMask = 1;
	HW_EscWriteDWord(u32intMask, ESC_AL_EVENTMASK_OFFSET);
	
	HW_EscReadDWord(u32intMask, ESC_AL_EVENTMASK_OFFSET);
	
	u32data = 0x00000101;
	ECAT_Lan9253_SPIWrite(0x3054, (UINT8*)&u32data, 4);
	
	u32data = 0x00000001;
	ECAT_Lan9253_SPIWrite(0x305C, (UINT8*)&u32data, 4);

	ECAT_Lan9253_SPIFastRead(0x3058, (UINT8*)&u32data, 4);
}

