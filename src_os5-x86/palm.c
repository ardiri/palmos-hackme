/*
 * @(#)palm.c
 *
 * Copyright 2000, Aaron Ardiri (mailto:aaron@ardiri.com)
 * All rights reserved.
 *
 * This  file was  generated as part of the "|HaCkMe|" system extension
 * that has been specifically designed for the Palm Computing Platform.
 *
 *   http://www.palm.com/
 *
 * The contents of this file are confidential and proprietary in nature
 * ("Confidential Information"). Redistribution or modification without
 * prior consent of the original author(s) is prohibited.
 */

#include <PalmOS.h>

/**
 * The Palm Computing Platform entry routine (mainline).
 *
 * @param cmd         a word value specifying the launch code.
 * @param cmdPBP      pointer to a structure associated with the launch code.
 * @param launchFlags additional launch flags.
 * @return zero if launch successful, non zero otherwise.
 */
UInt32
PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
  UInt32 result = 0;

  // what type of launch was this?
  switch (cmd)
  {
    case sysAppLaunchCmdNormalLaunch:
    case sysAppLaunchCmdSystemReset:
         {
           UInt32 romVersion;

           // get the rom version
           FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

           // OS5: (palm simulator)
           if (romVersion >= sysMakeROMVersion(5,0,0,sysROMStageDevelopment,0))
           {

typedef struct
{
UInt8   paddingA[4];
UInt32  userID;
UInt8   paddingB[32];
UInt8   userNameLen;
UInt8   paddingC[7];       // x86 padding is slightly different! [for PalmSim]
UInt32  data;
} DlkUserInfoType_ARM;

#define SIZE_DLK        52 // x86 padding is slightly different! [for PalmSim]
#define DEFAULT_HOTSYNC "PalmOS Simulator"

             // do we need to set the username?
             DmOpenRef db_ref;

             db_ref = DmOpenDatabaseByTypeCreator('pref', 'psys', dmModeReadWrite);
             if (db_ref)
             {
               MemHandle            memHandle;
               UInt16               size;
               DlkUserInfoType_ARM *ptr;
               DlkUserInfoType_ARM *buf;
               Char                 str[32];
               UInt16               len;

               // the default hotsync username
               StrCopy(str, DEFAULT_HOTSYNC);
               len = StrLen(str);

               memHandle = DmGetResource('psys', 4);
               if (memHandle == NULL) goto USERNAME_ABORT;
  		         size = MemHandleSize(memHandle);
    		       ptr = (DlkUserInfoType_ARM *)MemHandleLock(memHandle);
               if (ptr->userID == 0)
               {

#define __write_byte32(addr, value) \
( ((unsigned char *)(addr))[3] = (unsigned char)((unsigned long)(value) >> 24), \
  ((unsigned char *)(addr))[2] = (unsigned char)((unsigned long)(value) >> 16), \
  ((unsigned char *)(addr))[1] = (unsigned char)((unsigned long)(value) >>  8), \
  ((unsigned char *)(addr))[0] = (unsigned char)((unsigned long)(value)) )

                 // lets prepare our structure for writing back to device
                 buf = MemPtrNew(SIZE_DLK + len);
                 MemSet(buf, SIZE_DLK + len, 0);
                 MemMove(buf, ptr, size);           // copy existing stuff (important)
                 __write_byte32(&buf->userID, 2);
                 buf->userNameLen = len;
                 StrCopy((char *)&buf->data, str);  // write the new user name

                 // must prepare the new resource now
                 MemHandleUnlock(memHandle);
                 memHandle = DmResizeResource(memHandle, SIZE_DLK + len);
      		       ptr = (DlkUserInfoType_ARM *)MemHandleLock(memHandle);
			           DmWrite(ptr, 0, buf, SIZE_DLK + len);
               }
               MemHandleUnlock(memHandle);
             }

USERNAME_ABORT:

             DmCloseDatabase(db_ref);
           }
         }
         break;

    default:
         break;
  }

  return result;
}