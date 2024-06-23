
#include "ecat_def.h"
#include "applInterface.h"
#include "DemoObjects.h"



void APPL_Application_Adpt(void)
{
	if(Object70000x7000.Reserved1){
		Object60000x6000.Value1 = 1;
	}else{
		Object60000x6000.Value1 = 0;
	}
	
	if(Object70000x7000.Reserved2){
		Object60000x6000.Value2 = 1;
	}else{
		Object60000x6000.Value2 = 0;
	}
	
	if(Object70000x7000.Reserved3){
		Object60000x6000.Value3 = 1;
	}else{
		Object60000x6000.Value3 = 0;
	}
	
	if(Object70000x7000.Reserved4){
		Object60000x6000.Value4 = 1;
	}else{
		Object60000x6000.Value4 = 0;
	}
	
	if(Object70000x7000.Reserved5){
		Object60100x6010.Code1 = 0xff;
	}else{
		Object60100x6010.Code1 = 0;
	}
	
	if(Object70000x7000.Reserved6){
		Object60100x6010.Code2 = 0xff;
	}else{
		Object60100x6010.Code2 = 0;
	}

	if(Object70000x7000.Reserved7){
		Object60100x6010.Code3 = 0xff;
	}else{
		Object60100x6010.Code3 = 0;
	}
	
	if(Object70000x7000.Reserved8){
		Object60100x6010.Code4 = 0xff;
	}else{
		Object60100x6010.Code4 = 0;
	}
}

void APPL_OutputMapping_Adpt(UINT16* pData)
{
    UINT16 *pTmpData = (UINT16 *)pData;
	
	Object70000x7000.Reserved1 = (*pTmpData & 0x01) ? 1 : 0;
	Object70000x7000.Reserved2 = (*pTmpData & 0x02) ? 1 : 0;
	Object70000x7000.Reserved3 = (*pTmpData & 0x04) ? 1 : 0;
	Object70000x7000.Reserved4 = (*pTmpData & 0x08) ? 1 : 0;
	Object70000x7000.Reserved5 = (*pTmpData & 0x10) ? 1 : 0;
	Object70000x7000.Reserved6 = (*pTmpData & 0x20) ? 1 : 0;
	Object70000x7000.Reserved7 = (*pTmpData & 0x40) ? 1 : 0;
	Object70000x7000.Reserved8 = (*pTmpData & 0x80) ? 1 : 0;
	
	Object70000x7000.OutputValue = ((*pTmpData & 0xff00) >> 8) & 0xff;
}

void APPL_InputMapping_Adpt(UINT16* pData)
{
    UINT16 *pTmpData = (UINT16 *)pData;
	
	*pTmpData++ = Object60000x6000.Value1;
	*pTmpData++ = Object60000x6000.Value2;
	*pTmpData++ = Object60000x6000.Value3;
	*pTmpData++ = Object60000x6000.Value4;

	*pTmpData++ = Object60100x6010.Code1;
	*pTmpData++ = Object60100x6010.Code2;
	*pTmpData++ = Object60100x6010.Code3;
	*pTmpData++ = Object60100x6010.Code4;
}
