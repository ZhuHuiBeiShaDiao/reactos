/*
 * COPYRIGHT:   See COPYING in the top level directory
 * PROJECT:     ReactOS user32.dll
 * FILE:        include/user32.h
 * PURPOSE:     Global user32 definitions
 */
#include <windows.h>
#include <win32k/win32k.h>

typedef struct _USER32_THREAD_DATA
{
  MSG LastMessage;
  HKL KeyboardLayoutHandle;
} USER32_THREAD_DATA, *PUSER32_THREAD_DATA;

PUSER32_THREAD_DATA User32GetThreadData();

VOID DeleteFrameBrushes(VOID);

