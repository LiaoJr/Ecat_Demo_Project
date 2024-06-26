/*******************************************************************************
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by HDSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 */
/******************************************************************************/
/** \file main.c
 **
 ** \brief Template sample
 **
 **   - 2021-04-16 CDT First version for Device Driver Library of template.
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"
#include "Demo.h"
#include "applInterface.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void Ecat_Appl_Init(void)
{
	MainInit();
	
#if FOE_SUPPORTED
    u32FileSize = 0;
#endif
	
	/*Create basic mapping*/
    APPL_GenerateMapping(&nPdInputSize,&nPdOutputSize);
}

/**
 *******************************************************************************
 ** \brief  Main function of template project
 **
 ** \param  None
 **
 ** \retval int32_t return value, if needed
 **
 ******************************************************************************/
int32_t main(void)
{
	/* board_hw_init(); */
	
	/* EtherCAT hardware initiate */
	Ecat_HW_Init();
	
	/* Bootloader_Init(); */
	
	Ecat_Appl_Init();
	
    /* add your code here */
    while (1){
		/* EtherCAT Slave main loop */
        MainLoop();
    }
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
