/*
 * @(#)gccfix.c
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
 *
 * - source code obtained from John Marshall (john_w_marshall@palm.com)
 */

#include "palm.h"

void
_GccReleaseCode(UInt16 cmd UNUSED, void *pbp UNUSED, UInt16 flags)
{
  if (flags & sysAppLaunchFlagNewGlobals) {

    MemHandle codeH;
    int resno;

    for (resno = 2; (codeH = DmGet1Resource ('code', resno)) != NULL; resno++) {
      MemHandleUnlock (codeH);
      DmReleaseResource (codeH);
    }
  }
}
