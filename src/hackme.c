/*
 * @(#)hackme.c
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

#include "palm.h"

/**
 * DlkGetSyncInfo() system TRAP routine.
 */
Err    
NewDlkGetSyncInfo(UInt32           *succSyncDateP, 
                  UInt32           *lastSyncDateP, 
                  DlkSyncStateType *syncStateP, 
                  Char             *nameBufP, 
                  Char             *logBufP, 
                  Int32            *logLenP)
{
  Err               result = errNone;
  DmSearchStateType stateInfo;
  UInt16            appCard;
  LocalID           appdbID;
  Err               error;

  // lets ensure the hackme application is installed (we need it)
  error =
    DmGetNextDatabaseByTypeCreator(true, &stateInfo,
                                   'appl', appCreator, false,
                                   &appCard, &appdbID);

  // hackme is installed, continue
  if (error == errNone) {

    Err (*trapReplacement)(UInt32 *, 
                           UInt32 *, 
                           DlkSyncStateType *, 
                           Char *, 
                           Char *, 
                           Int32 *);
    DmOpenRef   dbRef;
    MemHandle   codeH;

    // open the hackme database 
    dbRef = DmOpenDatabase(appCard, appdbID, dmModeReadOnly);

    // lock replacement routine
    codeH           = DmGet1Resource('code', 0x0003);
    trapReplacement = MemHandleLock(codeH);

    // ok, lets do our "replacement" call
    result = 
      trapReplacement(succSyncDateP,lastSyncDateP,syncStateP,
                      nameBufP,logBufP,logLenP);

    // close database
    MemHandleUnlock(codeH);
    DmReleaseResource(codeH);
    DmCloseDatabase(dbRef);
  }

  // hotsync is not installed, remove configuration
  else {

    Err (*trapPtr)(UInt32 *, 
                   UInt32 *, 
                   DlkSyncStateType *, 
                   Char *, 
                   Char *, 
                   Int32 *);

    // un-install
    FtrGet(appCreator,ftrTrapAddressSystem, (UInt32 *)&trapPtr);
    SysSetTrapAddress(sysTrapDlkGetSyncInfo, trapPtr);

    FtrUnregister(appCreator, ftrVersion);
    FtrUnregister(appCreator, ftrTrapAddressSystem);

    // call the system routine
    result = trapPtr(succSyncDateP,lastSyncDateP,syncStateP,
                     nameBufP,logBufP,logLenP);
  }

  // return the system call result
  return result;
}

/**
 * DlkGetSyncInfo() replacement routine. 
 */
Err    
NewDlkGetSyncInfoReplacement(UInt32           *succSyncDateP, 
                             UInt32           *lastSyncDateP, 
                             DlkSyncStateType *syncStateP, 
                             Char             *nameBufP, 
                             Char             *logBufP, 
                             Int32            *logLenP)
{
  Err  result = errNone;
  Err  (*trapPtr)(UInt32 *, 
                  UInt32 *, 
                  DlkSyncStateType *, 
                  Char *, 
                  Char *, 
                  Int32 *);

  // get the trap pointer
  FtrGet(appCreator,ftrTrapAddressSystem, (UInt32 *)&trapPtr);

  // call the system routine
  result = trapPtr(succSyncDateP,lastSyncDateP,syncStateP,
                   nameBufP,logBufP,logLenP);

  // lets fake it
  {
    DmOpenRef dbRef;
    UInt16    appCard;
    LocalID   appdbID;
    UInt32    appCr8r;
    
    // whats the current app - get the creator?
    SysCurAppDatabase(&appCard, &appdbID);
    DmDatabaseInfo(appCard, appdbID, NULL, NULL, NULL, NULL, 
                   NULL, NULL, NULL, NULL, NULL, NULL, &appCr8r);

    // lets locate the "sync" name database :)
    dbRef = DmOpenDatabaseByTypeCreator(syncType, appCreator, dmModeReadOnly);
    if (dbRef != NULL) {

      MemHandle strH;
      Char      *str;

      // copy across the appropriate string :)
      strH = DmGet1Resource(appCr8r, 1);
      if (strH != NULL) {
        str  = MemHandleLock(strH);

        StrCopy(nameBufP, str);
        nameBufP[MemHandleSize(strH)] = '\0';   // ensure it ends with \0

        MemHandleUnlock(strH);
        DmReleaseResource(strH);
      }

      // close the database
      DmCloseDatabase(dbRef);
    }
  }

  // return the system call result
  return result;
}
