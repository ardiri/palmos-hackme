/*
 * @(#)palm.h
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

#ifndef _PALM_H
#define _PALM_H

// system includes
#include <PalmOS.h>
#include <System/DLServer.h>

// application constants and structures
#define appCreator           'HkMe'
#define syncType             'sync'
#define __LOADER__           __attribute__ ((section ("loader")))   // code0002
#define __CODEBASE__         __attribute__ ((section ("codebase"))) // code0003
#define ftrVersion           1000
#define ftrTrapAddressSystem 2000
#define ftrHotSyncName       3000
#define ftrListMemory        4000

#define KEY_FILENAME         "hotsync_keyfile"

// local includes
#include "resource.h"
#include "hackme.h"

// functions
extern UInt32  PilotMain(UInt16, MemPtr, UInt16);
extern void    InitApplication(void);
extern Boolean ApplicationHandleEvent(EventType *);
extern void    ApplicationDisplayDialog(UInt16);
extern void    EventLoop(void);
extern void    EndApplication(void);

#endif
