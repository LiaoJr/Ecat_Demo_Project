/*
 * Describe: null
 * Author:   wangqingsheng
 * Date:     2023-09-17
 */

#ifndef __TIMER0_DRV_H__
#define __TIMER0_DRV_H__
#include "hc32_ddl.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TMR_UNIT            (M4_TMR01)
#define TMR_INI_GCMA        (INT_TMR01_GCMA)
#define TMR_INI_GCMB        (INT_TMR01_GCMB)

#define ENABLE_TMR0()      (PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM01, Enable))
void Timer_0_CHB_Init(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __TIMER0_DRV_H__ */
