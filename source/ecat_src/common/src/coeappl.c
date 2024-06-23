/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
\addtogroup CoE CAN Application Profile over EtherCAT
@{
*/

/**
\file coeappl.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains an example for CoE services and the CoE object dictionary

\version 5.13

<br>Changes to version V5.12:<br>
V5.13 CIA402 3: change define "CIA402_DEVICE" to "CiA402_SAMPLE_APPLICATION"<br>
V5.13 COE1: handling objects with 255 entries<br>
V5.13 COE10: change 0x10F1.1 access from "rw" to "ro"<br>
V5.13 COE2: handle Backup entries with the code VAR<br>
V5.13 COE6: add 0x10F0.2 (Backup parameter changed)<br>
V5.13 COE7: define 0x1C3x.13 Shift Time too short, just define variable because expected in DC mode but shift mode is not supported by the SSC<br>
<br>Changes to version V5.11:<br>
V5.12 COE1: load backup value after the complete OD was initialized<br>
V5.12 COE4: add time stamp object (0x10F8) and update diagnosis handling<br>
V5.12 COE8: fix invalid size calculation to init backup entries<br>
V5.12 ECAT1: update SM Parameter measurement (based on the system time), enhancement for input only devices and no mailbox support, use only 16Bit pointer in process data length caluclation<br>
V5.12 ECAT5: update Sync error counter/flag handling,check enum memory alignment depending on the processor,in case of a polled timer disable ESC interrupts during DC_CheckWatchdog<br>
<br>Changes to version V5.10:<br>
V5.11 COE1: update invalid end entry in the object dictionaries (error with some compilers)<br>
V5.11 COE3: change 0x10F3.2 (Sync Error limit) from UINT32 to UINT16 (according to the ETG.1020)<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
V5.11 ECAT4: enhance SM/Sync monitoring for input/output only slaves<br>
<br>Changes to version V5.01:<br>
V5.10 COE1: Define one entry description for all 0x1C3x objects and change data type of SI11,12,13 to UINT16 (according ETG.1020)<br>
V5.10 COE2: Change return value of Get ObjectSize to UINT32<br>
            Change object size to UINT32<br>
V5.10 ECAT13: Update Synchronization handling (FreeRun,SM Sync, Sync0, Sync1)<br>
              Compare DC UINT configuration (by ESC Config data) vs. DC activation register settings<br>
              Update 0x1C3x entries<br>
<br>Changes to version V5.0:<br>
V5.01 APPL3: Include library demo application<br>
V5.01 COE1: Remove alignment entry (SI33) from objects 0x1C32/0x1C33<br>
V5.01 EL9800 1: Read/Write functions for setting object 0x8020 moved to el9800appl.c<br>
V5.01 EL9800 2: Add TxPdo Parameter object 0x1802<br>
V5.01 SDO6: Update SDO response interface handling. (used if the object access function returns "ABORTIDX_WORKING" and SDO_RES_INTERFACE is active)<br>
<br>Changes to version V4.40:<br>
V5.0 TEST1: Add test application. See Application Note ET9300 for more details.<br>
V5.0 COE1: Add reserved Subindex7 to object 0x1C32 and 0x1C33.<br>
V5.0 COE4: Create object dictionary on device startup.<br>
V5.0 COE6: Update entry descriptions for object with the code ARRAY.<br>
V5.0 ECAT2: Reference to application specific object dictionary is set to "ApplicationObjDic".<br>
<br>Changes to version V4.30:<br>
V4.40 COE 6: change dynamic object dictionary handling<br>
V4.40 CiA402 2: enable freerun Sync Mode for CiA402 Device (The motion controller application will only be triggered if DC Synchronisation is disabled)<br>
V4.40 COE5: move identification and synchronisation object values to ecat_def.h<br>
V4.40 DIAG1: Add diagnosis object 0x10F3<br>
V4.40 OBJ3: Update the global object dictionary pointer if a new list head is added<br>
V4.40 COE4: Prevent access NULL pointer<br>
V4.40 SYNC1: Initialize 0x1C3x objects<br>
V4.40 HW1: Add (adapt) objects for FC1100 support<br>
<br>Changes to version V4.20:<br>
V4.30 OBJ 3: Handle object dictionary in double link list<br>
V4.30 OBJ 2: General EtherCAT device objects are moved from chnappl.h to coeappl.c<br>
V4.30 SYNC: set 0x1C32:05; 0x1C33:05 (minCyleTime) to MIN_PD_CYCLE_TIME
                    (should be set dynamic if required)<br>
V4.30 PDO: rename PDO specific function calls
                    (PDO specific functions are moved to ecatappl.c)<br>
V4.20 PIC24: Add EL9800_4 (PIC24) required source code<br>
V4.11 ECAT 1: C166: MinDelayTime renamed in CalcAndCopyTime<br>
<br>Changes to version V4.07:<br>
V4.10 ECAT 1: To save and restore arrays in the object directory.<br>
            The code to get the size of an array entry was changed.<br>
V4.10 COE 1:  The SM-Parameter were extended<br>
<br>Changes to version V4.06:<br>
V4.07 COEAPPL 1: The PDO numbers were adapted according top the modular device profile,<br>
           the PDO numbers increment now with every module (in chnappl.h)<br>
V4.07 COEAPPL 2: The example is working for the NIOS with the evaluation board DBC2C20 V1.2<br>
            which is available by Altera<br>
V4.07 ECAT 1: The sources for SPI and MCI were merged (in ecat_def.h<br>
            set the switch MCI_HW to 1 when using the MCI,<br>
            set the switch SPI_HW to 1 when using the SPI<br>
<br>Changes to version V4.03:<br>
V4.04 SDO 1: The SDO interface was changed in that way that a SDO response<br>
            could be sent by the application to a later time. In that case<br>
            the functions OBJ_Read and OBJ_Write shall return the value<br>
            ABORTIDX_WORKING. To send the SDO response the new function SDOS_SdoRes<br>
            has to be called by the application. While waiting for the call<br>
            of SDOS_SdoRes the SDO interface will answer to another SDO request<br>
            with the error MBXERR_SERVICEINWORK in the mailbox protocol.<br>
            In this example the reading and writing of object 0x8020 is delayed,<br>
            the function SDOS_SdoRes will be called from COE_Main.<br>
<br>Changes to version V4.00:<br>
V4.01 COEAPPL 1: The object dictionary and the mapping functions are now defined<br>
             in chnappl.h (chnappl.inc made problems with some developing tools)<br>
<br>Changes to version V3.20:<br>
V4.00 COEAPPL 1: The object dictionary and the mapping functions are now defined<br>
            in chnappl.c and were adapted to the modular device profile<br>
V4.00 COEAPPL 2: The handling of backup parameters was included according to<br>
            the EtherCAT Guidelines and Protocol Enhancements Specification<br>
V4.00 COEAPPL 3: The handling of the TxPDO Toggle was included according to<br>
             the EtherCAT Guidelines and Protocol Enhancements Specification<br>
V4.00 COEAPPL 4: The handling of the TxPDO State was included according to<br>
            the EtherCAT Guidelines and Protocol Enhancements Specification<br>
V4.00 ECAT 1: The handling of the Sync Manager Parameter was included according to<br>
            the EtherCAT Guidelines and Protocol Enhancements Specification
*/

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/

#include "ecat_def.h"

#if COE_SUPPORTED

#include "ecatslv.h"

#define _COEAPPL_    1
#include "coeappl.h"
#undef _COEAPPL_
/*remove definition of _COEAPPL_ (#ifdef is used in coeappl.h)*/

#define _OBJD_
#if DIAGNOSIS_SUPPORTED && !TEST_APPLICATION
/*will be included via testappl.h*/
#include "diag.h"
#endif


/*Add Application specific Objects*/
/*ECATCHANGE_START(V5.13) CIA402 3*/
#if CiA402_SAMPLE_APPLICATION
/*ECATCHANGE_END(V5.13) CIA402 3*/
#include "cia402appl.h"
#elif EL9800_APPLICATION
#include "el9800appl.h"
#elif SAMPLE_APPLICATION
#include "sampleappl.h"
#elif TEST_APPLICATION
#include "testappl.h"
#elif SAMPLE_APPLICATION_INTERFACE
#include "SampleApplicationInterface.h"
#else
/*CODE_INSERT_START (APPLICATION_FILE)*/
#include "Demo.h"
/*CODE_INSERT_END*/
#endif


#undef _OBJD_
/*-----------------------------------------------------------------------------------------
------
------    Backup Parameter
------
-----------------------------------------------------------------------------------------*/
#if BACKUP_PARAMETER_SUPPORTED
/* the following functions are defined in this file */
void         InitDefaultEntries(void);
void         LoadBackupEntries(void);
void         StoreBackupEntries(void);
void         LoadDefaultEntries(void);

#endif //#if BACKUP_PARAMETER_SUPPORTED

/*-----------------------------------------------------------------------------------------
------
------    Generic EtherCAT device object
------
-----------------------------------------------------------------------------------------*/

/*---------------------------------------------
-    0x1000
-----------------------------------------------*/
/**
 * \brief 0x1000 (Device type) variable to handle the object data
 */
UINT32 u32Devicetype = (DEVICE_PROFILE_TYPE);

/**
 * \brief 0x1000 (Device type) entry description
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x1000 = {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ};

/**
 * \brief 0x1000 (Device type) object name
 */
OBJCONST UCHAR OBJMEM aName0x1000[] = "Device type";


/*---------------------------------------------
-    0x1001
-----------------------------------------------*/
/**
 * \brief 0x1001 (Error Register) variable to handle the object data
 */
UINT16 u16ErrorRegister = 0x0;

/**
 * \brief 0x1001 (Error Register) entry description
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x1001 = {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ};

/**
 * \brief 0x1001 (Error Register) object name
 */
OBJCONST UCHAR OBJMEM aName0x1001[] = "Error register";


/*---------------------------------------------
-    0x1008
-----------------------------------------------*/
/**
 * \brief 0x1008 (Device name) variable to handle the object data
 */
CHAR acDevicename[] = DEVICE_NAME;

/**
 * \brief 0x1008 (Device name) entry description
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x1008 = {DEFTYPE_VISIBLESTRING, BYTE2BIT(DEVICE_NAME_LEN), ACCESS_READ};

/**
 * \brief 0x1008 (Device name) object name
 */
OBJCONST UCHAR OBJMEM aName0x1008[] = "Device name";


/*---------------------------------------------
-    0x1009
-----------------------------------------------*/
/**
 * \brief 0x1009 (Hardware version) variable to handle the object data
 */
CHAR acHardwareversion[] = DEVICE_HW_VERSION;

/**
 * \brief 0x1009 (Hardware version) entry description
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x1009 = {DEFTYPE_VISIBLESTRING,BYTE2BIT(DEVICE_HW_VERSION_LEN), ACCESS_READ};

/**
 * \brief 0x1009 (Hardware version) object name
 */
OBJCONST UCHAR OBJMEM aName0x1009[] = "Manufacturer Hardware version";


/*---------------------------------------------
-    0x100A
-----------------------------------------------*/
/**
 * \brief 0x100A (Software version) variable to handle the object data
 */
CHAR acSoftwareversion[] = DEVICE_SW_VERSION;

/**
 * \brief 0x100A (Software version) entry description
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x100A = {DEFTYPE_VISIBLESTRING,BYTE2BIT(DEVICE_SW_VERSION_LEN), ACCESS_READ};

/**
 * \brief 0x100A (Software version) object name
 */
OBJCONST UCHAR OBJMEM aName0x100A[] = "Manufacturer Software version";


#if BACKUP_PARAMETER_SUPPORTED
/*---------------------------------------------
-    0x1010
-----------------------------------------------*/

/**
 * \brief 0x1010 (Store parameters) variable to handle the object data
 */
TOBJ1010 sStoreparameters = {0x01, {0x00}};

/**
 * \brief 0x1010 (Store parameters) entry descriptions
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1010[] = {
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE}};

/**
 * \brief 0x1010 (Store parameters) object name (for the entry "SubIndex XXX" will be returned)
 */
OBJCONST UCHAR OBJMEM aName0x1010[] = "Store parameters";


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     index                    index of the requested object.
 \param     subindex                subindex of the requested object.
 \param    dataSize                received data size of the SDO Download
 \param    pData                    Pointer to the buffer where the written data can be copied from
 \param    bCompleteAccess    Indicates if a complete write of all subindices of the
                                     object shall be done or not

 \return    result of the write operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief    This function writes the object 0x1010
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 Write0x1010( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{
    
   if ( bCompleteAccess )
   {
      /* Complete Access is not supported for object 0x1010 */
      return ABORTIDX_UNSUPPORTED_ACCESS;
   }

   if ( subindex == 0 )
   {
      /* Subindex 0 is not writable */
      return ABORTIDX_READ_ONLY_ENTRY;
   }

   if ( pData[0] == 0x6173 && pData[1] == 0x6576 )
   {
      /* Save the backup entries */
      StoreBackupEntries();
   }
   else
   {
      return ABORTIDX_VALUE_EXCEEDED;
   }
   return 0;
}

/*---------------------------------------------
-    0x1011
-----------------------------------------------*/

/**
 * \brief 0x1011 (Restore default parameters) variable to handle the object data
 */
TOBJ1011 sRestoredefaultparameters = {0x01, {0x01}};

/**
 * \brief 0x1011 (Restore default parameters) entry descriptions
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1011[] = {
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE}};

/**
 * \brief 0x1011 (Restore default parameters) object name 
 */
OBJCONST UCHAR OBJMEM aName0x1011[] = "Restore default parameters";

/**
 \param     index                    index of the requested object.
 \param     subindex                subindex of the requested object.
 \param    dataSize                received data size of the SDO Download
 \param    pData                    Pointer to the buffer where the written data can be copied from
 \param    bCompleteAccess    Indicates if a complete write of all subindices of the
                                     object shall be done or not

 \return    result of the write operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief    This function writes the object 0x1011
*/
UINT8 Write0x1011( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{
    
   if ( bCompleteAccess )
   {
      /* Complete Access is not supported for object 0x1011 */
      return ABORTIDX_UNSUPPORTED_ACCESS;
   }

   if ( subindex == 0 )
   {
      /* Subindex 0 is not writable */
      return ABORTIDX_READ_ONLY_ENTRY;
   }

   if ( pData[0] == 0x6F6C && pData[1] == 0x6461 )
   {
      /* Load the defaults for all backup entries */
      LoadDefaultEntries();
   }
   else
   {
      return ABORTIDX_VALUE_EXCEEDED;
   }
   return 0;
}

#endif // BACKUP_PARAMETER_SUPPORTED


/*---------------------------------------------
-    0x1018
-----------------------------------------------*/

/**
 * \brief 0x1018 (Identity) variable to handle the object data
 */
TOBJ1018 sIdentity = {4, (VENDOR_ID), (PRODUCT_CODE), (REVISION_NUMBER), (SERIAL_NUMBER)};

/**
 * \brief 0x1018 (Identity) entry descriptions
 * 
 * Subindex 000
 * SubIndex 001: Vendor ID
 * SubIndex 002: Product code
 * SubIndex 003: Revision
 * SubIndex 004: Serial number
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1018[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, 
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, 
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, 
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};

/**
 * \brief 0x1018 (Identity) object and entry names
 */
OBJCONST UCHAR OBJMEM aName0x1018[] = "Identity Object\000Vendor ID\000Product Code\000Revision Number\000Serial Number\000\377";


#if BACKUP_PARAMETER_SUPPORTED
/*ECATCHANGE_START(V5.13) COE6*/
/**
 * \brief 0x10F0 (Backup parameter handling) variable to handle the object data
 */
TOBJ10F0 sBackupparameterhandling = {2, 0x00,0};


/**
 * \brief 0x10F0 (Backup parameter handling) entry descriptions
 * 
 * Subindex 000
 * SubIndex 001: Checksum
 * SubIndex 002: Backup Parameter Changed
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x10F0[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_TXPDOMAPPING} };

/**
 * \brief 0x10F0 (Backup parameter handling) object name
 */
OBJCONST UCHAR OBJMEM aName0x10F0[] = "Backup parameter handling\000Checksum\000Backup Parameter Changed\000\377";
/*ECATCHANGE_END(V5.13) COE6*/
#endif // BACKUP_PARAMETER_SUPPORTED

#if !_PIC18
/*---------------------------------------------
-    0x10F1
-----------------------------------------------*/
/**
 * \brief 0x10F1 (Error settings) entry description
 * Subindex 000
 * SubIndex 001: Local Error Reaction
 * SubIndex 002: Sync Error Counter Limit
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x10F1[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
/*ECATCHANGE_START(V5.13) COE10*/
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
/*ECATCHANGE_END(V5.13) COE10*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READWRITE}};

/**
 * \brief 0x10F1 (Error settings) object and entry names
 */   
OBJCONST UCHAR OBJMEM aName0x10F1[] = "Error Settings\000Local Error Reaction\000Sync Error Counter Limit\000\377";


//object declaration and initialization in objdef.h
#endif


#if !_PIC18
/*---------------------------------------------
-    0x10F8 (Timestamp object)
-----------------------------------------------*/
/**
* \brief 0x10F8 (Timestamp object) entry description
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x10F8 = { DEFTYPE_UNSIGNED64, 0x40, ACCESS_READWRITE | OBJACCESS_TXPDOMAPPING } ;
/**
* \brief 0x10F8 (Timestamp object) object name
*/
OBJCONST UCHAR OBJMEM aName0x10F8[] = "Timestamp Object\000\377";


//object declaration and initialization in coeappl.h

/**
\param     index                    index of the requested object.
\param     subindex                subindex of the requested object.
\param    dataSize                received data size of the SDO Download
\param    pData                    Pointer to the buffer where the written data can be copied from
\param    bCompleteAccess    Indicates if a complete write of all subindices of the
object shall be done or not

\return    result of the write operation (0 (success) or an abort code (ABORTIDX_.... defined in
sdosrv.h))

\brief    This function reads the object 0x10F8
*/
UINT8 Read0x10F8(UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess)
{

    if (index != 0x10F8)
    {
        return ABORTIDX_PARAM_IS_INCOMPATIBLE;
    }

    
    if (bCompleteAccess)
    {
         /* Complete Access is not supported for object 0x10F8 */
        return ABORTIDX_UNSUPPORTED_ACCESS;
    }

    if (subindex > 0)
    {
        return ABORTIDX_SUBINDEX_NOT_EXISTING;
    }

    if (dataSize > 8)
    {
        return ABORTIDX_PARAM_LENGTH_TOO_LONG;
    }


#if DC_SUPPORTED
    COE_SyncTimeStamp();
#endif

    MEMCPY(pData, &u64Timestamp, dataSize);
    return 0;
}
#endif


#if DC_SUPPORTED
/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function updates the local time stamp object (0x10F8) and has to be called at least every 4.2sec to detect an 32Bit DC unit overrun.
          Called from the Timer handler 
*////////////////////////////////////////////////////////////////////////////////////////
void COE_SyncTimeStamp(void)
{

    if (b32BitDc)
    {
        UINT32 DcTime = (UINT32)(u64Timestamp & (UINT64)0x00000000FFFFFFFF);


        HW_EscReadDWord(DcTime, ESC_SYSTEMTIME_OFFSET);

        /*update the lower 32Bit*/
        u64Timestamp = ((u64Timestamp & ((UINT64)0xFFFFFFFF00000000)) | (UINT64)DcTime);

        if (DcTime < u32LastDc32Value)
        {
            /*32Bit overrun*/
            u64Timestamp = u64Timestamp + ((UINT64)0x0000000100000000);
        }
        
        u32LastDc32Value = DcTime;
    }
    else
    {
        /*The DC unit supports 64Bit => update the complete object*/
        
        HW_EscRead((MEM_ADDR *)&u64Timestamp, ESC_SYSTEMTIME_OFFSET, 8);
    }

    u32CheckForDcOverrunCnt = 0;

}
#endif



/*---------------------------------------------
-    0x1C00
-----------------------------------------------*/

/**
 * \brief 0x1C00 (Sync manager type) variable to handle the object data
 *
 * SI 0 : 4 (4 SyncManagers available)
 * SI1 (SM0): Mailbox Out (0x1)
 * SI2 (SM1): Mailbox In (0x2)
 * SI3 (SM2): Process data Out (0x3)
 * SI4 (SM3): Process data In (0x4)
 *
 */
 /*ECATCHANGE_START(V5.13)*/
TOBJ1C00 sSyncmanagertype = {0x04, {0x0201, 0x0403}};
/*ECATCHANGE_END(V5.13)*/


/**
 * \brief 0x1C00 (Sync manager type) entry descriptions
 *
 * The object is an array so only the first entry is described (which is used for all entries).
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C00[] = {
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ},
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ}};

/**
 * \brief 0x1C00 (Sync manager type) object name
 */
OBJCONST UCHAR OBJMEM aName0x1C00[] = "Sync manager type";


/*---------------------------------------------
-    0x1C3x
-----------------------------------------------*/
/**
 * \brief 0x1C3x (SyncManager parameter) entry description
 *
 * The entry descriptions are used for 0x1C32 and 0x1C33
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C3x[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ }, /* Subindex 000 */
#if !AL_EVENT_ENABLED && !DC_SUPPORTED
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex 001: Synchronization Type */
#else
   {DEFTYPE_UNSIGNED16, 0x10, (ACCESS_READ | ACCESS_WRITE_PREOP)}, /* SubIndex 001: Synchronization Type */
#endif
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 002: Cycle Time */
   {0x0000, 0x20, 0}, /* SubIndex 003: Shift Time (not supported)*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex 004: Synchronization Types supported */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 005: Minimum Cycle Time */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 006: Calc and Copy Time */
   {0x0000, 0x20, 0}, /* Subindex 007 Minimum Delay Time (not supported)*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READWRITE}, /* SubIndex 008: Get Cycle Time */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 009: Delay Time */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE}, /* SubIndex 010: Sync0 Cycle Time */
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex 011: SM-Event Missed */
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex 012: Cycle Time Too Small */
   /*ECATCHANGE_START(V5.13) COE7*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex 013: Shift Too Short Counter*/
   /*ECATCHANGE_END(V5.13) COE7*/
   {0x0000, 0x10, 0}, /* Subindex 014: RxPDO Toggle Failed (not supported)*/
   {0x0000, 0x20, 0}, /* Subindex 015: Minimum Cycle Distance (not supported)*/
   {0x0000, 0x20, 0}, /* Subindex 016: Maximum Cycle Distance (not supported)*/
   {0x0000, 0x20, 0}, /* Subindex 017: Minimum SM Sync Distance (not supported)*/
   {0x0000, 0x20, 0}, /* Subindex 018: Maximum SM Sync Distance (not supported)*/
   {0x0000, 0, 0}, /* Subindex 019 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 020 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 021 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 022 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 023 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 024 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 025 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 026 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 027 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 028 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 029 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 030 doesn't exist */
   {0x0000, 0, 0}, /* Subindex 031 doesn't exist */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}}; /* SubIndex 032: Sync error */

