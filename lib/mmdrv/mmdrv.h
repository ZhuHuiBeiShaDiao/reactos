/*
 *
 * COPYRIGHT:            See COPYING in the top level directory
 * PROJECT:              ReactOS kernel
 * FILE:                 lib/mmdrv/mmdrv.h
 * PURPOSE:              Multimedia User Mode Driver (header)
 * PROGRAMMER:           Andrew Greenwood
 * UPDATE HISTORY:
 *                       Jan 30, 2004: Imported into ReactOS tree
 */

#ifndef __INCLUDES_MMDRV_H__
#define __INCLUDES_MMDRV_H__

//#define UNICODE

#define EXPORT __declspec(dllexport)


#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>

// This needs to be done to get winioctl.h to work:
typedef unsigned __int64 DWORD64, *PDWORD64;

#include <winioctl.h>
//#include "mmddk.h"


#define SOUND_MAX_DEVICE_NAME 1024   // GUESSWORK
#define SOUND_MAX_DEVICES 256       // GUESSWORK


// If the root is \Device and the Device type is
// WaveIn and the device number is 2, the full name is \Device\WaveIn2

#define WAVE_IN_DEVICE_NAME     "\\Device\\WaveIn"
#define WAVE_IN_DEVICE_NAME_U  L"\\Device\\WaveIn"
#define WAVE_OUT_DEVICE_NAME    "\\Device\\WaveOut"
#define WAVE_OUT_DEVICE_NAME_U L"\\Device\\WaveOut"

#define MIDI_IN_DEVICE_NAME     "\\Device\\MidiIn"
#define MIDI_IN_DEVICE_NAME_U  L"\\Device\\MidiIn"
#define MIDI_OUT_DEVICE_NAME    "\\Device\\MidiOut"
#define MIDI_OUT_DEVICE_NAME_U L"\\Device\\MidiOut"

#define AUX_DEVICE_NAME     "\\Device\\MMAux"
#define AUX_DEVICE_NAME_U  L"\\Device\\MMAux"


#define IOCTL_SOUND_BASE    FILE_DEVICE_SOUND
#define IOCTL_WAVE_BASE     0x0000
#define IOCTL_MIDI_BASE     0x0080

// Wave device driver IOCTLs

#define IOCTL_WAVE_QUERY_FORMAT         CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0001, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_WAVE_SET_FORMAT           CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0002, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_GET_CAPABILITIES     CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0003, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_WAVE_SET_STATE            CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0004, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_GET_STATE            CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0005, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_GET_POSITION         CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0006, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_SET_VOLUME           CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0007, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_WAVE_GET_VOLUME           CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0008, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_WAVE_SET_PITCH            CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0009, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_GET_PITCH            CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x000A, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_SET_PLAYBACK_RATE    CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x000B, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_GET_PLAYBACK_RATE    CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x000C, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_PLAY                 CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x000D, METHOD_IN_DIRECT, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_RECORD               CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x000E, METHOD_OUT_DIRECT, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_BREAK_LOOP           CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x000F, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_WAVE_SET_LOW_PRIORITY     CTL_CODE(IOCTL_SOUND_BASE, IOCTL_WAVE_BASE + 0x0010, METHOD_BUFFERED, FILE_WRITE_ACCESS)

// MIDI device driver IOCTLs

#define IOCTL_MIDI_GET_CAPABILITIES   CTL_CODE(IOCTL_SOUND_BASE, IOCTL_MIDI_BASE + 0x0001, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_MIDI_SET_STATE          CTL_CODE(IOCTL_SOUND_BASE, IOCTL_MIDI_BASE + 0x0002, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_MIDI_GET_STATE          CTL_CODE(IOCTL_SOUND_BASE, IOCTL_MIDI_BASE + 0x0003, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_MIDI_SET_VOLUME         CTL_CODE(IOCTL_SOUND_BASE, IOCTL_MIDI_BASE + 0x0004, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_MIDI_GET_VOLUME         CTL_CODE(IOCTL_SOUND_BASE, IOCTL_MIDI_BASE + 0x0005, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_MIDI_PLAY               CTL_CODE(IOCTL_SOUND_BASE, IOCTL_MIDI_BASE + 0x0006, METHOD_NEITHER, FILE_WRITE_ACCESS)
#define IOCTL_MIDI_RECORD             CTL_CODE(IOCTL_SOUND_BASE, IOCTL_MIDI_BASE + 0x0007, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_MIDI_CACHE_PATCHES      CTL_CODE(IOCTL_SOUND_BASE, IOCTL_MIDI_BASE + 0x0008, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_MIDI_CACHE_DRUM_PATCHES CTL_CODE(IOCTL_SOUND_BASE, IOCTL_MIDI_BASE + 0x0009, METHOD_BUFFERED, FILE_WRITE_ACCESS)



CRITICAL_SECTION CS;  // Serialize access to device lists

HANDLE Heap;

 enum {
      InvalidDevice,
      WaveInDevice,
      WaveOutDevice,
      MidiInDevice,
      MidiOutDevice,
      AuxDevice
};


MMRESULT OpenDevice(UINT DeviceType, DWORD ID, PHANDLE pDeviceHandle,
                    DWORD Access);

MMRESULT FindDevices();

DWORD GetDeviceCount(UINT DeviceType);

DWORD TranslateStatus(void);


#endif
