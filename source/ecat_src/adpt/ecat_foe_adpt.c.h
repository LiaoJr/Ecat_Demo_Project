#ifndef __ZIOB_ECAT_FOE_ADPT_H
#define __ZIOB_ECAT_FOE_ADPT_H

#include "ecat_def.h"



#if FOE_SUPPORTED
UINT16 FoE_Master_Read(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password, UINT16 maxBlockSize, UINT16 *pData);
UINT16 FoE_Master_ReadData(UINT32 offset, UINT16 maxBlockSize, UINT16 *pData);
UINT16 FoE_Slave_WriteData(UINT16 MBXMEM * pData, UINT16 Size, BOOL bDataFollowing);
UINT16 FoE_Slave_Write(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password);
#endif

#endif