#if MAX_PD_OUTPUT_SIZE > 0
/**
 * \brief 0x1C32 (SyncManager 2 parameter) object and entry names
 */
OBJCONST UCHAR OBJMEM aName0x1C32[] = "SM output parameter\000Synchronization Type\000Cycle Time\000\000Synchronization Types supported\000Minimum Cycle Time\000Calc and Copy Time\000\000Get Cycle Time\000Delay Time\000Sync0 Cycle Time\000SM-Event Missed\000Cycle Time Too Small\000Shift Time Too Short Counter\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000Sync Error\000\377";
#endif

#if MAX_PD_INPUT_SIZE > 0
/**
 * \brief 0x1C33 (SyncManager 3 parameter) object and entry names
 */
OBJCONST UCHAR OBJMEM aName0x1C33[] = "SM input parameter\000Synchronization Type\000Cycle Time\000\000Synchronization Types supported\000Minimum Cycle Time\000Calc and Copy Time\000\000Get Cycle Time\000Delay Time\000Sync0 Cycle Time\000SM-Event Missed\000Cycle Time Too Small\000Shift Time Too Short Counter\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000Sync Error\000\377";
#endif

/******************************************************************************
** Object Dictionary
******************************************************************************/
#if !STATIC_OBJECT_DIC
/**
 * \brief Object dictionary pointer
 */
