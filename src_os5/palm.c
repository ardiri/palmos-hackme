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

#include "palm.h"

// interface
static Boolean mainFormEventHandler(EventType *);
static Boolean infoFormEventHandler(EventType *);

/**
 * The Form:mainForm event handling routine.
 *
 * @param event the event to process.
 * @return true if the event was handled, false otherwise.
 */
static Boolean
mainFormEventHandler(EventType *event)
{
  Boolean processed = false;

  switch (event->eType)
  {
    case frmOpenEvent:
         {
           FormType *frm;
           char     **strListContents;

           frm = FrmGetActiveForm();
           FrmDrawForm(frm);

           // populate the list
           {
             DmSearchStateType stateInfo;
             Err               error;
             UInt16            card, i, pos, size, passCount;
             LocalID           dbID;
             Char              appName[32];
             ListType          *lstApplicationList;

             size            = 0;
             strListContents = NULL;

             lstApplicationList =
               (ListType *)FrmGetObjectPtr(frm,
                 FrmGetObjectIndex(frm, mainFormAppList));

             // do a blatent brute-force search
             for (passCount=0; passCount < 2; passCount++)
             {
               // SECOND PASS - allocate memory for list
               if (passCount == 1)
               {
                 strListContents = (Char **)MemPtrNew(size * sizeof(Char *));
                 for (pos=0; pos<size; pos++)
                   strListContents[pos] = (Char *)MemPtrNew(32 * sizeof(Char));
               }

               error =
                 DmGetNextDatabaseByTypeCreator(true, &stateInfo,
                                                'appl', NULL, false,
                                                &card, &dbID);
               size = 0;
               while (error == errNone)
               {
                 // application must be RAM based :)
                 if (MemLocalIDKind(dbID) == memIDHandle)
                 {
                   // SECOND PASS - add info to the list
                   if (passCount == 1)
                   {
                     // extract the database info :)
                     DmDatabaseInfo(card, dbID, appName,
                                    NULL, NULL, NULL, NULL, NULL,
                                    NULL, NULL, NULL, NULL, NULL);

                     // found out where it should go
                     pos = 0;
                     while ((pos < size) &&
                            (StrCompare(appName,
                                        strListContents[pos]) > 0))
                     {
                       pos++;
                     }

                     // do we need to shift a few things?
                     if (pos < size)
                     {
                       // move em down
                       for (i=size; i>pos; i--)
                         StrCopy(strListContents[i], strListContents[i-1]);
                     }

                     // copy it to the list
                     StrCopy(strListContents[pos], appName);
                   }

                   size++;
                 }

                 // next sequence
                 error =
                   DmGetNextDatabaseByTypeCreator(false, &stateInfo,
                                                  'appl', NULL, false,
                                                  &card, &dbID);
               }
             }

             // configure the list
             LstSetListChoices(lstApplicationList, strListContents, size);
             FtrSet(appCreator, ftrListMemory, (UInt32)strListContents);
           }

           // select the first item in the list
           {
             EventType event;

             MemSet(&event, sizeof(EventType), 0);
             event.eType = popSelectEvent;
             event.data.popSelect.controlID      = mainFormAppTrigger;
             event.data.popSelect.controlP       =
               (ControlType *)FrmGetObjectPtr(frm,
                 FrmGetObjectIndex(frm, mainFormAppTrigger));
             event.data.popSelect.listP          =
               (ListType *)FrmGetObjectPtr(frm,
                 FrmGetObjectIndex(frm, mainFormAppList));
             event.data.popSelect.selection      = 0;
             event.data.popSelect.priorSelection = noListSelection;

             // PalmOS5+ does some funky stuff here, lets do it this way *g*
             mainFormEventHandler(&event);
             CtlSetLabel(event.data.popSelect.controlP, strListContents[0]);
             CtlDrawControl(event.data.popSelect.controlP);
           }
         }
         processed = true;
         break;

    case frmUpdateEvent:
         FrmDrawForm(FrmGetActiveForm());

         // draw seperators
         WinDrawLine(  0, 145, 159, 145);
         WinDrawLine(  0, 146, 159, 146);

         processed = true;
         break;

    case ctlSelectEvent:

         switch (event->data.ctlSelect.controlID)
         {
           case mainFormSaveButton:
                {
                  FormType  *frm;
                  ListType  *lstApplicationList;
                  Char      appName[32];
                  DmOpenRef dbRef;
                  UInt16    appCard;
                  LocalID   appdbID;
                  UInt32    appCr8r;

                  frm = FrmGetActiveForm();

                  lstApplicationList =
                    (ListType *)FrmGetObjectPtr(frm,
                      FrmGetObjectIndex(frm, mainFormAppList));

                  // what appname did they select?
                  StrCopy(appName,
                    LstGetSelectionText(lstApplicationList,
                                        LstGetSelection(lstApplicationList)));

                  // get the dbID, card and creator of app selected
                  appCard = 0;
                  appdbID = DmFindDatabase(appCard, appName);
                  if (appdbID == NULL)
                  {
                    appCard = 1;
                    appdbID = DmFindDatabase(appCard, appName);
                  }
                  DmDatabaseInfo(appCard, appdbID, NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL, NULL, NULL, NULL, &appCr8r);

                  {
                    Err       error;
                    Char      userName[32];
                    FieldType *fldUserName;

                    // assume everything is ok
                    error = errNone;

                    fldUserName =
                      (FieldType *)FrmGetObjectPtr(frm,
                        FrmGetObjectIndex(frm, mainFormHotsyncField));
                    MemSet(userName, 32, 0);
                    {
                      MemHandle textH = FldGetTextHandle(fldUserName);
                      FldSetTextHandle(fldUserName, NULL);
                      StrCopy(userName, MemHandleLock(textH));
                      userName[MemHandleSize(textH)] = '\0';
                      MemHandleUnlock(textH);
                      FldSetTextHandle(fldUserName, textH);
                    }

                    // create/open the database
                    DmCreateDatabase(0, KEY_FILENAME,
                                     appCreator, syncType, true);
                    dbRef =
                      DmOpenDatabaseByTypeCreator(syncType, appCreator, dmModeWrite);
                    if (dbRef != NULL)
                    {
                      // must ignore the check for the app
                      if (appCr8r != appCreator)
                      {
                        Int16     resIndex;
                        MemHandle strH;

                        // find the resource (if possible)
                        resIndex = DmFindResource(dbRef, appCr8r, 1, NULL);
                        if (resIndex != -1)
                          DmRemoveResource(dbRef, resIndex);  // kill it

                        // create the new resource
                        strH =
                          DmNewResource(dbRef, appCr8r, 1, StrLen(userName)+1);
                        if (strH != NULL)
                          DmWrite((MemPtr)MemHandleLock(strH),
                                  0, userName, StrLen(userName)+1);
                      }
                    }

                    // close the database
                    DmCloseDatabase(dbRef);
                  }
                }
                processed = true;
                break;

           default:
                break;
         }
         break;

    case popSelectEvent:
         {
           FormType  *frm;
           Char      appName[32];
           DmOpenRef dbRef;
           UInt16    appCard;
           LocalID   appdbID;
           UInt32    appCr8r;
           MemHandle defaultIconHandle;

           //
           // DRAW FORM
           //

           frm = FrmGetActiveForm();
           FrmDrawForm(frm);
           defaultIconHandle = DmGet1Resource('Tbmp', bitmapIconDefault);

           // draw seperators
           WinDrawLine(  0, 145, 159, 145);
           WinDrawLine(  0, 146, 159, 146);

           // what appname did they select?
           StrCopy(appName,
             LstGetSelectionText(event->data.popSelect.listP,
                                 event->data.popSelect.selection));

           // get the dbID, card and creator of app selected
           appCard = 0;
           appdbID = DmFindDatabase(appCard, appName);
           if (appdbID == NULL)
           {
             appCard = 1;
             appdbID = DmFindDatabase(appCard, appName);
           }
           DmDatabaseInfo(appCard, appdbID, NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL, NULL, NULL, NULL, &appCr8r);

           //
           // APP INFO
           //

           // open the database
           dbRef = DmOpenDatabase(appCard, appdbID, dmModeReadOnly);

           // draw the icon :)
           {
             const CustomPatternType erase = { 0,0,0,0,0,0,0,0 };
             const RectangleType     rect  = {{   8,  54 }, {  32,  32 }};
             MemHandle iconHandle;

             WinSetPattern(&erase);
             WinFillRectangle(&rect, 0);

             iconHandle = DmGet1Resource('tAIB', 1000);
             if (iconHandle != NULL)
             {
               WinDrawBitmap(
                 (BitmapType *)MemHandleLock(iconHandle), 8, 54);
               MemHandleUnlock(iconHandle);
               DmReleaseResource(iconHandle);
             }
             else
             {
               WinDrawBitmap(
                 (BitmapType *)MemHandleLock(defaultIconHandle), 8, 54);
               MemHandleUnlock(defaultIconHandle);
             }
           }

           // close the database
           DmCloseDatabase(dbRef);

           //
           // HOTSYNC USER NAME
           //

           {
             Char      userName[32];
             FieldType *fldUserName;

             fldUserName =
               (FieldType *)FrmGetObjectPtr(frm,
                 FrmGetObjectIndex(frm, mainFormHotsyncField));
             MemSet(userName, 32, 0);
             DlkGetSyncInfo(NULL, NULL, NULL, userName, NULL, NULL);

             // open the database
             dbRef =
               DmOpenDatabaseByTypeCreator(syncType,appCreator,dmModeReadOnly);
             if (dbRef != NULL)
             {
               // must ignore the check for the app
               if (appCr8r != appCreator)
               {
                 MemHandle strH;

                 // locate the string (if available)
                 strH = DmGet1Resource(appCr8r, 1);
                 if (strH != NULL)
                 {
                   StrCopy(userName, MemHandleLock(strH));
                   userName[MemHandleSize(strH)] = '\0';    // ensure it ends \0
                   MemHandleUnlock(strH);
                   DmReleaseResource(strH);
                 }
               }

               // close the database
               DmCloseDatabase(dbRef);
             }

             FldGrabFocus(fldUserName);

             // update the field
             {
               MemHandle textH = FldGetTextHandle(fldUserName);
               FldSetTextHandle(fldUserName, NULL);

               if (textH != NULL) MemHandleFree(textH);
               textH = MemHandleNew(32);
               StrCopy(MemHandleLock(textH), userName);
               MemHandleUnlock(textH);

               FldSetTextHandle(fldUserName, textH);
             }
             FldDrawField(fldUserName);
           }

           DmReleaseResource(defaultIconHandle);
         }
         break;

    case frmCloseEvent:

         // free the memory
         {
           FormType *frm;
           ListType *lstApplicationList;
           Char     **strListContents;
           UInt16   i, size;

           frm = FrmGetActiveForm();
           lstApplicationList =
             (ListType *)FrmGetObjectPtr(frm,
               FrmGetObjectIndex(frm, mainFormAppList));

           size = LstGetNumberOfItems(lstApplicationList);
           FtrGet(appCreator, ftrListMemory, (UInt32 *)&strListContents);

           for (i=0; i<size; i++)
           {
             MemPtrFree(strListContents[i]);
           }
           MemPtrFree(strListContents);
         }
         break;

    default:
         break;
  }

  return processed;
}

