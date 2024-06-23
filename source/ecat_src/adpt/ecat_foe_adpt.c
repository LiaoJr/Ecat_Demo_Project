/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"
#include "applInterface.h"
//#include "ZIOB_E0800AI.h"
//#include "ZMC_Flash_Drv_HC460.h"
//#include "upgrade.h"
//#include "c_heap2.h"
#if BOOTSTRAPMODE_SUPPORTED
#include "bootmode.h"
#endif


//extern struct c_heap2            __g_mem_heap_hdr;
//extern struct upgrade_store_dev  __g_store_dev;

/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/


#if FOE_SUPPORTED
#define MAX_FILE_NAME_SIZE        48
#define MAX_FILE_SIZE             0x180
#endif


/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/
#if FOE_SUPPORTED
UINT32           nFileWriteOffset;
CHAR             aFileName[MAX_FILE_NAME_SIZE];
#if MBX_16BIT_ACCESS
UINT16 MBXMEM     aFileData[(MAX_FILE_SIZE >> 1)];
#else
UINT8 MBXMEM      aFileData[MAX_FILE_SIZE];
#endif //#else #ifMBX_16BIT_ACCESS


UINT8  *p_buf      = NULL;          /* 内存空间指针p_buf */
UINT8  g_heap2_mem_alloc_flag = 0;  /* 内存空间有无申请标志位(g_heap2_mem_alloc_flag) */

UINT32 u32FileSize;

#endif //#if FOE_SUPPORTED


#if FOE_SUPPORTED
UINT16 FoE_Master_Read(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password, UINT16 maxBlockSize, UINT16 *pData)
{
    UINT16 i = 0;
    UINT16 sizeError = 0;

    CHAR aReadFileName[MAX_FILE_NAME_SIZE];


    if ((nameSize + 1) > MAX_FILE_NAME_SIZE)
    {

        return ECAT_FOE_ERRCODE_DISKFULL;
    }

    /*Read requested file name to endianess conversion if required*/
    MBXSTRCPY(aReadFileName, pName, nameSize);
    aReadFileName[nameSize] = '\0';

    if (u32FileSize == 0)
    {
        /* No file stored*/
        return ECAT_FOE_ERRCODE_NOTFOUND;
    }

    /* for test only the written file name can be read */
    for (i = 0; i < nameSize; i++)
    {
        if (aReadFileName[i] != aFileName[i])
        {
            /* file name not found */
            return ECAT_FOE_ERRCODE_NOTFOUND;
        }
    }

    sizeError = maxBlockSize;

    if (u32FileSize < sizeError)
    {
        sizeError = (UINT16)u32FileSize;
    }

    /*copy the first foe data block*/
    MEMCPY(pData, aFileData, sizeError);

    return sizeError;
}

UINT16 FoE_Master_ReadData(UINT32 offset, UINT16 maxBlockSize, UINT16 *pData)
{
    UINT16 sizeError = 0;

    if (u32FileSize < offset)
    {
        return 0;
    }

    /*get file length to send*/
    sizeError = (UINT16)(u32FileSize - offset);


    if (sizeError > maxBlockSize)
    {
        /*transmit max block size if the file data to be send is greater than the max data block*/
        sizeError = maxBlockSize;
    }

    /*copy the foe data block 2 .. n*/
    MEMCPY(pData, &(((UINT8 *)aFileData)[offset]), sizeError);

    return sizeError;
}


UINT16 FoE_Slave_WriteData(UINT16 MBXMEM * pData, UINT16 Size, BOOL bDataFollowing)
{
    int ret;
#if BOOTSTRAPMODE_SUPPORTED
	/* 如果是BOOT Strap模式 则接收文件 */
    if (bBootMode)
    {
		if (Size){
			/* 调用升级接口，将数据存入flash */
			ret = upgrade_process((uint8_t *)pData, Size);
			if(ret == 0){
				
			}else{
				return ECAT_FOE_ERRCODE_ILLEGAL;
			}
		}

		/* bDataFollowing标志是否为最后一包 */
		if (bDataFollowing){
			/* FoE-Data services will follow */
			nFileWriteOffset += Size;
		}else{
			/* last part of the file is written */
			u32FileSize = nFileWriteOffset + Size;
			nFileWriteOffset = 0;
			/* 结束传输并激活升级 */
			ret = upgrade_end(1024);  
			if(ret == 0){
				
			}else{
				return ECAT_FOE_ERRCODE_ILLEGAL;
			}
			
			ret = upgrade_active(&__g_store_dev, 3, BOOT_ALWAYS);
			if(ret == 0){
				
			}else{
				return ECAT_FOE_ERRCODE_ILLEGAL;
			}
			EFM_Lock();
		}
	}
#endif
    return 0;
}


UINT16 FoE_Slave_Write(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password)
{
    if (nameSize < MAX_FILE_NAME_SIZE)
    {
        /* for test every file name can be written */
        MBXSTRCPY(aFileName, pName, nameSize);
        MBXSTRCPY(aFileName + nameSize, "\0", 1); //string termination
        
        EFM_Unlock();
		EFM_FlashCmd(Enable);
		
        nFileWriteOffset = 0;
        u32FileSize = 0;
		
        return 0;
    }
    else
    {
        return ECAT_FOE_ERRCODE_DISKFULL;
    }

}
#endif //#if FOE_SUPPORTED