TOBJECT OBJMEM * ObjDicList = NULL;

/**
 * \brief List of generic application independent objects
 */
TOBJECT    OBJMEM GenObjDic[] = {
    /* Object 0x1000 */
   {NULL,NULL,  0x1000, {DEFTYPE_UNSIGNED32, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x1000, aName0x1000, &u32Devicetype, NULL, NULL, 0x0000 },
   /* Object 0x1001 */
   {NULL,NULL,  0x1001, {DEFTYPE_UNSIGNED8, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x1001, aName0x1001, &u16ErrorRegister, NULL, NULL, 0x0000 },
/* Object 0x1008 */
   {NULL,NULL,  0x1008, {DEFTYPE_VISIBLESTRING, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x1008, aName0x1008, acDevicename, NULL, NULL, 0x0000 },
   /* Object 0x1009 */
   {NULL,NULL,  0x1009, {DEFTYPE_VISIBLESTRING, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x1009, aName0x1009, acHardwareversion, NULL, NULL, 0x0000 },
   /* Object 0x100A */
   {NULL,NULL,  0x100A, {DEFTYPE_VISIBLESTRING, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x100A, aName0x100A, acSoftwareversion, NULL, NULL, 0x0000 },
#if BACKUP_PARAMETER_SUPPORTED
   /* Object 0x1010 */
   {NULL,NULL,  0x1010, {DEFTYPE_UNSIGNED32, 1 | (OBJCODE_ARR << 8)}, asEntryDesc0x1010, aName0x1010, &sStoreparameters, NULL, Write0x1010, 0x0000 },
   /* Object 0x1011 */
   {NULL,NULL,  0x1011, {DEFTYPE_UNSIGNED32, 1 | (OBJCODE_ARR << 8)}, asEntryDesc0x1011, aName0x1011, &sRestoredefaultparameters, NULL, Write0x1011, 0x0000 },
 #endif // BACKUP_PARAMETER_SUPPORTED
   /* Object 0x1018 */
   {NULL,NULL,  0x1018, {DEFTYPE_IDENTITY, 4 | (OBJCODE_REC << 8)}, asEntryDesc0x1018, aName0x1018, &sIdentity, NULL, NULL, 0x0000 },
#if BACKUP_PARAMETER_SUPPORTED
   /*ECATCHANGE_START(V5.13) COE6*/
    /* Object 0x10F0 */
   {NULL,NULL,  0x10F0, {DEFTYPE_RECORD, 2 | (OBJCODE_REC << 8)}, asEntryDesc0x10F0, aName0x10F0, &sBackupparameterhandling, NULL, NULL, 0x0000 },
   /*ECATCHANGE_END(V5.13) COE6*/
#endif // BACKUP_PARAMETER_SUPPORTED
#if !_PIC18
    /* Object 0x10F1 */
   {NULL,NULL,  0x10F1, {DEFTYPE_RECORD, 2 | (OBJCODE_REC << 8)}, asEntryDesc0x10F1, aName0x10F1, &sErrorSettings, NULL, NULL, 0x0000 },
    /* Object 0x10F8 */
   { NULL,NULL,  0x10F8,{ DEFTYPE_UNSIGNED64, 0 | (OBJCODE_VAR << 8) }, &sEntryDesc0x10F8, aName0x10F8, &u64Timestamp, Read0x10F8, NULL , 0x0000 },
#if DIAGNOSIS_SUPPORTED
    /* Object 0x10F3 */
    {NULL,NULL,  0x10F3, {DEFTYPE_RECORD, (5 + MAX_DIAG_MSG) | (OBJCODE_REC << 8)}, asEntryDesc0x10F3, aName0x10F3, &sDiagHistory,Read0x10F3, Write0x10F3, 0x0000 },
#endif
#endif
   /* Object 0x1C00 */
   {NULL,NULL, 0x1C00, {DEFTYPE_UNSIGNED8, 4 | (OBJCODE_ARR << 8)}, asEntryDesc0x1C00, aName0x1C00, &sSyncmanagertype, NULL, NULL, 0x0000 },
#if MAX_PD_OUTPUT_SIZE > 0
   /* Object 0x1C32 */
   {NULL,NULL, 0x1C32, {DEFTYPE_SMPAR, 32 | (OBJCODE_REC << 8)}, asEntryDesc0x1C3x, aName0x1C32, &sSyncManOutPar, NULL, NULL, 0x0000 },
#endif
#if MAX_PD_INPUT_SIZE > 0
   /* Object 0x1C33 */
   {NULL,NULL, 0x1C33, {DEFTYPE_SMPAR, 32 | (OBJCODE_REC << 8)}, asEntryDesc0x1C3x, aName0x1C33, &sSyncManInPar, NULL, NULL, 0x0000 },
#endif
   
  /*end of entries*/
  {NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL, NULL, 0x000}};

#else
/**
 * \brief Complete object dictionary
 */
const TOBJECT OBJMEM ObjDicList[] = {
#if _PIC18
    /* Enum 0x0800 */
    {0x0800, {DEFTYPE_ENUM, 0x02 | (OBJCODE_REC << 8)}, asEntryDesc0x0800, 0, apEnum0800 },
#endif
    /* Object 0x1000 */
   {0x1000, {DEFTYPE_UNSIGNED32, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x1000, aName0x1000, &u32Devicetype, NULL, NULL, 0x0000 },
   /* Object 0x1001 */
   {0x1001, {DEFTYPE_UNSIGNED8, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x1001, aName0x1001, &u16ErrorRegister, NULL, NULL, 0x0000 },
   /* Object 0x1008 */
   {0x1008, {DEFTYPE_VISIBLESTRING, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x1008, aName0x1008, acDevicename, NULL, NULL, 0x0000 },
   /* Object 0x1009 */
   {0x1009, {DEFTYPE_VISIBLESTRING, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x1009, aName0x1009, acHardwareversion, NULL, NULL, 0x0000 },
   /* Object 0x100A */
   {0x100A, {DEFTYPE_VISIBLESTRING, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x100A, aName0x100A, acSoftwareversion, NULL, NULL, 0x0000 },
#if BACKUP_PARAMETER_SUPPORTED
   /* Object 0x1010 */
   {0x1010, {DEFTYPE_UNSIGNED32, 1 | (OBJCODE_ARR << 8)}, asEntryDesc0x1010, aName0x1010, &sStoreparameters, NULL, Write0x1010, 0x0000 },
   /* Object 0x1011 */
   {0x1011, {DEFTYPE_UNSIGNED32, 1 | (OBJCODE_ARR << 8)}, asEntryDesc0x1011, aName0x1011, &sRestoredefaultparameters, NULL, Write0x1011, 0x0000 },
 #endif // BACKUP_PARAMETER_SUPPORTED
   /* Object 0x1018 */
   {0x1018, {DEFTYPE_IDENTITY, 4 | (OBJCODE_REC << 8)}, asEntryDesc0x1018, aName0x1018, &sIdentity, NULL, NULL, 0x0000 },
#if BACKUP_PARAMETER_SUPPORTED
    /* Object 0x10F0 */
   {0x10F0, {DEFTYPE_RECORD, 1 | (OBJCODE_REC << 8)}, asEntryDesc0x10F0, aName0x10F0, &sBackupparameterhandling, NULL, NULL, 0x0000 },
#endif // BACKUP_PARAMETER_SUPPORTED
#if DIAGNOSIS_SUPPORTED
    /* Object 0x10F3 */
   {0x10F3, {DEFTYPE_RECORD, (5 + MAX_DIAG_MSG) | (OBJCODE_REC << 8)}, asEntryDesc0x10F3, aName0x10F3, &sDiagHistory, Read0x10F3, Write0x10F3, 0x0000 },
#endif
#if _PIC18
  /* Object 0x1601 */
    {0x1601, {DEFTYPE_PDOMAPPING, 5 | (OBJCODE_REC << 8)}, asEntryDesc0x1601, aName0x1601, &sDORxPDOMap, NULL, NULL, 0x0000 },
   /* Object 0x1802 */
   {0x1802, {DEFTYPE_RECORD, 9 | (OBJCODE_REC << 8)}, asEntryDesc0x1802, aName0x1802, ReadObject0x1802, NULL, 0x0000 },
    /* Object 0x1A00 */
   {0x1A00, {DEFTYPE_PDOMAPPING, 5 | (OBJCODE_REC << 8)}, asEntryDesc0x1A00, aName0x1A00, &sDITxPDOMap, NULL, NULL, 0x0000 },
   /* Object 0x1A02 */
   {0x1A02, {DEFTYPE_PDOMAPPING, 8 | (OBJCODE_REC << 8)}, asEntryDesc0x1A02, aName0x1A02, &sAITxPDOMap, NULL, NULL, 0x0000 },
   /* Object 0x1C00 */
   {0x1C00, {DEFTYPE_UNSIGNED8, 4 | (OBJCODE_ARR << 8)}, asEntryDesc0x1C00, aName0x1C00, &sSyncmanagertype, NULL, NULL, 0x0000 },
     /* Object 0x1C12 */
   {0x1C12, {DEFTYPE_UNSIGNED16, 1 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C12, &sRxPDOassign, NULL, NULL, 0x0000 },
     /* Object 0x1C13 */
    {0x1C13, {DEFTYPE_UNSIGNED16, 2 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C13, &sTxPDOassign, NULL, NULL, 0x0000 },
#endif
   /* Object 0x1C32 */
   {0x1C32, {DEFTYPE_SMPAR, 32 | (OBJCODE_REC << 8)}, asEntryDesc0x1C3x, aName0x1C32, &sSyncManOutPar, NULL, NULL, 0x0000 },
   /* Object 0x1C33 */
   {0x1C33, {DEFTYPE_SMPAR, 32 | (OBJCODE_REC << 8)}, asEntryDesc0x1C3x, aName0x1C33, &sSyncManInPar, NULL, NULL, 0x0000 },
#if _PIC18
   /* Object 0x6000 */
    {0x6000, {DEFTYPE_RECORD, 4 | (OBJCODE_REC << 8)}, asEntryDesc0x6000, aName0x6000, &sDIInputs, NULL, NULL, 0x0000 },
   /* Object 0x6020 */
   {0x6020, {DEFTYPE_RECORD, 17 | (OBJCODE_REC << 8)}, asEntryDesc0x6020, aName0x6020, &sAIInputs, NULL, NULL, 0x0000 },
   /* Object 0x7010 */
    {0x7010, {DEFTYPE_RECORD, 4 | (OBJCODE_REC << 8)}, asEntryDesc0x7010, aName0x7010, &sDOOutputs, NULL, NULL, 0x0000 },
    /* Object 0x8020 */
   {0x8020, {DEFTYPE_RECORD, 20 | (OBJCODE_REC << 8)}, asEntryDesc0x8020, aName0x8020, &sAISettings, NULL, NULL, 0x0008 },
      /* Object 0xF000 */
   {0xF000, {DEFTYPE_RECORD, 2 | (OBJCODE_REC << 8)}, asEntryDesc0xF000, aName0xF000, &sModulardeviceprofile, NULL, NULL, 0x0000 },
   /* Object 0xF010 */
   {0xF010, {DEFTYPE_UNSIGNED32, 3 | (OBJCODE_ARR << 8)}, asEntryDesc0xF010, aName0xF010, &sModulelist, NULL, NULL, 0x0000 },
#endif
   /*end of entries*/
   {0xFFFF, {0, 0}, NULL, NULL, NULL, NULL, NULL, 0x000}};
#endif //#else #if !STATIC_OBJECT_DIC

/*-----------------------------------------------------------------------------------------
------
------    Functions
------
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    returns the pointer to the object dictionary
*////////////////////////////////////////////////////////////////////////////////////////
OBJCONST TOBJECT OBJMEM * COE_GetObjectDictionary(void)
{
    return (OBJCONST TOBJECT OBJMEM *) ObjDicList;
}

#if BACKUP_PARAMETER_SUPPORTED
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function initialize the non-volatile memory with the default values
         of all backup entries
*////////////////////////////////////////////////////////////////////////////////////////

void InitDefaultEntries(void)
{
    if(ObjDicList != NULL)
    {

    OBJCONST TOBJECT OBJMEM * pObjEntry = (OBJCONST TOBJECT OBJMEM *) ObjDicList;

    /* call the microcontroller specific function to store the actual values of the Non-Volatile Memory as Default data */
    EE_ResetFlashData();
    /* we make a loop over all objects */
#if !STATIC_OBJECT_DIC
    while (pObjEntry!= NULL)
#else
    while (pObjEntry->Index != 0xFFFF)
#endif
    {
        if (pObjEntry->NonVolatileOffset != 0)
        {
            UINT8 maxSi = ((pObjEntry->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT);
            UINT8 objCode = (pObjEntry->ObjDesc.ObjFlags & OBJFLAGS_OBJCODEMASK) >> OBJFLAGS_OBJCODESHIFT;

            /* object contains backup entries, in this example we store the complete object
               in non-volatile memory */

            /*The full object size (including gaps based on the used platform/compiler) is needed*/
            UINT32 size = (UINT32)OBJ_GetEntryOffset(maxSi, pObjEntry);


            if(objCode == OBJCODE_REC)
            {
                size += pObjEntry->pEntryDesc[maxSi].BitLength;
            }
            else if (objCode == OBJCODE_ARR)
            {
                size += pObjEntry->pEntryDesc[1].BitLength;
            }
/*ECATCHANGE_START(V5.13) COE2*/
            else if (objCode == OBJCODE_VAR)
            {
                size = pObjEntry->pEntryDesc[0].BitLength;
            }
/*ECATCHANGE_END(V5.13) COE2*/
            size = BIT2WORD(size);


            /* call the microcontroller specific function to store the data in the non-volatile memory */
            EE_WriteWordsToNonVolatileMemory(pObjEntry->NonVolatileOffset,(UINT16 *) pObjEntry->pVarPtr, (UINT16) size);
        }
#if !STATIC_OBJECT_DIC
        pObjEntry = (TOBJECT OBJMEM *) pObjEntry->pNext;
#else
        pObjEntry++;
#endif
    }
    /* call the microcontroller specific function to store the actual values of the Non-Volatile Memory as Default data */
    EE_StoreDefaultData();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function loads all backup entries from the non-volatile memory
*////////////////////////////////////////////////////////////////////////////////////////

void LoadBackupEntries(void)
{
    if(ObjDicList != NULL)
    {

    OBJCONST TOBJECT OBJMEM * pObjEntry = (OBJCONST TOBJECT OBJMEM *) ObjDicList;

    /* we make a loop over all objects */
#if !STATIC_OBJECT_DIC
    while (pObjEntry!= NULL)
#else
    while (pObjEntry->Index != 0xFFFF)
#endif
    {
        if (pObjEntry->NonVolatileOffset != 0)
        {
            /* object contains backup entries */
            UINT8 maxSubindex = (pObjEntry->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;
/*ECATCHANGE_START(V5.13) COE1*/
            UINT16 i;
/*ECATCHANGE_END(V5.13) COE1*/

            for (i = 0; i <= maxSubindex; i++)
            {
                /* get the corresponding entry description */
                OBJCONST TSDOINFOENTRYDESC OBJMEM *pEntry = OBJ_GetEntryDesc(pObjEntry, (UINT8)i);

                if (pEntry->ObjAccess & OBJACCESS_BACKUP)
                {
                    /* entry is a backup entry */
                    UINT16 wordOffset = OBJ_GetEntryOffset((UINT8)i, pObjEntry) >> 4;

                    UINT16 wordSize = BIT2WORD(pEntry->BitLength);

                    /* call the microcontroller specific function to load the data from the non-volatile memory */
                    EE_ReadWordsFromNonVolatileMemory(&((UINT16 *) pObjEntry->pVarPtr)[wordOffset], pObjEntry->NonVolatileOffset+wordOffset, wordSize);
                }
            }
        }
#if !STATIC_OBJECT_DIC
        pObjEntry = (TOBJECT OBJMEM *) pObjEntry->pNext;
#else
        pObjEntry++;
#endif
    }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function stores all backup entries in the non-volatile memory
*////////////////////////////////////////////////////////////////////////////////////////

void StoreBackupEntries(void)
{
    if(ObjDicList != NULL)
    {
    OBJCONST TOBJECT OBJMEM * pObjEntry = (OBJCONST TOBJECT OBJMEM *) ObjDicList;
    /* we make a loop over all objects */
#if !STATIC_OBJECT_DIC
    while (pObjEntry!= NULL)
#else
    while (pObjEntry->Index != 0xFFFF)
#endif
    {
        if (pObjEntry->NonVolatileOffset != 0)
        {
            /* object contains backup entries */
            UINT8 maxSubindex = (pObjEntry->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;
/*ECATCHANGE_START(V5.13) COE1*/
            UINT16 i;
/*ECATCHANGE_END(V5.13) COE1*/

            for (i = 0; i <= maxSubindex; i++)
            {
                /* get the corresponding entry description */
                OBJCONST TSDOINFOENTRYDESC OBJMEM *pEntry = OBJ_GetEntryDesc(pObjEntry, (UINT8)i);

                if (pEntry->ObjAccess & OBJACCESS_BACKUP)
                {
                    /* entry is a backup entry */
                    UINT16 wordOffset = OBJ_GetEntryOffset((UINT8)i, pObjEntry) >> 4;

                    UINT16 wordSize = BIT2WORD(pEntry->BitLength);

                    /* call the microcontroller specific function to store the data in the non-volatile memory
                       and store the checksum in backup parameter checksum object */
                    sBackupparameterhandling.u32Checksum = EE_WriteWordsToNonVolatileMemory(pObjEntry->NonVolatileOffset+wordOffset, &((UINT16 *) pObjEntry->pVarPtr)[wordOffset], wordSize);
                }
            }
        }
#if !STATIC_OBJECT_DIC
        pObjEntry = (TOBJECT OBJMEM *) pObjEntry->pNext;
#else
        pObjEntry++;
#endif
    }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function load the defaults of all backup parameters
*////////////////////////////////////////////////////////////////////////////////////////

void LoadDefaultEntries(void)
{
    /* call the microcontroller specific function to store the defaults of the backup parameters */
    EE_LoadDefaultData();
    /* load the backup parameters in RAM from the non-volatile memory */
    LoadBackupEntries();
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     subindex                subindex of the requested object.
 \param     pObjEntry            handle to the dictionary object returned by
                                     OBJ_GetObjectHandle which was called before

 \brief    This function is called when a backup entry is written that the values
            can be stored in a non-volatile memory
*////////////////////////////////////////////////////////////////////////////////////////

void COE_WriteBackupEntry(UINT8 subindex, OBJCONST TOBJECT OBJMEM * pObjEntry)
{
    /* get the offset of the entry in the object's variable */
    UINT16 wordOffset = OBJ_GetEntryOffset(subindex, pObjEntry) >> 4;

    OBJCONST TSDOINFOENTRYDESC OBJMEM *pEntry =   OBJ_GetEntryDesc(pObjEntry, subindex);
    UINT16 wordSize = BIT2WORD(pEntry->BitLength);

    /* call the microcontroller specific function to store the data in the non-volatile memory
       and store the checksum in backup parameter checksum object */
    sBackupparameterhandling.u32Checksum = EE_WriteWordsToNonVolatileMemory(pObjEntry->NonVolatileOffset+wordOffset, &((UINT16 MBXMEM *) pObjEntry->pVarPtr)[wordOffset], wordSize);
}
#endif // BACKUP_PARAMETER_SUPPORTED

/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief     Update the Sync Error Indication
*////////////////////////////////////////////////////////////////////////////////////////
void COE_UpdateSyncErrorStatus(void)
{
#if MAX_PD_OUTPUT_SIZE > 0
#if !_PIC18
    if (sSyncManOutPar.u16CycleExceededCounter > 0 || sSyncManOutPar.u16SmEventMissedCounter > sErrorSettings.u16SyncErrorCounterLimit)
#else
    if (sSyncManOutPar.u16CycleExceededCounter > 0 || sSyncManOutPar.u16SmEventMissedCounter > (UINT16)MAX_SM_EVENT_MISSED)
#endif
    {
        sSyncManOutPar.u8SyncError = 1;
    }
    else
    {
        sSyncManOutPar.u8SyncError = 0;
    }

#endif

#if MAX_PD_INPUT_SIZE > 0
#if !_PIC18
    if (sSyncManInPar.u16CycleExceededCounter > 0 || sSyncManInPar.u16SmEventMissedCounter > sErrorSettings.u16SyncErrorCounterLimit)
#else
    if (sSyncManInPar.u16CycleExceededCounter > 0 || sSyncManInPar.u16SmEventMissedCounter > (UINT16)MAX_SM_EVENT_MISSED)
#endif
    {
        sSyncManInPar.u8SyncError = 1;
    }
    else
    {
        sSyncManInPar.u8SyncError = 0;
    }
#endif	

}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief     This function initialize the several objects
*////////////////////////////////////////////////////////////////////////////////////////

void COE_ObjInit(void)
{

#if MAX_PD_OUTPUT_SIZE > 0
    /* initialize the Sync Manager Output parameter object 0x1C32 */

    sSyncManOutPar.subindex0         = 32;
    /*
    subindex 1 contains the actual synchronization mode, it could be written
      from the master to switch between ECAT FreeRun and ECAT Synchron Mode
        if the slave supports both modes,
        in DC mode (selected by the DC registers) this value will be overwritten
        with SYNCTYPE_DCSYNC0 or SYNCTYPE_DCSYNC1 */
     /*default mode is ECAT Synchron Mode */
    sSyncManOutPar.u16SyncType     = SYNCTYPE_FREERUN;

    /* subindex 2 contains the cycle time of the application,
       in ECAT FreeRun mode it could be used for a timer interrupt to run the application,
        in ECAT Synchron mode it could be written from the master with its local cycle time
        that the slave can check if this cycle time is supported,
        in DC Mode this value will be overwritten with the DC cycle time register */
    sSyncManOutPar.u32CycleTime     = 0;

    /* the subindex 4 contains the supported synchronization types */

    sSyncManOutPar.u16SyncTypesSupported    = SYNCTYPE_FREERUNSUPP            /* ECAT FreeRun Mode is supported */
#if AL_EVENT_ENABLED

#if (PD_OUTPUT_CALC_AND_COPY_TIME == 0) || (PD_INPUT_CALC_AND_COPY_TIME == 0) || (MIN_PD_CYCLE_TIME == 0)
                                              | SYNCTYPE_TIMESVARIABLE        /* the execution times depend on the connected modules */
#endif
                                              | SYNCTYPE_SYNCHRONSUPP         /* ECAT Synchron Mode is supported */
#endif
#if DC_SUPPORTED
                                              | SYNCTYPE_DCSYNC0SUPP          /* DC Sync0 Mode is supported */
                                              | SYNCTYPE_DCSYNC1SUPP          /* DC Sync1 Mode is supported */
#if AL_EVENT_ENABLED
                                              | SYNCTYPE_SUBCYCLESUPP         /*Subordinated application cycles supported*/
#endif
#endif
    ;

    /* subindex 5 contains the minimum cycle time the slave is able to support,
       will be calculated dynamically because it depends on the connected modules
        (in this example we will make an online measurement in the ESC Interrupt Routine).
        For the sample application this value is set to MIN_PD_CYCLE_TIME */
    sSyncManOutPar.u32MinCycleTime = MIN_PD_CYCLE_TIME;
    /* only for DC Mode important: subindex 6 contains the minimum delay time the slave
       needs after receiving the SM2-event before the SYNC0(SYNC1) can be received without delays
       will be calculated dynamically because it depends on the connected modules
        (in this example we will make an online measurement in the ESC Interrupt Routine) */
    sSyncManOutPar.u32CalcAndCopyTime = (PD_OUTPUT_CALC_AND_COPY_TIME);

    /*subindex 8: trigger cycle time measurement*/
    sSyncManOutPar.u16GetCycleTime = 0;

    /*subindex 9: time from start driving outputs until outputs are valid*/
    sSyncManOutPar.u32DelayTime = (PD_OUTPUT_DELAY_TIME);

    /*subindex 11: reset the sm missed error counter*/
    sSyncManOutPar.u16SmEventMissedCounter = 0;

    /*subindex 12: reset the cycle exceed error counter*/
    sSyncManOutPar.u16CycleExceededCounter = 0;

    /*subindex 32: indicates if a synchronisation error has occurred*/
    sSyncManOutPar.u8SyncError = 0;
#endif //#if MAX_PD_OUTPUT_SIZE > 0

#if MAX_PD_INPUT_SIZE > 0
    /* initialize the Sync Manager Input parameter object 0x1C33 */
    sSyncManInPar.subindex0         = 32;
    /* default mode is ECAT Synchron Mode, if output size > 0 the inputs are updated with the SM2-event */
    sSyncManInPar.u16SyncType         = SYNCTYPE_FREERUN;

    
    /* subindex 2: same as 0x1C32:02 */
#if MAX_PD_OUTPUT_SIZE > 0
    sSyncManInPar.u32CycleTime     = sSyncManOutPar.u32CycleTime;
#else
    sSyncManInPar.u32CycleTime     = 0;
#endif

    /* subindex 4: same as 0x1C32:04 */
#if MAX_PD_OUTPUT_SIZE > 0
    sSyncManInPar.u16SyncTypesSupported    = sSyncManOutPar.u16SyncTypesSupported;
#elif MAX_PD_INPUT_SIZE > 0
    sSyncManInPar.u16SyncTypesSupported    = SYNCTYPE_FREERUNSUPP            /* ECAT FreeRun Mode is supported */
#if AL_EVENT_ENABLED
#if (PD_OUTPUT_CALC_AND_COPY_TIME == 0) || (PD_INPUT_CALC_AND_COPY_TIME == 0) || (MIN_PD_CYCLE_TIME == 0)
                                              | SYNCTYPE_TIMESVARIABLE        /* the execution times depend on the connected modules */
#endif

                                              | SYNCTYPE_SYNCHRONSUPP         /* ECAT Synchron Mode is supported */
#endif
#if DC_SUPPORTED
                                              | SYNCTYPE_DCSYNC0SUPP          /* DC Sync0 Mode is supported */
                                              | SYNCTYPE_DCSYNC1SUPP          /* DC Sync1 Mode is supported */
#if AL_EVENT_ENABLED
                                              | SYNCTYPE_SUBCYCLESUPP         /*Subordinated application cycles supported*/
#endif
                                              | SYNCTYPE_SHIFTBYSYNC1SUPP     /* Input shift by Sync1 supported*/
#endif
    ;
#endif //#else #if MAX_PD_OUTPUT_SIZE > 0

    /* subindex 5: same as 0x1C32:05 */
    sSyncManInPar.u32MinCycleTime = MIN_PD_CYCLE_TIME;
    /* subindex 6: delay read inputs, calculation and copy to SM buffer*/
    sSyncManInPar.u32CalcAndCopyTime = (PD_INPUT_CALC_AND_COPY_TIME);
    /*subindex 8: trigger cycle time measurement*/
    sSyncManInPar.u16GetCycleTime = 0;
    /*subindex 9: delay to prepare input latch*/
    sSyncManInPar.u32DelayTime = (PD_INPUT_DELAY_TIME);

    /*subindex 11: reset the sm missed error counter*/
    sSyncManInPar.u16SmEventMissedCounter = 0;

    /*subindex 12: reset the cycle exceed error counter*/
    sSyncManInPar.u16CycleExceededCounter = 0;

    /*subindex 32: incremented if a synchronisation error has occurred*/
    sSyncManInPar.u8SyncError = 0;
#endif //#if MAX_PD_INPUT_SIZE > 0

    /*Indicate no user specified Sync mode*/
    bSyncSetByUser = FALSE;

#if !STATIC_OBJECT_DIC && COE_SUPPORTED
    {
    UINT16 result = COE_ObjDictionaryInit();
    if(result != 0)
    {
        /*clear already linked objects*/
        COE_ClearObjDictionary();
    }
    }
#endif

#if SDO_RES_INTERFACE
    u8PendingSdo = 0;
    bStoreCompleteAccess = FALSE;
    u16StoreIndex   =   0;
    u8StoreSubindex = 0;
    u32StoreDataSize = 0;
    pStoreData = NULL;
    pSdoPendFunc    = NULL;
#endif

#if SEGMENTED_SDO_SUPPORTED
    pSdoSegData = NULL;
#endif

#if BACKUP_PARAMETER_SUPPORTED
    /* check, if we have to initialize the default data of the backup entries in the non-volatile memory */
    if ( !EE_IsDefaultDataInitialized() )
    {
        /* the default data in the non-volatile memory has to be initialized */
        InitDefaultEntries();
    }

    /* load the backup entries from the non-volatile memory */
    LoadBackupEntries();
    /* initialize the backup parameter checksum object */
    sBackupparameterhandling.u32Checksum = EE_GetChecksum();
#endif // BACKUP_PARAMETER_OBJECT

#if DC_SUPPORTED
    {
        UINT32 EscFeature = 0;
        HW_EscReadDWord(EscFeature, ESC_FEATURES_OFFSET);
        EscFeature = SWAPDWORD(EscFeature);

        if ((EscFeature & ESC_DC_32BIT_MASK) > 0)
        {
            b32BitDc = FALSE;
        }
        else
        {
            b32BitDc = TRUE;

            HW_EscReadDWord(u32LastDc32Value, ESC_SYSTEMTIME_OFFSET);
        }

        u32CheckForDcOverrunCnt = CHECK_DC_OVERRUN_IN_MS;
    }
#endif
}

#if !STATIC_OBJECT_DIC
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    0               object successful added to object dictionary
            ALSTATUSCODE_XX add object failed

 \brief    This function adds an object to the object dictionary
 *////////////////////////////////////////////////////////////////////////////////////////
UINT16 COE_AddObjectToDic(TOBJECT OBJMEM * pNewObjEntry)
{
    if(pNewObjEntry != NULL)
    {
        if(ObjDicList == NULL)
        {
            /* Object dictionary is empty */
            ObjDicList = pNewObjEntry;
            ObjDicList->pNext = NULL;
            ObjDicList->pPrev = NULL;
            return 0;
        }
        else if(ObjDicList->Index > pNewObjEntry->Index)
        {
            /*insert new object dictionary head*/
            pNewObjEntry->pPrev = NULL;
            pNewObjEntry->pNext = ObjDicList;
            ObjDicList->pPrev = pNewObjEntry;
            ObjDicList = pNewObjEntry;
            return 0;
        }
        else
        {
            TOBJECT    OBJMEM * pDicEntry = ObjDicList;
            while(pDicEntry != NULL)
            {
                if(pDicEntry->Index == pNewObjEntry->Index)
                {
                    /*object already exists in object dictionary*/
                    return ALSTATUSCODE_UNSPECIFIEDERROR;
                }
                else if(pDicEntry->Index > pNewObjEntry->Index)
                {
                    pNewObjEntry->pPrev = pDicEntry->pPrev;
                    pNewObjEntry->pNext = pDicEntry;

                    if(pDicEntry->pPrev != NULL)
                        pDicEntry->pPrev->pNext = pNewObjEntry;

                    pDicEntry->pPrev = pNewObjEntry;

                    return 0;
                }
                else if(pDicEntry->pNext == NULL)
                {
                    /*Last entry reached => add object to list tail*/
                    pDicEntry->pNext = pNewObjEntry;
                    pNewObjEntry->pPrev = pDicEntry;
                    pNewObjEntry->pNext = NULL;
                    return 0;
                }
                else
                {
                    /*The new object index is smaller than the current index. Get next object handle.*/
                    pDicEntry = pDicEntry->pNext;
                }
            }
        }
    }
    return ALSTATUSCODE_UNSPECIFIEDERROR;
}
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function removes an object to the object dictionary
*////////////////////////////////////////////////////////////////////////////////////////
void COE_RemoveDicEntry(UINT16 index)
{
    TOBJECT    OBJMEM * pDicEntry = ObjDicList;

    while(pDicEntry != NULL)
    {
        if(pDicEntry->Index == index)
        {
            TOBJECT OBJMEM *pPrevEntry = pDicEntry->pPrev;
            TOBJECT OBJMEM *pNextEntry = pDicEntry->pNext;

            if(pPrevEntry != NULL)
            {
                pPrevEntry->pNext = pNextEntry;
            }

            if(pNextEntry != NULL)
            {
                pNextEntry->pPrev = pPrevEntry;
            }

            pDicEntry->pPrev = NULL;
            pDicEntry->pNext = NULL;
            /*Update Object dictionary pointer if list head was removed*/
            if(pDicEntry->Index == ObjDicList->Index)
            {
                ObjDicList = pNextEntry;
            }
            return;
        }

        pDicEntry = pDicEntry->pNext;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function clear the object dictionary
*////////////////////////////////////////////////////////////////////////////////////////
void COE_ClearObjDictionary(void)
{
    TOBJECT OBJMEM * pObjEntry = (TOBJECT OBJMEM *) ObjDicList;
    UINT16 Index = 0;

    while(pObjEntry != NULL)
    {
        Index = pObjEntry->Index;
        pObjEntry = pObjEntry->pNext;

        COE_RemoveDicEntry(Index);
    }
    ObjDicList = NULL;
}


UINT16 AddObjectsToObjDictionary(TOBJECT OBJMEM * pObjEntry)
{
    UINT16 result = 0;
    TOBJECT OBJMEM * pEntry = (TOBJECT OBJMEM *)pObjEntry;

    while(pEntry->Index != 0xFFFF)
    {
        result = COE_AddObjectToDic(pEntry);

        if(result != 0)
        {
            return result;
        }

        pEntry++;
    }

    return result;

}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    0               object dictionary created successful
            ALSTATUSCODE_XX create object dictionary failed

 \brief    This function initialize the object dictionary
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 COE_ObjDictionaryInit(void)
{
    UINT16 result = 0;

    /*Reset object dictionary pointer*/
    ObjDicList = NULL;

    result = AddObjectsToObjDictionary((TOBJECT OBJMEM *) GenObjDic);

    if(result != 0)
    {
        return result;
    }
    
    if(ApplicationObjDic != NULL)
    {
        result = AddObjectsToObjDictionary((TOBJECT OBJMEM *) ApplicationObjDic);
    }



    return result;
}
#endif //#if !STATIC_OBJECT_DIC


/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    is called for background calculations which should not influence the
             ECAT_Application in synchronous modes
*////////////////////////////////////////////////////////////////////////////////////////

void COE_Main(void)
{
#if SDO_RES_INTERFACE
     UINT8 abort = 0;
     
    if(pSdoPendFunc != NULL)
    {
        abort = pSdoPendFunc(u16StoreIndex,u8StoreSubindex,u32StoreDataSize,pStoreData,bStoreCompleteAccess);

        if(abort != ABORTIDX_WORKING)
        {
            switch(u8PendingSdo)
            {
#if SEGMENTED_SDO_SUPPORTED
                case SDO_PENDING_SEG_WRITE:
                    if(pSdoSegData)
                    {
                        /* the allocated buffer can be released */
                        FREEMEM( (UINT16 VARMEM *) pSdoSegData );
                        pSdoSegData = NULL;
                    }
#endif
                case SDO_PENDING_WRITE:
                     /*send SDO Download Response*/
                    SDOS_SdoRes(abort, 0, NULL);
                break;

                case SDO_PENDING_SEG_READ:
                case SDO_PENDING_READ:
                    /* send SDO upload response */
                      SDOS_SdoRes(abort, u32StoreDataSize, pStoreData);
                    break;

            }

            u8PendingSdo = 0;
            u16StoreIndex = 0;
            u8StoreSubindex = 0;
            u32StoreDataSize = 0;
            pStoreData = NULL;
            bStoreCompleteAccess = 0;
            pSdoPendFunc = NULL;
        }
    }
#endif
}

#endif //#if COE_SUPPORTED
/** @} */