/**
 * The Form:infoForm event handling routine.
 *
 * @param event the event to process.
 * @return true if the event was handled, false otherwise.
 */
static Boolean
infoFormEventHandler(EventType *event)
{
  Boolean processed = false;

  switch (event->eType)
  {
    case frmOpenEvent:
         FrmDrawForm(FrmGetActiveForm());
         processed = true;
         break;

    case ctlSelectEvent:

         switch (event->data.ctlSelect.controlID)
         {
           case infoFormOkButton:

                // send a close event
                {
                  EventType event;

                  MemSet(&event, sizeof(EventType), 0);
                  event.eType = frmCloseEvent;
                  event.data.frmClose.formID = FrmGetActiveFormID();
                  EvtAddEventToQueue(&event);
                }
                processed = true;
                break;

           default:
                break;
         }
         break;

    default:
         break;
  }

  return processed;
}

/**
 * The Palm Computing Platform initialization routine.
 */
void
InitApplication()
{
  // goto the main form
  FrmGotoForm(mainForm);
}

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
    case sysAppLaunchCmdSystemReset:
         {
           UInt32 version;

           // lets check if the libary has been installed (OS4 only)
           if (FtrGet(appCreator,ftrVersion,&version) == ftrErrNoSuchFeature)
           {
             UInt32 romVersion;

             // get the rom version
             FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

             // OS4:
             if (romVersion < sysMakeROMVersion(5,0,0,sysROMStageDevelopment,0))
             {
               UInt32    trapAddress, size;
               MemHandle codeH;
               UInt8     *ptr;

               // lets keep track of the old trap address for DlkGetSyncInfo();
               trapAddress = (UInt32)SysGetTrapAddress(sysTrapDlkGetSyncInfo);
               FtrSet(appCreator, ftrTrapAddressSystem, (UInt32)trapAddress);

               // lets configure the new handler
               codeH = DmGet1Resource('code', 0x0002);
               size  = MemHandleSize(codeH);
               ptr   = (UInt8 *)MemPtrNew(size);
               MemMove(ptr, MemHandleLock(codeH), size);
               MemHandleUnlock(codeH);
               DmReleaseResource(codeH);

               // replace the DlkGetSyncInfo() trap
               MemPtrSetOwner(ptr, 0);         // the system owns this now
               SysSetTrapAddress(sysTrapDlkGetSyncInfo, ptr);
             }

             // OS5:
             else
             {

typedef struct
{
  UInt8   paddingA[4];
  UInt32  userID;
  UInt8   paddingB[32];
  UInt8   userNameLen;
//UInt8   paddingC[7];       // x86 padding is slightly different! [for PalmSim]
  UInt8   paddingC[3];       // arm padding is slightly different! [for device]
  UInt32  data;
} DlkUserInfoType_ARM;

//#define SIZE_DLK        52 // x86 padding is slightly different! [for PalmSim]
//#define DEFAULT_HOTSYNC "PalmOS Simulator"

  #define SIZE_DLK        48 // arm padding is slightly different! [for device]
  #define DEFAULT_HOTSYNC "|HaCkMe|"

               // do we need to set the username?
               DmOpenRef db_ref;

               db_ref = DmOpenDatabaseByTypeCreator('pref', 'psys', dmModeReadWrite);
               if (db_ref)
               {
                 MemHandle            memHandle;
                 UInt16               size;
                 DlkUserInfoType_ARM *ptr;

                 memHandle = DmGetResource('psys', 4);
    		         size = MemHandleSize(memHandle);
      		       ptr = (DlkUserInfoType_ARM *)MemHandleLock(memHandle);
                 if (ptr->userID == 0)
                 {
                   DlkUserInfoType_ARM *buf;
                   Char                 str[32];
                   UInt16               len;

#define __write_byte32(addr, value) \
	( ((unsigned char *)(addr))[3] = (unsigned char)((unsigned long)(value) >> 24), \
	  ((unsigned char *)(addr))[2] = (unsigned char)((unsigned long)(value) >> 16), \
	  ((unsigned char *)(addr))[1] = (unsigned char)((unsigned long)(value) >>  8), \
	  ((unsigned char *)(addr))[0] = (unsigned char)((unsigned long)(value)) )

                   // the default hotsync username
                   StrCopy(str, DEFAULT_HOTSYNC);
                   len = StrLen(str);

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
               DmCloseDatabase(db_ref);
             }

             // set feature, saying we are installed
             version = sysMakeROMVersion(1,0,0,sysROMStageRelease,0);
             FtrSet(appCreator, ftrVersion, (UInt32)version);
           }
         }
         break;

    case sysAppLaunchCmdNormalLaunch:
         {
           InitApplication();
           EventLoop();
           EndApplication();
         }
         break;

    default:
         break;
  }

  return result;
}

