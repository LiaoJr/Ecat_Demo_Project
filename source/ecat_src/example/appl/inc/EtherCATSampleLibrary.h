/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/*-----------------------------------------------------------------------------------------
------
------    Description
------
------    EtherCATSampleLibrary.h
------
------ 
-----------------------------------------------------------------------------------------*/

#include <windows.h>

#ifdef __cplusplus
#ifdef IMPL_OR_EXTERN
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT extern "C" __declspec (dllimport)
#endif
#else
#ifdef IMPL_OR_EXTERN
#define EXPORT __declspec (dllexport)
#else
#define EXPORT __declspec (dllimport)
#endif
#endif


typedef struct
{
    unsigned long * pInput;
    unsigned long * pOutput;
    
    void (* pApplication)(void);
    void (* pStateTrans)(unsigned short transind);
}
ECAT_SLAVE_INTERFACE, *pECAT_SLAVE_INTERFACE;


EXPORT pECAT_SLAVE_INTERFACE __stdcall Ecat_Open(unsigned short InputSize, unsigned short OutputSize);

EXPORT void __stdcall Ecat_OnTimer(void);

EXPORT void __stdcall Ecat_Close(void);
