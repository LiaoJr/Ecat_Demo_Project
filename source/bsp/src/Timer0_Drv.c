/*
 * Describe: null
 * Author:   wangqingsheng
 * Date:     2024-01-25
 */
#include "hc32_ddl.h"
#include "Timer0_Drv.h"
//#include "Adc_8Hall_Drv.h"
#include "9253hw.h"
#include "ecatappl.h"

uint32_t ecat_ticks = 0;

//extern uint32_t Fan_speed_Timecnt;

//static void All_1ms_timccnt_add(void){
//	ecat_ticks ++;
//	Fan_speed_Timecnt ++;
//}

void Timer0B_CallBack(void)
{
	DISABLE_ESC_INT();

    ECAT_CheckTimer();
	
    //All_1ms_timccnt_add();
	
	ENABLE_ESC_INT();
}

void Timer_0_CHB_Init()
{
    stc_tim0_base_init_t stcTimerCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    uint32_t u32Pclk1;
    stc_clk_freq_t stcClkTmp;
    
    MEM_ZERO_STRUCT(stcTimerCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    
    /* Get pclk1 */
    CLK_GetClockFreq(&stcClkTmp);
    u32Pclk1 = stcClkTmp.pclk1Freq;

    /* Timer0 peripheral enable */
    ENABLE_TMR0();

    /*config register for channel B */
    stcTimerCfg.Tim0_CounterMode = Tim0_Sync;
    stcTimerCfg.Tim0_SyncClockSource = Tim0_Pclk1;
    stcTimerCfg.Tim0_ClockDivision = Tim0_ClkDiv1024;
    stcTimerCfg.Tim0_CmpValue = (uint16_t)(u32Pclk1/1024ul/1000ul - 1ul);
    TIMER0_BaseInit(TMR_UNIT,Tim0_ChannelB,&stcTimerCfg);

    /* Enable channel B interrupt */
    TIMER0_IntCmd(TMR_UNIT,Tim0_ChannelB,Enable);
    /* Register TMR_INI_GCMB Int to Vect.No.010 */
    stcIrqRegiConf.enIRQn = Int010_IRQn;
    /* Select I2C Error or Event interrupt function */
    stcIrqRegiConf.enIntSrc = TMR_INI_GCMB;
    /* Callback function */
    stcIrqRegiConf.pfnCallback = &Timer0B_CallBack;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);
    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_12);
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
	
    /*start timer0*/
    TIMER0_Cmd(TMR_UNIT,Tim0_ChannelB,Enable);
}