/**
 * The application event handling routine.
 *
 * @param event the event to process.
 * @return true if the event was handled, false otherwise.
 */
Boolean
ApplicationHandleEvent(EventType *event)
{
  Boolean processed = false;

  switch (event->eType)
  {
    case frmLoadEvent:
         {
           UInt16   formID = event->data.frmLoad.formID;
           FormType *frm   = FrmInitForm(formID);

           FrmSetActiveForm(frm);
           switch (formID)
           {
             case mainForm:
                  FrmSetEventHandler(frm,
                    (FormEventHandlerPtr)mainFormEventHandler);

                  processed = true;
                  break;

             case infoForm:
                  FrmSetEventHandler(frm,
                    (FormEventHandlerPtr)infoFormEventHandler);

                  processed = true;
                  break;

             default:
                  break;
           }
         }
         break;

    case menuEvent:

         switch (event->data.menu.itemID)
         {
           case menuItemAbout:
                ApplicationDisplayDialog(infoForm);

                processed = true;
                break;

           default:
                break;
         }
         break;

    case ctlSelectEvent:

         switch (event->data.ctlSelect.controlID)
         {
           case globalFormAboutButton:

                // regenerate menu event
                {
                  EventType event;

                  MemSet(&event, sizeof(EventType), 0);
                  event.eType            = menuEvent;
                  event.data.menu.itemID = menuItemAbout;
                  EvtAddEventToQueue(&event);
                }
                processed = true;
                break;

           default:
                break;

         }
         break;

    default:
         break;
  }

  return processed;
}

