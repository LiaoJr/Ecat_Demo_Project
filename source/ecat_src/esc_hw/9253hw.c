/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
\addtogroup EL9800_HW EL9800 Platform (Serial ESC Access)
@{
*/

/**
\file    el9800hw.c
\author EthercatSSC@beckhoff.com
\brief Implementation
Hardware access implementation for EL9800 onboard PIC18/PIC24 connected via SPI to ESC

\version 5.13

<br>Changes to version V5.12:<br>
V5.13 EL9800 1: update includes and configurations bits<br>
<br>Changes to version V5.11:<br>
V5.12 EL9800 1: improve the SPI access<br>
<br>Changes to version V5.10:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
V5.11 EL9800 2: change PDI access test to 32Bit ESC access and reset AL Event mask after test even if AL Event is not enabled<br>
<br>Changes to version V5.01:<br>
V5.10 ESC5: Add missing swapping<br>
V5.10 HW3: Sync1 Isr added<br>
V5.10 HW4: Add volatile directive for direct ESC DWORD/WORD/BYTE access<br>
           Add missing swapping in mcihw.c<br>
           Add "volatile" directive vor dummy variables in enable and disable SyncManger functions<br>
           Add missing swapping in EL9800hw files<br>
<br>Changes to version V5.0:<br>
V5.01 HW1: Invalid ESC access function was used<br>
<br>Changes to version V4.40:<br>
V5.0 ESC4: Save SM disable/Enable. Operation may be pending due to frame handling.<br>
<br>Changes to version V4.30:<br>
V4.40 : File renamed from spihw.c to el9800hw.c<br>
<br>Changes to version V4.20:<br>
V4.30 ESM: if mailbox Syncmanger is disabled and bMbxRunning is true the SyncManger settings need to be revalidate<br>
V4.30 EL9800: EL9800_x hardware initialization is moved to el9800.c<br>
V4.30 SYNC: change synchronisation control function. Add usage of 0x1C32:12 [SM missed counter].<br>
Calculate bus cycle time (0x1C32:02 ; 0x1C33:02) CalcSMCycleTime()<br>
V4.30 PDO: rename PDO specific functions (COE_xxMapping -> PDO_xxMapping and COE_Application -> ECAT_Application)<br>
V4.30 ESC: change requested address in GetInterruptRegister() to prevent acknowledge events.<br>
(e.g. reading an SM config register acknowledge SM change event)<br>
GENERIC: renamed several variables to identify used SPI if multiple interfaces are available<br>
V4.20 MBX 1: Add Mailbox queue support<br>
V4.20 SPI 1: include SPI RxBuffer dummy read<br>
V4.20 DC 1: Add Sync0 Handling<br>
V4.20 PIC24: Add EL9800_4 (PIC24) required source code<br>
V4.08 ECAT 3: The AlStatusCode is changed as parameter of the function AL_ControlInd<br>
<br>Changes to version V4.02:<br>
V4.03 SPI 1: In ISR_GetInterruptRegister the NOP-command should be used.<br>
<br>Changes to version V4.01:<br>
V4.02 SPI 1: In HW_OutputMapping the variable u16OldTimer shall not be set,<br>
             otherwise the watchdog might exceed too early.<br>
<br>Changes to version V4.00:<br>
V4.01 SPI 1: DI and DO were changed (DI is now an input for the uC, DO is now an output for the uC)<br>
V4.01 SPI 2: The SPI has to operate with Late-Sample = FALSE on the Eva-Board<br>
<br>Changes to version V3.20:<br>
V4.00 ECAT 1: The handling of the Sync Manager Parameter was included according to<br>
              the EtherCAT Guidelines and Protocol Enhancements Specification<br>
V4.00 APPL 1: The watchdog checking should be done by a microcontroller<br>
                 timer because the watchdog trigger of the ESC will be reset too<br>
                 if only a part of the sync manager data is written<br>
V4.00 APPL 4: The EEPROM access through the ESC is added

*/


/*--------------------------------------------------------------------------------------
------
------    Includes
------
--------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#include "ecatslv.h"


#define    _9253HW_ 1
#include "9253hw.h"
#undef    _9253HW_
/*remove definition of _EL9800HW_ (#ifdef is used in el9800hw.h)*/

#include "ecatappl.h"

typedef union
{
    unsigned short    Word;
    unsigned char    Byte[2];
} UBYTETOWORD;

typedef union 
{
    UINT8           Byte[2];
    UINT16          Word;
}
UALEVENT;

/*--------------------------------------------------------------------------------------
------
------    internal Types and Defines
------
--------------------------------------------------------------------------------------*/
#if INTERRUPTS_SUPPORTED
/*-----------------------------------------------------------------------------------------
------
------    Global Interrupt setting
------
-----------------------------------------------------------------------------------------*/
#define    DISABLE_GLOBAL_INT()             __disable_irq()			
#define    ENABLE_GLOBAL_INT()           	__enable_irq()				
#define    DISABLE_AL_EVENT_INT()           DISABLE_GLOBAL_INT()
#define    ENABLE_AL_EVENT_INT()            ENABLE_GLOBAL_INT()

/*-----------------------------------------------------------------------------------------
------
------    ESC Interrupt
------
-----------------------------------------------------------------------------------------*/
#if AL_EVENT_ENABLED
#define    INIT_ESC_INT()           pdi_isr_init()
#endif //#if AL_EVENT_ENABLED

/*-----------------------------------------------------------------------------------------
------
------    SYNC0 Interrupt
------
-----------------------------------------------------------------------------------------*/
#if DC_SUPPORTED
#define    INIT_SYNC0_INT()                 sync0_isr_init()		        //EXTI1_Configuration--sync0_isr_init
#define    DISABLE_SYNC0_INT()              NVIC_DisableIRQ(Int004_IRQn)	// {(_INT3IE)=0;}//disable interrupt source INT3
#define    ENABLE_SYNC0_INT()               NVIC_EnableIRQ(Int004_IRQn)	    // {(_INT3IE) = 1;} //enable interrupt source INT3


/*ECATCHANGE_START(V5.10) HW3*/

#define    INIT_SYNC1_INT()                   sync1_isr_init()
#define    DISABLE_SYNC1_INT()                NVIC_DisableIRQ(Int005_IRQn)  // {(_INT4IE)=0;}//disable interrupt source INT4
#define    ENABLE_SYNC1_INT()                 NVIC_EnableIRQ(Int005_IRQn)   //{(_INT4IE) = 1;} //enable interrupt source INT4


/*ECATCHANGE_END(V5.10) HW3*/

#endif //#if DC_SUPPORTED && _STM32_IO8

#endif	//#if INTERRUPTS_SUPPORTED

/*-----------------------------------------------------------------------------------------
------
------    Hardware timer
------
-----------------------------------------------------------------------------------------*/
#define INIT_ECAT_TIMER()                Timer_0_CHB_Init()

/*--------------------------------------------------------------------------------------
------
------    internal Variables
------
--------------------------------------------------------------------------------------*/
UALEVENT         EscALEvent;            //contains the content of the ALEvent register (0x220), this variable is updated on each Access to the Esc
/*--------------------------------------------------------------------------------------
------
------    internal functions
------
--------------------------------------------------------------------------------------*/

/**
 \brief  The function operates a SPI access without addressing.

        The first two bytes of an access to the EtherCAT ASIC always deliver the AL_Event register (0x220).
        It will be saved in the global "EscALEvent"
*////////////////////////////////////////////////////////////////////////////////////////
static void GetInterruptRegister(void)
{
     HW_EscReadIsr((MEM_ADDR *)&EscALEvent.Word, 0x220, 2);
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief  The function operates a SPI access without addressing.
        Shall be implemented if interrupts are supported else this function is equal to "GetInterruptRegsiter()"

        The first two bytes of an access to the EtherCAT ASIC always deliver the AL_Event register (0x220).
        It will be saved in the global "EscALEvent"
*////////////////////////////////////////////////////////////////////////////////////////
static void ISR_GetInterruptRegister(void)
{
    HW_EscReadIsr((MEM_ADDR *)&EscALEvent.Word, 0x220, 2);
}

/*--------------------------------------------------------------------------------------
------
------    exported hardware access functions
------
--------------------------------------------------------------------------------------*/


/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0 if initialization was successful

 \brief    This function intialize the Process Data Interface (PDI) and the host controller.
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 Ecat_HW_Init(void)
{
    unsigned long i;
    //UINT32 intMask;
    //UINT32 data;
	
    /* Qspi initialization */
    Qspi_Init();
	
    for(i = 0; i < 1000000; i++)
	{
    	__asm__("nop");__asm__("nop");
    	__asm__("nop");__asm__("nop");
    	__asm__("nop");__asm__("nop");
    	__asm__("nop");__asm__("nop");
    	__asm__("nop");__asm__("nop");
	}
    
//    ECAT_SPI_SetCfg_dataInit();
//    ECAT_SPI_SetConfiguration(gau8DummyCntArr);
    
    Lan9253_Init();

#if AL_EVENT_ENABLED
    /* Initiate SM-Event Interrupt Port */
    INIT_ESC_INT();
#endif

#if DC_SUPPORTED
    /* Initiate Sync0/Sync1 Event Interrupt Port */
    INIT_SYNC0_INT();
    INIT_SYNC1_INT();
#endif

    INIT_ECAT_TIMER();
    
#if INTERRUPTS_SUPPORTED
    /* enable all interrupts */
    ENABLE_GLOBAL_INT();
#endif

    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    This function shall be implemented if hardware resources need to be release
        when the sample application stops
*////////////////////////////////////////////////////////////////////////////////////////
void HW_Release(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    first two Bytes of ALEvent register (0x220)

 \brief  This function gets the current content of ALEvent register
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 HW_GetALEventRegister(void)
{
    GetInterruptRegister();
    return EscALEvent.Word;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    first two Bytes of ALEvent register (0x220)

 \brief  The SPI PDI requires an extra ESC read access functions from interrupts service routines.
        The behaviour is equal to "HW_GetALEventRegister()"
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 HW_GetALEventRegister_Isr(void)
{
     ISR_GetInterruptRegister();
    return EscALEvent.Word;
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param RunLed            desired EtherCAT Run led state
 \param ErrLed            desired EtherCAT Error led state

  \brief    This function updates the EtherCAT run and error led
*////////////////////////////////////////////////////////////////////////////////////////
void HW_SetLed(UINT8 RunLed,UINT8 ErrLed)
{

}
void EscRead(MEM_ADDR *pmData, UINT16 u16Address, UINT16 u16Len)
{
    UINT32_VAL u32Val;
    UINT8 u8ValidDataLen = 0, u8Itr = 0;
    UINT8 *pu8Data = (UINT8 *)pmData;

    while (u16Len > 0) {
        u8ValidDataLen = (u16Len > 4) ? 4 : u16Len;

        if (u16Address & 0x1) {
            u8ValidDataLen = 1;
        }
        else if (u16Address & 0x2)
        {
            u8ValidDataLen = (u8ValidDataLen >= 2 ) ? 2 : 1;
        }
        else if (u8ValidDataLen < 4)
        {
            u8ValidDataLen = (u8ValidDataLen >= 2) ? 2 : 1;
        }

        ECAT_Lan9253_SPIFastRead(u16Address, (UINT8*)&u32Val.Val, 4);

        for (u8Itr = 0; u8Itr < u8ValidDataLen; u8Itr++)
        *pu8Data++ = u32Val.v[u8Itr];

        u16Address += u8ValidDataLen;
        u16Len -= u8ValidDataLen;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves read data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

 \brief  This function operates the SPI read access to the EtherCAT ASIC.
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscRead( MEM_ADDR *pmData, UINT16 u16Address, UINT16 u16Len)
{
    UINT32_VAL u32Val;
	uint8_t u8ValidDataLen = 0, u8Itr = 0;
	uint8_t *pu8Data = (uint8_t *)pmData;

	while (u16Len > 0) {
        u8ValidDataLen = (u16Len > 4) ? 4 : u16Len;

        if (u16Address & 0x1) {
            u8ValidDataLen = 1;
        }
        else if (u16Address & 0x2)
        {
            u8ValidDataLen = (u8ValidDataLen >= 2 ) ? 2 : 1;
        }
        else if (u8ValidDataLen < 4)
        {
            u8ValidDataLen = (u8ValidDataLen >= 2) ? 2 : 1;
        }

        __disable_irq();
        ECAT_Lan9253_SPIFastRead(u16Address, (UINT8*)&u32Val.Val, 4);
        __enable_irq();

        for (u8Itr = 0; u8Itr < u8ValidDataLen; u8Itr++){
            *pu8Data++ = u32Val.v[u8Itr];
        }

        u16Address += u8ValidDataLen;
        u16Len -= u8ValidDataLen;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves read data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

\brief  The SPI PDI requires an extra ESC read access functions from interrupts service routines.
        The behaviour is equal to "HW_EscRead()"
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscReadIsr( MEM_ADDR *pmData, UINT16 u16Address, UINT16 u16Len )
{
    UINT8 *pu8Data = (UINT8 *)pmData;
  
    if(u16Address > 0xfff)
    {
      /* send the address and command to the ESC */
        ECAT_Lan9253_SPIFastRead (u16Address, pu8Data, u16Len); 	  
    }
    else
    {
        EscRead(pmData, u16Address, u16Len);
    }
}

void Esc_Write(MEM_ADDR *pmData, UINT16 u16Address, UINT16 u16Len)
{
    UINT32_VAL u32Val;
	uint8_t u8ValidDataLen = 0, u8Itr = 0;
	uint8_t *pu8Data = (uint8_t *)pmData;

	while (u16Len > 0) {
		u8ValidDataLen = (u16Len > 4) ? 4 : u16Len;
	
		if (u16Address & 0x1) {
			u8ValidDataLen = 1;
		}
		else if (u16Address & 0x2)
		{
			u8ValidDataLen = (u8ValidDataLen >= 2) ? 2 : 1;
		}
		else if (u8ValidDataLen < 4)
		{
			u8ValidDataLen = (u8ValidDataLen >= 2) ? 2 : 1;
		}

		for (u8Itr = 0; u8Itr < u8ValidDataLen; u8Itr++){
            u32Val.v[u8Itr] = *pu8Data++;
        }
        
		ECAT_Lan9253_SPIWrite(u16Address, (uint8_t*)&u32Val.Val, 4);

		u16Address += u8ValidDataLen;
		u16Len -= u8ValidDataLen;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves write data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

  \brief  This function operates the SPI write access to the EtherCAT ASIC.
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscWrite( MEM_ADDR *pmData, UINT16 u16Address, UINT16 u16Len )
{
    UINT32_VAL u32Val;
	uint8_t u8ValidDataLen = 0, u8Itr = 0;
	uint8_t *pu8Data = (uint8_t *)pmData;

	while (u16Len > 0) {
		u8ValidDataLen = (u16Len > 4) ? 4 : u16Len;
	
		if (u16Address & 0x1) {
			u8ValidDataLen = 1;
		}
		else if (u16Address & 0x2)
		{
			u8ValidDataLen = (u8ValidDataLen >= 2) ? 2 : 1;
		}
		else if (u8ValidDataLen < 4)
		{
			u8ValidDataLen = (u8ValidDataLen >= 2) ? 2 : 1;
		}

		for (u8Itr = 0; u8Itr < u8ValidDataLen; u8Itr++){
            u32Val.v[u8Itr] = *pu8Data++;
        }
	
		__disable_irq();
		ECAT_Lan9253_SPIWrite(u16Address, (uint8_t*)&u32Val.Val, 4);
		__enable_irq();

		u16Address += u8ValidDataLen;
		u16Len -= u8ValidDataLen;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves write data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

 \brief  The SPI PDI requires an extra ESC write access functions from interrupts service routines.
        The behaviour is equal to "HW_EscWrite()"
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscWriteIsr( MEM_ADDR *pmData, UINT16 u16Address, UINT16 u16Len)
{
    //UINT16 i ;
    UINT8 *pTmpData = (UINT8 *)pmData;
    
    if(u16Address > 0xfff){
        ECAT_Lan9253_SPIWrite(u16Address, pTmpData, u16Len);
    }else{
        Esc_Write(pmData, u16Address, u16Len);
    }
}


/** @} */