/**
 * Display a MODAL dialog to the user (this is a modified FrmDoDialog)
 *
 * @param formID the ID of the form to display.
 */
void
ApplicationDisplayDialog(UInt16 formID)
{
  FormActiveStateType frmCurrState;
  FormType            *frmActive      = NULL;
  WinHandle           winDrawWindow   = NULL;
  WinHandle           winActiveWindow = NULL;
  UInt32              romVersion;

  // get the rom version
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

  // save the active form/window
  if (romVersion >= sysMakeROMVersion(3,0,0,sysROMStageRelease,0)) {
    FrmSaveActiveState(&frmCurrState);
  }
  else {
    frmActive       = FrmGetActiveForm();
    winDrawWindow   = WinGetDrawWindow();
    winActiveWindow = WinGetActiveWindow();  // < palmos3.0, manual work
  }

  {
    EventType event;
    UInt16    err;
    Boolean   keepFormOpen;

    MemSet(&event, sizeof(EventType), 0);

    // send a load form event
    event.eType = frmLoadEvent;
    event.data.frmLoad.formID = formID;
    EvtAddEventToQueue(&event);

    // send a open form event
    event.eType = frmOpenEvent;
    event.data.frmLoad.formID = formID;
    EvtAddEventToQueue(&event);

    // handle all events here (trap them before the OS does) :)
    keepFormOpen = true;
    while (keepFormOpen)
    {
      EvtGetEvent(&event, evtWaitForever);

      // this is our exit condition! :)
      keepFormOpen = (event.eType != frmCloseEvent);

      if (!SysHandleEvent(&event))
        if (!MenuHandleEvent(0, &event, &err))
          if (!ApplicationHandleEvent(&event))
            FrmDispatchEvent(&event);

      if (event.eType == appStopEvent)
      {
        keepFormOpen = false;
        EvtAddEventToQueue(&event);  // tap "applications", need to exit
      }
    }
  }

  // restore the active form/window
  if (romVersion >= sysMakeROMVersion(3,0,0,sysROMStageRelease,0))
  {
    FrmRestoreActiveState(&frmCurrState);
  }
  else {
    FrmSetActiveForm(frmActive);
    WinSetDrawWindow(winDrawWindow);
    WinSetActiveWindow(winActiveWindow);     // < palmos3.0, manual work
  }
}

/**
 * The Palm Computing Platform event processing loop.
 */
void
EventLoop()
{
  EventType event;
  UInt16    err;

  do {
    EvtGetEvent(&event, evtWaitForever);

    if (!SysHandleEvent(&event))
      if (!MenuHandleEvent(0, &event, &err))
        if (!ApplicationHandleEvent(&event))
          FrmDispatchEvent(&event);

  } while (event.eType != appStopEvent);
}

/**
 * The Palm Computing Platform termination routine.
 */
void
EndApplication()
{
  // ensure all forms are closed
  FrmCloseAllForms();
}
