/*
 * DirectDraw driver interface
 * (DirectX 7 version)
 *
 * Copyright (C) 2001 Ove Kaaven
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __WINE_DDRAWI_H
#define __WINE_DDRAWI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ddraw.h>

#ifndef __DDK_DDRAWINT_H
typedef struct _DDVIDEOPORTCAPS *LPDDVIDEOPORTCAPS; /* should be in dvp.h */
typedef struct _DDKERNELCAPS *LPDDKERNELCAPS; /* should be in ddkernel.h */
typedef struct _VMEMHEAP *LPVMEMHEAP; /* should be in dmemmgr.h */
#endif

#define DDAPI WINAPI

/* the DirectDraw versions */
#define DD_VERSION		0x0200 /* compatibility version */
#define DD_RUNTIME_VERSION	0x0700 /* actual version */

/* the HAL version returned from QUERYESCSUPPORT - DCICOMMAND */
#define DD_HAL_VERSION	0x0100

/* more DCICOMMAND escapes */
#define DDCREATEDRIVEROBJECT	10
#define DDGET32BITDRIVERNAME	11
#define DDNEWCALLBACKFNS	12
#define DDVERSIONINFO		13

#define MAX_DRIVER_NAME		CCHDEVICENAME

/*****************************************************************************
 * Initialization stuff
 */
typedef struct {
    char szName[260];
    char szEntryPoint[64];
    DWORD dwContext;
} DD32BITDRIVERDATA,*LPDD32BITDRIVERDATA;

typedef struct {
    DWORD dwHALVersion;
    ULONG_PTR dwReserved1;
    ULONG_PTR dwReserved2;
} DDVERSIONDATA,*LPDDVERSIONDATA;

typedef DWORD (PASCAL *LPDD32BITDRIVERINIT)(DWORD dwContext);

#ifndef __DDK_DDRAWINT_H
/* pointer to video memory */
typedef ULONG_PTR FLATPTR;
#endif

/* predeclare some structures */
typedef struct _DDHALINFO *LPDDHALINFO;
typedef struct _DDRAWI_DIRECTDRAW_INT *LPDDRAWI_DIRECTDRAW_INT;
typedef struct _DDRAWI_DIRECTDRAW_LCL *LPDDRAWI_DIRECTDRAW_LCL;
typedef struct _DDRAWI_DIRECTDRAW_GBL *LPDDRAWI_DIRECTDRAW_GBL;
typedef struct _DDRAWI_DDRAWSURFACE_INT *LPDDRAWI_DDRAWSURFACE_INT;
typedef struct _DDRAWI_DDRAWSURFACE_LCL *LPDDRAWI_DDRAWSURFACE_LCL;
typedef struct _DDRAWI_DDRAWSURFACE_GBL *LPDDRAWI_DDRAWSURFACE_GBL;
typedef struct _DDRAWI_DDRAWSURFACE_MORE *LPDDRAWI_DDRAWSURFACE_MORE;
typedef struct _DDRAWI_DDRAWSURFACE_GBL_MORE *LPDDRAWI_DDRAWSURFACE_GBL_MORE;
typedef struct _DDRAWI_DDRAWPALETTE_INT *LPDDRAWI_DDRAWPALETTE_INT;
typedef struct _DDRAWI_DDRAWPALETTE_LCL *LPDDRAWI_DDRAWPALETTE_LCL;
typedef struct _DDRAWI_DDRAWPALETTE_GBL *LPDDRAWI_DDRAWPALETTE_GBL;
typedef struct _DDRAWI_DDRAWCLIPPER_INT *LPDDRAWI_DDRAWCLIPPER_INT;
typedef struct _DDRAWI_DDRAWCLIPPER_LCL *LPDDRAWI_DDRAWCLIPPER_LCL;
typedef struct _DDRAWI_DDRAWCLIPPER_GBL *LPDDRAWI_DDRAWCLIPPER_GBL;
typedef struct _DDRAWI_DDVIDEOPORT_INT *LPDDRAWI_DDVIDEOPORT_INT;
typedef struct _DDRAWI_DDVIDEOPORT_LCL *LPDDRAWI_DDVIDEOPORT_LCL;
typedef struct _DDRAWI_DDMOTIONCOMP_INT *LPDDRAWI_DDMOTIONCOMP_INT;
typedef struct _DDRAWI_DDMOTIONCOMP_LCL *LPDDRAWI_DDMOTIONCOMP_LCL;

/*****************************************************************************
 * driver->ddraw callbacks
 */
typedef BOOL    (DDAPI *LPDDHAL_SETINFO)(LPDDHALINFO lpDDHalInfo, BOOL reset);
typedef FLATPTR (DDAPI *LPDDHAL_VIDMEMALLOC)(LPDDRAWI_DIRECTDRAW_GBL lpDD, int heap, DWORD dwWidth, DWORD dwHeight);
typedef void    (DDAPI *LPDDHAL_VIDMEMFREE)(LPDDRAWI_DIRECTDRAW_GBL lpDD, int heap, FLATPTR fpMem);

typedef struct {
    DWORD		dwSize;
    LPDDHAL_SETINFO	lpSetInfo;
    LPDDHAL_VIDMEMALLOC	lpVidMemAlloc;
    LPDDHAL_VIDMEMFREE	lpVidMemFree;
} DDHALDDRAWFNS,*LPDDHALDDRAWFNS;

/*****************************************************************************
 * mode info structure
 */
typedef struct _DDHALMODEINFO {
    DWORD	dwWidth;
    DWORD	dwHeight;
    LONG	lPitch;
    DWORD	dwBPP;
    WORD	wFlags;
    WORD	wRefreshRate;
    DWORD	dwRBitMask;
    DWORD	dwGBitMask;
    DWORD	dwBBitMask;
    DWORD	dwAlphaBitMask;
} DDHALMODEINFO,*LPDDHALMODEINFO;

#define DDMODEINFO_PALETTIZED	0x0001
#define DDMODEINFO_MODEX	0x0002
#define DDMODEINFO_UNSUPPORTED	0x0004
#define DDMODEINFO_STANDARDVGA	0x0008
#define DDMODEINFO_MAXREFRESH	0x0010
#define DDMODEINFO_STEREO	0x0020

/*****************************************************************************
 * video memory info structure
 */
typedef struct _VIDMEM {
    DWORD	dwFlags;
    FLATPTR	fpStart;
    union {
	FLATPTR		fpEnd;
	DWORD		dwWidth;
    } DUMMYUNIONNAME1;
    DDSCAPS	ddsCaps;
    DDSCAPS	ddsCapsAlt;
    union {
	LPVMEMHEAP	lpHeap;
	DWORD		dwHeight;
    } DUMMYUNIONNAME2;
} VIDMEM,*LPVIDMEM;

#ifndef __DDK_DDRAWINT_H
#define VIDMEM_ISLINEAR		0x00000001
#define VIDMEM_ISRECTANGULAR	0x00000002
#define VIDMEM_ISHEAP		0x00000004
#define VIDMEM_ISNONLOCAL	0x00000008
#define VIDMEM_ISWC		0x00000010
#define VIDMEM_ISDISABLED	0x00000020
#endif

typedef struct _VIDMEMINFO {
    FLATPTR		fpPrimary;
    DWORD		dwFlags;
    DWORD		dwDisplayWidth;
    DWORD		dwDisplayHeight;
    LONG		lDisplayPitch;
    DDPIXELFORMAT	ddpfDisplay;
    DWORD		dwOffscreenAlign;
    DWORD		dwOverlayAlign;
    DWORD		dwTextureAlign;
    DWORD		dwZBufferAlign;
    DWORD		dwAlphaAlign;
    DWORD		dwNumHeaps;
    LPVIDMEM		pvmList;
} VIDMEMINFO,*LPVIDMEMINFO;

typedef struct _HEAPALIAS {
    FLATPTR	fpVidMem;
    LPVOID	lpAlias;
    DWORD	dwAliasSize;
} HEAPALIAS,*LPHEAPALIAS;

typedef struct _HEAPALIASINFO {
    DWORD	dwRefCnt;
    DWORD	dwFlags;
    DWORD	dwNumHeaps;
    LPHEAPALIAS	lpAliases;
} HEAPALIASINFO,*LPHEAPALIASINFO;

#define HEAPALIASINFO_MAPPEDREAL	0x00000001
#define HEAPALIASINFO_MAPPEDDUMMY	0x00000002

/*****************************************************************************
 * capabilities structures
 */
typedef struct _DDCORECAPS {
    DWORD	dwSize;
    DWORD	dwCaps;
    DWORD	dwCaps2;
    DWORD	dwCKeyCaps;
    DWORD	dwFXCaps;
    DWORD	dwFXAlphaCaps;
    DWORD	dwPalCaps;
    DWORD	dwSVCaps;
    DWORD	dwAlphaBltConstBitDepths;
    DWORD	dwAlphaBltPixelBitDepths;
    DWORD	dwAlphaBltSurfaceBitDepths;
    DWORD	dwAlphaOverlayConstBitDepths;
    DWORD	dwAlphaOverlayPixelBitDepths;
    DWORD	dwAlphaOverlaySurfaceBitDepths;
    DWORD	dwZBufferBitDepths;
    DWORD	dwVidMemTotal;
    DWORD	dwVidMemFree;
    DWORD	dwMaxVisibleOverlays;
    DWORD	dwCurrVisibleOverlays;
    DWORD	dwNumFourCCCodes;
    DWORD	dwAlignBoundarySrc;
    DWORD	dwAlignSizeSrc;
    DWORD	dwAlignBoundaryDest;
    DWORD	dwAlignSizeDest;
    DWORD	dwAlignStrideAlign;
    DWORD	dwRops[DD_ROP_SPACE];
    DDSCAPS	ddsCaps;
    DWORD	dwMinOverlayStretch;
    DWORD	dwMaxOverlayStretch;
    DWORD	dwMinLiveVideoStretch;
    DWORD	dwMaxLiveVideoStretch;
    DWORD	dwMinHwCodecStretch;
    DWORD	dwMaxHwCodecStretch;
    DWORD	dwReserved1;
    DWORD	dwReserved2;
    DWORD	dwReserved3;
    DWORD	dwSVBCaps;
    DWORD	dwSVBCKeyCaps;
    DWORD	dwSVBFXCaps;
    DWORD	dwSVBRops[DD_ROP_SPACE];
    DWORD	dwVSBCaps;
    DWORD	dwVSBCKeyCaps;
    DWORD	dwVSBFXCaps;
    DWORD	dwVSBRops[DD_ROP_SPACE];
    DWORD	dwSSBCaps;
    DWORD	dwSSBCKeyCaps;
    DWORD	dwSSBFXCaps;
    DWORD	dwSSBRops[DD_ROP_SPACE];
    DWORD	dwMaxVideoPorts;
    DWORD	dwCurrVideoPorts;
    DWORD	dwSVBCaps2;
} DDCORECAPS,*LPDDCORECAPS;

typedef struct _DDNONLOCALVIDMEMCAPS {
    DWORD	dwSize;
    DWORD	dwNLVBCaps;
    DWORD	dwNLVBCaps2;
    DWORD	dwNLVBCKeyCaps;
    DWORD	dwNLVBFXCaps;
    DWORD	dwNLVBRops[DD_ROP_SPACE];
} DDNONLOCALVIDMEMCAPS,*LPDDNONLOCALVIDMEMCAPS;

#define DDSCAPS_EXECUTEBUFFER	DDSCAPS_RESERVED2
#define DDSCAPS2_VERTEXBUFFER	DDSCAPS2_RESERVED1
#define DDSCAPS2_COMMANDBUFFER	DDSCAPS2_RESERVED2

/*****************************************************************************
 * ddraw->driver callbacks
 */
#define DDHAL_DRIVER_NOTHANDLED	0
#define DDHAL_DRIVER_HANDLED	1
#define DDHAL_DRIVER_NOCKEYHW	2

typedef struct _DDHAL_DESTROYDRIVERDATA		*LPDDHAL_DESTROYDRIVERDATA;
typedef struct _DDHAL_CREATESURFACEDATA		*LPDDHAL_CREATESURFACEDATA;
typedef struct _DDHAL_DRVSETCOLORKEYDATA	*LPDDHAL_DRVSETCOLORKEYDATA;
typedef struct _DDHAL_SETMODEDATA		*LPDDHAL_SETMODEDATA;
typedef struct _DDHAL_WAITFORVERTICALBLANKDATA	*LPDDHAL_WAITFORVERTICALBLANKDATA;
typedef struct _DDHAL_CANCREATESURFACEDATA	*LPDDHAL_CANCREATESURFACEDATA;
typedef struct _DDHAL_CREATEPALETTEDATA		*LPDDHAL_CREATEPALETTEDATA;
typedef struct _DDHAL_GETSCANLINEDATA		*LPDDHAL_GETSCANLINEDATA;
typedef struct _DDHAL_SETEXCLUSIVEMODEDATA	*LPDDHAL_SETEXCLUSIVEMODEDATA;
typedef struct _DDHAL_FLIPTOGDISURFACEDATA	*LPDDHAL_FLIPTOGDISURFACEDATA;

typedef DWORD (PASCAL *LPDDHAL_DESTROYDRIVER)	    (LPDDHAL_DESTROYDRIVERDATA);
typedef DWORD (PASCAL *LPDDHAL_CREATESURFACE)	    (LPDDHAL_CREATESURFACEDATA);
typedef DWORD (PASCAL *LPDDHAL_SETCOLORKEY)	    (LPDDHAL_DRVSETCOLORKEYDATA);
typedef DWORD (PASCAL *LPDDHAL_SETMODE)		    (LPDDHAL_SETMODEDATA);
typedef DWORD (PASCAL *LPDDHAL_WAITFORVERTICALBLANK)(LPDDHAL_WAITFORVERTICALBLANKDATA);
typedef DWORD (PASCAL *LPDDHAL_CANCREATESURFACE)    (LPDDHAL_CANCREATESURFACEDATA );
typedef DWORD (PASCAL *LPDDHAL_CREATEPALETTE)	    (LPDDHAL_CREATEPALETTEDATA);
typedef DWORD (PASCAL *LPDDHAL_GETSCANLINE)	    (LPDDHAL_GETSCANLINEDATA);
typedef DWORD (PASCAL *LPDDHAL_SETEXCLUSIVEMODE)    (LPDDHAL_SETEXCLUSIVEMODEDATA);
typedef DWORD (PASCAL *LPDDHAL_FLIPTOGDISURFACE)    (LPDDHAL_FLIPTOGDISURFACEDATA);

typedef struct _DDHAL_DDCALLBACKS {
    DWORD				dwSize;
    DWORD				dwFlags;
    LPDDHAL_DESTROYDRIVER		DestroyDriver;
    LPDDHAL_CREATESURFACE		CreateSurface;
    LPDDHAL_SETCOLORKEY			SetColorKey;
    LPDDHAL_SETMODE			SetMode;
    LPDDHAL_WAITFORVERTICALBLANK	WaitForVerticalBlank;
    LPDDHAL_CANCREATESURFACE		CanCreateSurface;
    LPDDHAL_CREATEPALETTE		CreatePalette;
    LPDDHAL_GETSCANLINE			GetScanLine;
    /* DirectX 2 */
    LPDDHAL_SETEXCLUSIVEMODE		SetExclusiveMode;
    LPDDHAL_FLIPTOGDISURFACE		FlipToGDISurface;
} DDHAL_DDCALLBACKS,*LPDDHAL_DDCALLBACKS;

typedef struct _DDHAL_DESTROYSURFACEDATA	*LPDDHAL_DESTROYSURFACEDATA;
typedef struct _DDHAL_FLIPDATA			*LPDDHAL_FLIPDATA;
typedef struct _DDHAL_SETCLIPLISTDATA		*LPDDHAL_SETCLIPLISTDATA;
typedef struct _DDHAL_LOCKDATA			*LPDDHAL_LOCKDATA;
typedef struct _DDHAL_UNLOCKDATA		*LPDDHAL_UNLOCKDATA;
typedef struct _DDHAL_BLTDATA			*LPDDHAL_BLTDATA;
typedef struct _DDHAL_SETCOLORKEYDATA		*LPDDHAL_SETCOLORKEYDATA;
typedef struct _DDHAL_ADDATTACHEDSURFACEDATA	*LPDDHAL_ADDATTACHEDSURFACEDATA;
typedef struct _DDHAL_GETBLTSTATUSDATA		*LPDDHAL_GETBLTSTATUSDATA;
typedef struct _DDHAL_GETFLIPSTATUSDATA		*LPDDHAL_GETFLIPSTATUSDATA;
typedef struct _DDHAL_UPDATEOVERLAYDATA		*LPDDHAL_UPDATEOVERLAYDATA;
typedef struct _DDHAL_SETOVERLAYPOSITIONDATA	*LPDDHAL_SETOVERLAYPOSITIONDATA;
typedef struct _DDHAL_SETPALETTEDATA		*LPDDHAL_SETPALETTEDATA;

typedef DWORD (PASCAL *LPDDHALSURFCB_DESTROYSURFACE)	(LPDDHAL_DESTROYSURFACEDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_FLIP)		(LPDDHAL_FLIPDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_SETCLIPLIST)	(LPDDHAL_SETCLIPLISTDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_LOCK)		(LPDDHAL_LOCKDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_UNLOCK)		(LPDDHAL_UNLOCKDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_BLT)		(LPDDHAL_BLTDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_SETCOLORKEY)	(LPDDHAL_SETCOLORKEYDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_ADDATTACHEDSURFACE)(LPDDHAL_ADDATTACHEDSURFACEDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_GETBLTSTATUS)	(LPDDHAL_GETBLTSTATUSDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_GETFLIPSTATUS)	(LPDDHAL_GETFLIPSTATUSDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_UPDATEOVERLAY)	(LPDDHAL_UPDATEOVERLAYDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_SETOVERLAYPOSITION)(LPDDHAL_SETOVERLAYPOSITIONDATA);
typedef DWORD (PASCAL *LPDDHALSURFCB_SETPALETTE)	(LPDDHAL_SETPALETTEDATA);

typedef struct _DDHAL_DDSURFACECALLBACKS {
    DWORD				dwSize;
    DWORD				dwFlags;
    LPDDHALSURFCB_DESTROYSURFACE	DestroySurface;
    LPDDHALSURFCB_FLIP			Flip;
    LPDDHALSURFCB_SETCLIPLIST		SetClipList;
    LPDDHALSURFCB_LOCK			Lock;
    LPDDHALSURFCB_UNLOCK		Unlock;
    LPDDHALSURFCB_BLT			Blt;
    LPDDHALSURFCB_SETCOLORKEY		SetColorKey;
    LPDDHALSURFCB_ADDATTACHEDSURFACE	AddAttachedSurface;
    LPDDHALSURFCB_GETBLTSTATUS		GetBltStatus;
    LPDDHALSURFCB_GETFLIPSTATUS		GetFlipStatus;
    LPDDHALSURFCB_UPDATEOVERLAY		UpdateOverlay;
    LPDDHALSURFCB_SETOVERLAYPOSITION	SetOverlayPosition;
    LPVOID				reserved4;
    LPDDHALSURFCB_SETPALETTE		SetPalette;
} DDHAL_DDSURFACECALLBACKS,*LPDDHAL_DDSURFACECALLBACKS;

typedef struct _DDHAL_DESTROYPALETTEDATA	*LPDDHAL_DESTROYPALETTEDATA;
typedef struct _DDHAL_SETENTRIESDATA		*LPDDHAL_SETENTRIESDATA;

typedef DWORD (PASCAL *LPDDHALPALCB_DESTROYPALETTE)(LPDDHAL_DESTROYPALETTEDATA);
typedef DWORD (PASCAL *LPDDHALPALCB_SETENTRIES)    (LPDDHAL_SETENTRIESDATA);

typedef struct _DDHAL_DDPALETTECALLBACKS {
    DWORD				dwSize;
    DWORD				dwFlags;
    LPDDHALPALCB_DESTROYPALETTE		DestroyPalette;
    LPDDHALPALCB_SETENTRIES		SetEntries;
} DDHAL_DDPALETTECALLBACKS,*LPDDHAL_DDPALETTECALLBACKS;

typedef DWORD (PASCAL *LPDDHALEXEBUFCB_CANCREATEEXEBUF)(LPDDHAL_CANCREATESURFACEDATA);
typedef DWORD (PASCAL *LPDDHALEXEBUFCB_CREATEEXEBUF)   (LPDDHAL_CREATESURFACEDATA);
typedef DWORD (PASCAL *LPDDHALEXEBUFCB_DESTROYEXEBUF)  (LPDDHAL_DESTROYSURFACEDATA);
typedef DWORD (PASCAL *LPDDHALEXEBUFCB_LOCKEXEBUF)     (LPDDHAL_LOCKDATA);
typedef DWORD (PASCAL *LPDDHALEXEBUFCB_UNLOCKEXEBUF)   (LPDDHAL_UNLOCKDATA);

typedef struct _DDHAL_DDEXEBUFCALLBACKS {
    DWORD				dwSize;
    DWORD				dwFlags;
    LPDDHALEXEBUFCB_CANCREATEEXEBUF	CanCreateExecuteBuffer;
    LPDDHALEXEBUFCB_CREATEEXEBUF	CreateExecuteBuffer;
    LPDDHALEXEBUFCB_DESTROYEXEBUF	DestroyExecuteBuffer;
    LPDDHALEXEBUFCB_LOCKEXEBUF		LockExecuteBuffer;
    LPDDHALEXEBUFCB_UNLOCKEXEBUF	UnlockExecuteBuffer;
} DDHAL_DDEXEBUFCALLBACKS,*LPDDHAL_DDEXEBUFCALLBACKS;

typedef struct _DDHAL_GETAVAILDRIVERMEMORYDATA	*LPDDHAL_GETAVAILDRIVERMEMORYDATA;
typedef struct _DDHAL_UPDATENONLOCALHEAPDATA	*LPDDHAL_UPDATENONLOCALHEAPDATA;
typedef struct _DDHAL_GETHEAPALIGNMENTDATA	*LPDDHAL_GETHEAPALIGNMENTDATA;

typedef DWORD (PASCAL *LPDDHAL_GETAVAILDRIVERMEMORY)(LPDDHAL_GETAVAILDRIVERMEMORYDATA);
typedef DWORD (PASCAL *LPDDHAL_UPDATENONLOCALHEAP)  (LPDDHAL_UPDATENONLOCALHEAPDATA);
typedef DWORD (PASCAL *LPDDHAL_GETHEAPALIGNMENT)    (LPDDHAL_GETHEAPALIGNMENTDATA);

typedef struct _DDHAL_DDMISCELLANEOUSCALLBACKS {
    DWORD				dwSize;
    DWORD				dwFlags;
    LPDDHAL_GETAVAILDRIVERMEMORY	GetAvailDriverMemory;
    LPDDHAL_UPDATENONLOCALHEAP		UpdateNonLocalHeap;
    LPDDHAL_GETHEAPALIGNMENT		GetHeapAlignment;
    LPDDHALSURFCB_GETBLTSTATUS		GetSysmemBltStatus;
} DDHAL_DDMISCELLANEOUSCALLBACKS,*LPDDHAL_DDMISCELLANEOUSCALLBACKS;

typedef struct _DDHAL_CREATESURFACEEXDATA	*LPDDHAL_CREATESURFACEEXDATA;
typedef struct _DDHAL_GETDRIVERSTATEDATA	*LPDDHAL_GETDRIVERSTATEDATA;
typedef struct _DDHAL_DESTROYDDLOCALDATA	*LPDDHAL_DESTROYDDLOCALDATA;

typedef DWORD (PASCAL *LPDDHAL_CREATESURFACEEX)(LPDDHAL_CREATESURFACEEXDATA);
typedef DWORD (PASCAL *LPDDHAL_GETDRIVERSTATE) (LPDDHAL_GETDRIVERSTATEDATA);
typedef DWORD (PASCAL *LPDDHAL_DESTROYDDLOCAL) (LPDDHAL_DESTROYDDLOCALDATA);

typedef struct _DDHAL_DDMISCELLANEOUS2CALLBACKS {
    DWORD				dwSize;
    DWORD				dwFlags;
    LPVOID				Reserved;
    LPDDHAL_CREATESURFACEEX		CreateSurfaceEx;
    LPDDHAL_GETDRIVERSTATE		GetDriverState;
    LPDDHAL_DESTROYDDLOCAL		DestroyDDLocal;
} DDHAL_DDMISCELLANEOUS2CALLBACKS,*LPDDHAL_DDMISCELLANEOUS2CALLBACKS;

typedef HRESULT (WINAPI *LPDDGAMMACALIBRATORPROC)(LPDDGAMMARAMP, LPBYTE);

/*****************************************************************************
 * driver info structure
 *
 * The HAL is queried for additional callbacks via the GetDriverInfo callback.
 */
typedef struct _DDHAL_GETDRIVERINFODATA *LPDDHAL_GETDRIVERINFODATA;
typedef DWORD (PASCAL *LPDDHAL_GETDRIVERINFO)(LPDDHAL_GETDRIVERINFODATA);

typedef struct _DDHALINFO {
    DWORD			dwSize;
    LPDDHAL_DDCALLBACKS		lpDDCallbacks;
    LPDDHAL_DDSURFACECALLBACKS	lpDDSurfaceCallbacks;
    LPDDHAL_DDPALETTECALLBACKS	lpDDPaletteCallbacks;
    VIDMEMINFO			vmiData;
    DDCORECAPS			ddCaps;
    DWORD			dwMonitorFrequency;
    LPDDHAL_GETDRIVERINFO	GetDriverInfo;
    DWORD			dwModeIndex;
    LPDWORD			lpdwFourCC;
    DWORD			dwNumModes;
    LPDDHALMODEINFO		lpModeInfo;
    DWORD			dwFlags;
    LPVOID			lpPDevice;
    DWORD			hInstance;
    /* DirectX 2 */
    ULONG_PTR			lpD3DGlobalDriverData;
    ULONG_PTR			lpD3DHALCallbacks;
    LPDDHAL_DDEXEBUFCALLBACKS	lpDDExeBufCallbacks;
} DDHALINFO;

#define DDHALINFO_ISPRIMARYDISPLAY	0x00000001
#define DDHALINFO_MODEXILLEGAL		0x00000002
#define DDHALINFO_GETDRIVERINFOSET	0x00000004

/* where the high-level ddraw implementation stores the callbacks */
typedef struct _DDHAL_CALLBACKS {
    DDHAL_DDCALLBACKS		cbDDCallbacks;
    DDHAL_DDSURFACECALLBACKS	cbDDSurfaceCallbacks;
    DDHAL_DDPALETTECALLBACKS	cbDDPaletteCallbacks;
    DDHAL_DDCALLBACKS		HALDD;
    DDHAL_DDSURFACECALLBACKS	HALDDSurface;
    DDHAL_DDPALETTECALLBACKS	HALDDPalette;
    DDHAL_DDCALLBACKS		HELDD;
    DDHAL_DDSURFACECALLBACKS	HELDDSurface;
    DDHAL_DDPALETTECALLBACKS	HELDDPalette;
    DDHAL_DDEXEBUFCALLBACKS	cbDDExeBufCallbacks;
    DDHAL_DDEXEBUFCALLBACKS	HALDDExeBuf;
    DDHAL_DDEXEBUFCALLBACKS	HELDDExeBuf;
    /* there's more... videoport, colorcontrol, misc, and motion compensation callbacks... */
} DDHAL_CALLBACKS,*LPDDHAL_CALLBACKS;

/*****************************************************************************
 * parameter structures
 */
typedef struct _DDHAL_DESTROYDRIVERDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    HRESULT			ddRVal;
    LPDDHAL_DESTROYDRIVER	DestroyDriver;
} DDHAL_DESTROYDRIVERDATA;

typedef struct _DDHAL_SETMODEDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    DWORD			dwModeIndex;
    HRESULT			ddRVal;
    LPDDHAL_SETMODE		SetMode;
    BOOL			inexcl;
    BOOL			useRefreshRate;
} DDHAL_SETMODEDATA;

typedef struct _DDHAL_CREATESURFACEDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDSURFACEDESC		lpDDSurfaceDesc;
    LPDDRAWI_DDRAWSURFACE_LCL *	lplpSList;
    DWORD			dwSCnt;
    HRESULT			ddRVal;
    LPDDHAL_CREATESURFACE	CreateSurface;
} DDHAL_CREATESURFACEDATA;

typedef struct _DDHAL_CANCREATESURFACEDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDSURFACEDESC		lpDDSurfaceDesc;
    DWORD			bIsDifferentPixelFormat;
    HRESULT			ddRVal;
    LPDDHAL_CANCREATESURFACE	CanCreateSurface;
} DDHAL_CANCREATESURFACEDATA;

typedef struct _DDHAL_CREATEPALETTEDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDRAWI_DDRAWPALETTE_GBL	lpDDPalette;
    LPPALETTEENTRY		lpColorTable;
    HRESULT			ddRVal;
    LPDDHAL_CREATEPALETTE	CreatePalette;
    BOOL			is_excl;
} DDHAL_CREATEPALETTEDATA;

typedef struct _DDHAL_SETEXCLUSIVEMODEDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    DWORD			dwEnterExcl;
    DWORD			dwReserved;
    HRESULT			ddRVal;
    LPDDHAL_SETEXCLUSIVEMODE	SetExclusiveMode;
} DDHAL_SETEXCLUSIVEMODEDATA;

/* surfaces */
typedef struct _DDHAL_DESTROYSURFACEDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDRAWI_DDRAWSURFACE_LCL	lpDDSurface;
    HRESULT			ddRVal;
    LPDDHALSURFCB_DESTROYSURFACE DestroySurface;
} DDHAL_DESTROYSURFACEDATA;

typedef struct _DDHAL_FLIPDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDRAWI_DDRAWSURFACE_LCL	lpSurfCurr;
    LPDDRAWI_DDRAWSURFACE_LCL	lpSurfTarg;
    DWORD			dwFlags;
    HRESULT			ddRVal;
    LPDDHALSURFCB_FLIP		Flip;
    LPDDRAWI_DDRAWSURFACE_LCL	lpSurfCurrLeft;
    LPDDRAWI_DDRAWSURFACE_LCL	lpSurfTargLeft;
} DDHAL_FLIPDATA;

typedef struct _DDHAL_LOCKDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDRAWI_DDRAWSURFACE_LCL	lpDDSurface;
    DWORD			bHasRect;
    RECTL			rArea;
    LPVOID			lpSurfData;
    HRESULT			ddRVal;
    LPDDHALSURFCB_LOCK		Lock;
    DWORD			dwFlags;
} DDHAL_LOCKDATA;

typedef struct _DDHAL_UNLOCKDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDRAWI_DDRAWSURFACE_LCL	lpDDSurface;
    HRESULT			ddRVal;
    LPDDHALSURFCB_UNLOCK	Unlock;
} DDHAL_UNLOCKDATA;

typedef struct _DDHAL_BLTDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDRAWI_DDRAWSURFACE_LCL	lpDDDestSurface;
    RECTL			rDest;
    LPDDRAWI_DDRAWSURFACE_LCL	lpDDSrcSurface;
    RECTL			rSrc;
    DWORD			dwFlags;
    DWORD			dwROPFlags;
    DDBLTFX			bltFX;
    HRESULT			ddRVal;
    LPDDHALSURFCB_BLT		Blt;
    BOOL			IsClipped;
    RECTL			rOrigDest;
    RECTL			rOrigSrc;
    DWORD			dwRectCnt;
    LPRECT			prDestRects;
} DDHAL_BLTDATA;

typedef struct _DDHAL_SETPALETTEDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDRAWI_DDRAWSURFACE_LCL	lpDDSurface;
    LPDDRAWI_DDRAWPALETTE_GBL	lpDDPalette;
    HRESULT			ddRVal;
    LPDDHALSURFCB_SETPALETTE	SetPalette;
    BOOL			Attach;
} DDHAL_SETPALETTEDATA;

/* palettes */
typedef struct _DDHAL_DESTROYPALETTEDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDRAWI_DDRAWPALETTE_GBL	lpDDPalette;
    HRESULT			ddRVal;
    LPDDHALPALCB_DESTROYPALETTE	DestroyPalette;
} DDHAL_DESTROYPALETTEDATA;

typedef struct _DDHAL_SETENTRIESDATA {
    LPDDRAWI_DIRECTDRAW_GBL	lpDD;
    LPDDRAWI_DDRAWPALETTE_GBL	lpDDPalette;
    DWORD			dwBase;
    DWORD			dwNumEntries;
    LPPALETTEENTRY		lpEntries;
    HRESULT			ddRVal;
    LPDDHALPALCB_SETENTRIES	SetEntries;
} DDHAL_SETENTRIESDATA;

typedef struct _DDHAL_GETDRIVERINFODATA {
    DWORD	dwSize;
    DWORD	dwFlags;
    GUID	guidInfo;
    DWORD	dwExpectedSize;
    LPVOID	lpvData;
    DWORD	dwActualSize;
    HRESULT	ddRVal;
    ULONG_PTR	dwContext;
} DDHAL_GETDRIVERINFODATA;

/*****************************************************************************
 * high-level ddraw implementation structures
 */
typedef DWORD IUnknown; /* FIXME: implement proper definition */
typedef struct _IUNKNOWN_LIST {
    struct _IUNKNOWN_LIST *	lpLink;
    LPGUID			lpGuid;
    IUnknown *			lpIUnknown;
} IUNKNOWN_LIST,*LPIUNKNOWN_LIST;

typedef struct _PROCESS_LIST {
    struct _PROCESS_LIST *	lpLink;
    DWORD			dwProcessId;
    DWORD			dwRefCnt;
    DWORD			dwAlphaDepth;
    DWORD			dwZDepth;
} PROCESS_LIST,*LPPROCESS_LIST;

typedef struct _ATTACHLIST {
    DWORD			dwFlags;
    struct _ATTACHLIST *	lpLink;
    LPDDRAWI_DDRAWSURFACE_LCL	lpAttached;
    LPDDRAWI_DDRAWSURFACE_INT	lpIAttached;
} ATTACHLIST,*LPATTACHLIST;

#define DDAL_IMPLICIT	0x00000001

typedef struct _ACCESSRECTLIST {
    struct _ACCESSRECTLIST *	lpLink;
    RECT			rDest;
    LPDDRAWI_DIRECTDRAW_LCL	lpOwner;
    LPVOID			lpSurfaceData;
    DWORD			dwFlags;
    LPHEAPALIASINFO		lpHeapAliasInfo;
} ACCESSRECTLIST,*LPACCESSRECTLIST;

#define ACCESSRECT_VRAMSTYLE		0x00000001
#define ACCESSRECT_NOTHOLDINGWIN16LOCK	0x00000002
#define ACCESSRECT_BROKEN		0x00000004

typedef struct _DBLNODE {
    struct _DBLNODE *		next;
    struct _DBLNODE *		prev;
    LPDDRAWI_DDRAWSURFACE_LCL	object;
    LPDDRAWI_DDRAWSURFACE_INT	object_int;
} DBLNODE,*LPDBLNODE;

typedef struct _DDRAWI_DIRECTDRAW_INT {
    LPVOID			lpVtbl;
    LPDDRAWI_DIRECTDRAW_LCL	lpLcl;
    LPDDRAWI_DIRECTDRAW_INT	lpLink;
    DWORD			dwIntRefCnt;
} DDRAWI_DIRECTDRAW_INT;

typedef struct _DDRAWI_DIRECTDRAW_LCL {
    DWORD			lpDDMore;
    LPDDRAWI_DIRECTDRAW_GBL	lpGbl;
    DWORD			dwUnused0;
    DWORD			dwLocalFlags;
    DWORD			dwLocalRefCnt;
    DWORD			dwProcessId;
    IUnknown *			pUnkOuter;
    DWORD			dwObsolete1;
    ULONG_PTR			hWnd;
    ULONG_PTR			hDC;
    DWORD			dwErrorMode;
    LPDDRAWI_DDRAWSURFACE_INT	lpPrimary;
    LPDDRAWI_DDRAWSURFACE_INT	lpCB;
    DWORD			dwPreferredMode;
    /* DirectX 2 */
    HINSTANCE			hD3DInstance;
    IUnknown *			pD3DIUnknown;
    LPDDHAL_CALLBACKS		lpDDCB;
    ULONG_PTR			hDDVxd;
    /* DirectX 5.0 */
    DWORD			dwAppHackFlags;
    /* DirectX 5.0A */
    ULONG_PTR			hFocusWnd;
    DWORD			dwHotTracking;
    DWORD			dwIMEState;
    /* DirectX 6.0 */
    ULONG_PTR			hWndPopup;
    ULONG_PTR			hDD;
    ULONG_PTR			hGammaCalibrator;
    LPDDGAMMACALIBRATORPROC	lpGammaCalibrator;
} DDRAWI_DIRECTDRAW_LCL;

#define DDRAWILCL_HASEXCLUSIVEMODE	0x00000001
#define DDRAWILCL_ISFULLSCREEN		0x00000002
#define DDRAWILCL_SETCOOPCALLED		0x00000004
#define DDRAWILCL_ACTIVEYES		0x00000008
#define DDRAWILCL_ACTIVENO		0x00000010
#define DDRAWILCL_HOOKEDHWND		0x00000020
#define DDRAWILCL_ALLOWMODEX		0x00000040
#define DDRAWILCL_V1SCLBEHAVIOUR	0x00000080
#define DDRAWILCL_MODEHASBEENCHANGED	0x00000100
#define DDRAWILCL_CREATEDWINDOW		0x00000200
#define DDRAWILCL_DIRTYDC		0x00000400
#define DDRAWILCL_DISABLEINACTIVATE	0x00000800
#define DDRAWILCL_CURSORCLIPPED		0x00001000
#define DDRAWILCL_EXPLICITMONITOR	0x00002000
#define DDRAWILCL_MULTITHREADED		0x00004000
#define DDRAWILCL_FPUSETUP		0x00008000
#define DDRAWILCL_POWEREDDOWN		0x00010000
#define DDRAWILCL_DIRECTDRAW7		0x00020000
#define DDRAWILCL_ATTEMPTEDD3DCONTEXT	0x00040000
#define DDRAWILCL_FPUPRESERVE		0x00080000

typedef struct _DDRAWI_DIRECTDRAW_GBL {
    DWORD			dwRefCnt;
    DWORD			dwFlags;
    FLATPTR			fpPrimaryOrig;
    DDCORECAPS			ddCaps;
    DWORD			dwInternal1;
    DWORD			dwUnused1[9];
    LPDDHAL_CALLBACKS		lpDDCBtmp;
    LPDDRAWI_DDRAWSURFACE_INT	dsList;
    LPDDRAWI_DDRAWPALETTE_INT	palList;
    LPDDRAWI_DDRAWCLIPPER_INT	clipperList;
    LPDDRAWI_DIRECTDRAW_GBL	lp16DD;
    DWORD			dwMaxOverlays;
    DWORD			dwCurrOverlays;
    DWORD			dwMonitorFrequency;
    DDCORECAPS			ddHELCaps;
    DWORD			dwUnused2[50];
    DDCOLORKEY			ddckCKDestOverlay;
    DDCOLORKEY			ddckCKSrcOverlay;
    VIDMEMINFO			vmiData;
    LPVOID			lpDriverHandle;
    LPDDRAWI_DIRECTDRAW_LCL	lpExclusiveOwner;
    DWORD			dwModeIndex;
    DWORD			dwModeIndexOrig;
    DWORD			dwNumFourCC;
    LPDWORD			lpdwFourCC;
    DWORD			dwNumModes;
    LPDDHALMODEINFO		lpModeInfo;
    PROCESS_LIST		plProcessList;
    DWORD			dwSurfaceLockCount;
    DWORD			dwAliasedLockCnt;
    ULONG_PTR			dwReserved3;
    ULONG_PTR			hDD;
    char			cObsolete[12];
    DWORD			dwReserved1;
    DWORD			dwReserved2;
    DBLNODE			dbnOverlayRoot;
    volatile LPWORD		lpwPDeviceFlags;
    DWORD			dwPDevice;
    DWORD			dwWin16LockCnt;
    DWORD			dwUnused3;
    DWORD			hInstance;
    DWORD			dwEvent16;
    DWORD			dwSaveNumModes;
    /* DirectX 2 */
    ULONG_PTR			lpD3DGlobalDriverData;
    ULONG_PTR			lpD3DHALCallbacks;
    DDCORECAPS			ddBothCaps;
    /* DirectX 5.0 */
    LPDDVIDEOPORTCAPS		lpDDVideoPortCaps;
    LPDDRAWI_DDVIDEOPORT_INT	dvpList;
    ULONG_PTR			lpD3DHALCallbacks2;
    RECT			rectDevice;
    DWORD			cMonitors;
    LPVOID			gpbmiSrc;
    LPVOID			gpbmiDest;
    LPHEAPALIASINFO		phaiHeapAliases;
    ULONG_PTR			hKernelHandle;
    ULONG_PTR			pfnNotifyProc;
    LPDDKERNELCAPS		lpDDKernelCaps;
    LPDDNONLOCALVIDMEMCAPS	lpddNLVCaps;
    LPDDNONLOCALVIDMEMCAPS	lpddNLVHELCaps;
    LPDDNONLOCALVIDMEMCAPS	lpddNLVBothCaps;
    ULONG_PTR			lpD3DExtendedCaps;
    /* DirectX 5.0A */
    DWORD			dwDOSBoxEvent;
    RECT			rectDesktop;
    char			cDriverName[MAX_DRIVER_NAME];
    /* DirectX 6.0 */
    ULONG_PTR			lpD3DHALCallbacks3;
    DWORD			dwNumZPixelFormats;
    LPDDPIXELFORMAT		lpZPixelFormats;
    LPDDRAWI_DDMOTIONCOMP_INT	mcList;
    DWORD			hDDVxd;
    DDSCAPSEX			ddsCapsMore;
} DDRAWI_DIRECTDRAW_GBL;

#define DDRAWI_VIRTUALDESKTOP	0x00000008
#define DDRAWI_MODEX		0x00000010
#define DDRAWI_DISPLAYDRV	0x00000020
#define DDRAWI_FULLSCREEN	0x00000040
#define DDRAWI_MODECHANGED	0x00000080
#define DDRAWI_NOHARDWARE	0x00000100
#define DDRAWI_PALETTEINIT	0x00000200
#define DDRAWI_NOEMULATION	0x00000400
/* more... */

/* surfaces */
typedef struct _DDRAWI_DDRAWSURFACE_INT {
    LPVOID			lpVtbl;
    LPDDRAWI_DDRAWSURFACE_LCL	lpLcl;
    LPDDRAWI_DDRAWSURFACE_INT	lpLink;
    DWORD			dwIntRefCnt;
} DDRAWI_DDRAWSURFACE_INT;

typedef struct _DDRAWI_DDRAWSURFACE_GBL {
    DWORD			dwRefCnt;
    DWORD			dwGlobalFlags;
    union {
	LPACCESSRECTLIST	lpRectList;
	DWORD			dwBlockSizeY;
    } DUMMYUNIONNAME1;
    union {
	LPVMEMHEAP		lpVidMemHeap;
	DWORD			dwBlockSizeX;
    } DUMMYUNIONNAME2;
    union {
	LPDDRAWI_DIRECTDRAW_GBL	lpDD;
	LPVOID			lpDDHandle;
    } DUMMYUNIONNAME3;
    FLATPTR			fpVidMem;
    union {
	LONG			lPitch;
	DWORD			dwLinearSize;
    } DUMMYUNIONNAME4;
    WORD			wHeight;
    WORD			wWidth;
    DWORD			dwUsageCount;
    ULONG_PTR			dwReserved1; /* for display driver use */
    /* optional (defaults to primary surface pixelformat) */
    DDPIXELFORMAT		ddpfSurface;
} DDRAWI_DDRAWSURFACE_GBL;

#define DDRAWISURFGBL_MEMFREE			0x00000001
#define DDRAWISURFGBL_SYSMEMREQUESTED		0x00000002
#define DDRAWISURFGBL_ISGDISURFACE		0x00000004
#define DDRAWISURFGBL_SOFTWAREAUTOFLIP		0x00000008
#define DDRAWISURFGBL_LOCKNOTHOLDINGWIN16LOCK	0x00000010
#define DDRAWISURFGBL_LOCKVRAMSTYLE		0x00000020
#define DDRAWISURFGBL_LOCKBROKEN		0x00000040
#define DDRAWISURFGBL_IMPLICITHANDLE		0x00000080
#define DDRAWISURFGBL_ISCLIENTMEM		0x00000100
#define DDRAWISURFGBL_HARDWAREOPSOURCE		0x00000200
#define DDRAWISURFGBL_HARDWAREOPDEST		0x00000400
#define DDRAWISURFGBL_HARDWAREOPSTARTED		0x00000600
#define DDRAWISURFGBL_VPORTINTERLEAVED		0x00000800
#define DDRAWISURFGBL_VPORTDATA			0x00001000
#define DDRAWISURFGBL_LATEALLOCATELINEAR	0x00002000
#define DDRAWISURFGBL_SYSMEMEXECUTEBUFFER	0x00004000
#define DDRAWISURFGBL_FASTLOCKHELD		0x00008000
#define DDRAWISURFGBL_READONLYLOCKHELD		0x00010000

typedef struct _DDRAWI_DDRAWSURFACE_GBL_MORE {
    DWORD			dwSize;
    union {
	DWORD			dwPhysicalPageTable;
	FLATPTR			fpPhysicalVidMem;
    } DUMMYUNIONNAME1;
    LPDWORD			pPageTable;
    DWORD			cPages;
    ULONG_PTR			dwSavedDCContext;
    FLATPTR			fpAliasedVidMem;
    ULONG_PTR			dwDriverReserved;
    ULONG_PTR			dwHELReserved;
    DWORD			cPageUnlocks;
    ULONG_PTR			hKernelSurface;
    DWORD			dwKernelRefCnt;
    LPDDCOLORCONTROL		lpColorInfo;
    FLATPTR			fpNTAlias;
    DWORD			dwContentsStamp;
    LPVOID			lpvUnswappedDriverReserved;
    LPVOID			lpDDRAWReserved2;
    DWORD			dwDDRAWReserved1;
    DWORD			dwDDRAWReserved2;
    FLATPTR			fpAliasOfVidMem;
} DDRAWI_DDRAWSURFACE_GBL_MORE;

/* the MS version of this macro was somewhat obfuscated and unreadable
 * (possibly because of mediocre MS coders)... so I simplified it...
 * (and so I commit no copyright violations either, hah) */
#define GET_LPDDRAWSURFACE_GBL_MORE(psurf_gbl) \
    (*(((LPDDRAWI_DDRAWSURFACE_GBL_MORE *)(psurf_gbl)) - 1))

typedef struct _DDRAWI_DDRAWSURFACE_MORE {
    DWORD			dwSize;
    IUNKNOWN_LIST *		lpIUnknowns;
    LPDDRAWI_DIRECTDRAW_LCL	lpDD_lcl;
    DWORD			dwPageLockCount;
    DWORD			dwBytesAllocated;
    LPDDRAWI_DIRECTDRAW_INT	lpDD_int;
    DWORD			dwMipMapCount;
    LPDDRAWI_DDRAWCLIPPER_INT	lpDDIClipper;
    /* DirectX 5.0 */
    LPHEAPALIASINFO		lpHeapAliasInfo;
    DWORD			dwOverlayFlags;
    VOID			*rgjunc;
    LPDDRAWI_DDVIDEOPORT_LCL	lpVideoPort;
    LPDDOVERLAYFX		lpddOverlayFX;
    DDSCAPSEX			ddsCapsEx;
    DWORD			dwTextureStage;
    LPVOID			lpDDRAWReserved;
    LPVOID			lpDDRAWReserved2;
    LPVOID			lpDDrawReserved3;
    DWORD			dwDDrawReserved4;
    LPVOID			lpDDrawReserved5;
    LPDWORD			lpGammaRamp;
    LPDWORD			lpOriginalGammaRamp;
    LPVOID			lpDDrawReserved6;
    DWORD			dwSurfaceHandle;
    DWORD			qwDDrawReserved8[2];
    LPVOID			lpDDrawReserved9;
    DWORD			cSurfaces;
    LPDDSURFACEDESC2		pCreatedDDSurfaceDesc2;
    LPDDRAWI_DDRAWSURFACE_LCL	*slist;
    DWORD			dwFVF;
    LPVOID			lpVB;
} DDRAWI_DDRAWSURFACE_MORE;

typedef struct _DDRAWI_DDRAWSURFACE_LCL {
    LPDDRAWI_DDRAWSURFACE_MORE	lpSurfMore;
    LPDDRAWI_DDRAWSURFACE_GBL	lpGbl;
    ULONG_PTR			hDDSurface;
    LPATTACHLIST		lpAttachList;
    LPATTACHLIST		lpAttachListFrom;
    DWORD			dwLocalRefCnt;
    DWORD			dwProcessId;
    DWORD			dwFlags;
    DDSCAPS			ddsCaps;
    LPDDRAWI_DDRAWPALETTE_INT	lpDDPalette;
    LPDDRAWI_DDRAWCLIPPER_LCL	lpDDClipper;
    DWORD			dwModeCreatedIn;
    DWORD			dwBackBufferCount;
    DDCOLORKEY			ddckCKDestBlt;
    DDCOLORKEY			ddckCKSrcBlt;
    ULONG_PTR			hDC;
    ULONG_PTR			dwReserved1; /* for display driver use */
    /* overlays only */
    DDCOLORKEY			ddckCKSrcOverlay;
    DDCOLORKEY			ddckCKDestOverlay;
    LPDDRAWI_DDRAWSURFACE_INT	lpSurfaceOverlaying;
    DBLNODE			dbnOverlayNode;
    RECT			rcOverlaySrc;
    RECT			rcOverlayDest;
    DWORD			dwClrXparent;
    DWORD			dwAlpha;
    LONG			lOverlayX;
    LONG			lOverlayY;
} DDRAWI_DDRAWSURFACE_LCL;

#define DDRAWISURF_ATTACHED		0x00000001
#define DDRAWISURF_IMPLICITCREATE	0x00000002
#define DDRAWISURF_ISFREE		0x00000004
#define DDRAWISURF_ATTACHED_FROM	0x00000008
#define DDRAWISURF_IMPLICITROOT		0x00000010
#define DDRAWISURF_PARTOFPRIMARYCHAIN	0x00000020
#define DDRAWISURF_DATAISALIASED	0x00000040
#define DDRAWISURF_HASDC		0x00000080
#define DDRAWISURF_HASCKEYDESTOVERLAY	0x00000100
#define DDRAWISURF_HASCKEYDESTBLT	0x00000200
#define DDRAWISURF_HASCKEYSRCOVERLAY	0x00000400
#ifndef DDRAWISURF_HASCKEYSRCBLT
#define DDRAWISURF_HASCKEYSRCBLT	0x00000800
#endif
#define DDRAWISURF_LOCKEXCLUDEDCURSOR	0x00001000
#ifndef DDRAWISURF_HASPIXELFORMAT
#define DDRAWISURF_HASPIXELFORMAT	0x00002000
#endif
#ifndef DDRAWISURF_HASOVERLAYDATA
#define DDRAWISURF_HASOVERLAYDATA	0x00004000
#endif
#define DDRAWISURF_SETGAMMA		0x00008000
/* more... */
#ifndef DDRAWISURF_INVALID
#define DDRAWISURF_INVALID		0x10000000
#endif

/* palettes */
typedef struct _DDRAWI_DDRAWPALETTE_INT {
    LPVOID			lpVtbl;
    LPDDRAWI_DDRAWPALETTE_LCL	lpLcl;
    LPDDRAWI_DDRAWPALETTE_INT	lpLink;
    DWORD			dwIntRefCnt;
} DDRAWI_DDRAWPALETTE_INT;

typedef struct _DDRAWI_DDRAWPALETTE_GBL {
    DWORD			dwRefCnt;
    DWORD			dwFlags;
    LPDDRAWI_DIRECTDRAW_LCL	lpDD_lcl;
    DWORD			dwProcessId;
    LPPALETTEENTRY		lpColorTable;
    union {
	ULONG_PTR		dwReserved1; /* for display driver use */
	HPALETTE		hHELGDIPalette;
    } DUMMYUNIONNAME1;
    /* DirectX 5.0 */
    DWORD			dwDriverReserved;
    DWORD			dwContentsStamp;
    /* DirectX 6.0 */
    DWORD			dwSaveStamp;
    /* DirectX 7.0 */
    DWORD			dwHandle;
} DDRAWI_DDRAWPALETTE_GBL;

#define DDRAWIPAL_256		0x00000001
#define DDRAWIPAL_16		0x00000002
#define DDRAWIPAL_GDI		0x00000004
#define DDRAWIPAL_STORED_8	0x00000008
#define DDRAWIPAL_STORED_16	0x00000010
#define DDRAWIPAL_STORED_24	0x00000020
#define DDRAWIPAL_EXCLUSIVE	0x00000040
#define DDRAWIPAL_INHEL		0x00000080
#define DDRAWIPAL_DIRTY		0x00000100
#define DDRAWIPAL_ALLOW256	0x00000200
#define DDRAWIPAL_4		0x00000400
#define DDRAWIPAL_2		0x00000800
#define DDRAWIPAL_STORED_8INDEX	0x00001000
#define DDRAWIPAL_ALPHA		0x00002000

typedef struct _DDRAWI_DDRAWPALETTE_LCL {
    DWORD			lpPalMore;
    LPDDRAWI_DDRAWPALETTE_GBL	lpGbl;
    ULONG_PTR			dwUnused0;
    DWORD			dwLocalRefCnt;
    IUnknown *			pUnkOuter;
    LPDDRAWI_DIRECTDRAW_LCL	lpDD_lcl;
    ULONG_PTR			dwReserved1;
    /* DirectX 6.0 */
    ULONG_PTR			dwDDRAWReserved1;
    ULONG_PTR			dwDDRAWReserved2;
    ULONG_PTR			dwDDRAWReserved3;
} DDRAWI_DDRAWPALETTE_LCL;

typedef struct _DDHAL_GETMOCOMPGUIDSDATA FAR *LPDDHAL_GETMOCOMPGUIDSDATA;
typedef struct _DDHAL_GETMOCOMPFORMATSDATA FAR *LPDDHAL_GETMOCOMPFORMATSDATA;
typedef struct _DDHAL_CREATEMOCOMPDATA FAR *LPDDHAL_CREATEMOCOMPDATA;
typedef struct _DDHAL_GETMOCOMPCOMPBUFFDATA FAR *LPDDHAL_GETMOCOMPCOMPBUFFDATA;
typedef struct _DDHAL_GETINTERNALMOCOMPDATA FAR *LPDDHAL_GETINTERNALMOCOMPDATA;
typedef struct _DDHAL_BEGINMOCOMPFRAMEDATA FAR *LPDDHAL_BEGINMOCOMPFRAMEDATA;
typedef struct _DDHAL_ENDMOCOMPFRAMEDATA FAR *LPDDHAL_ENDMOCOMPFRAMEDATA;
typedef struct _DDHAL_RENDERMOCOMPDATA FAR *LPDDHAL_RENDERMOCOMPDATA;
typedef struct _DDHAL_QUERYMOCOMPSTATUSDATA FAR *LPDDHAL_QUERYMOCOMPSTATUSDATA;
typedef struct _DDHAL_DESTROYMOCOMPDATA FAR *LPDDHAL_DESTROYMOCOMPDATA;
typedef struct _DDHAL_COLORCONTROLDATA FAR *LPDDHAL_COLORCONTROLDATA;

typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_GETGUIDS)( LPDDHAL_GETMOCOMPGUIDSDATA);
typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_GETFORMATS)( LPDDHAL_GETMOCOMPFORMATSDATA);
typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_CREATE)( LPDDHAL_CREATEMOCOMPDATA);
typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_GETCOMPBUFFINFO)( LPDDHAL_GETMOCOMPCOMPBUFFDATA);
typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_GETINTERNALINFO)( LPDDHAL_GETINTERNALMOCOMPDATA);
typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_BEGINFRAME)( LPDDHAL_BEGINMOCOMPFRAMEDATA);
typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_ENDFRAME)( LPDDHAL_ENDMOCOMPFRAMEDATA);
typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_RENDER)( LPDDHAL_RENDERMOCOMPDATA);
typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_QUERYSTATUS)( LPDDHAL_QUERYMOCOMPSTATUSDATA);
typedef DWORD (FAR PASCAL *LPDDHALMOCOMPCB_DESTROY)( LPDDHAL_DESTROYMOCOMPDATA);
typedef DWORD (FAR PASCAL *LPDDHALCOLORCB_COLORCONTROL)(LPDDHAL_COLORCONTROLDATA);

typedef struct _DDMCCOMPBUFFERINFO
{
    DWORD                       dwSize;             // [in]   size of the struct
    DWORD                       dwNumCompBuffers;   // [out]  number of buffers required for compressed data
    DWORD                       dwWidthToCreate;    // [out]    Width of surface to create
    DWORD                       dwHeightToCreate;   // [out]    Height of surface to create
    DWORD                       dwBytesToAllocate;  // [out]    Total number of bytes used by each surface
    DDSCAPS2                    ddCompCaps;         // [out]    caps to create surfaces to store compressed data
    DDPIXELFORMAT               ddPixelFormat;      // [out]  format to create surfaces to store compressed data
} DDMCCOMPBUFFERINFO, *LPDDMCCOMPBUFFERINFO;

typedef struct _DDMCBUFFERINFO
{
    DWORD                       dwSize;         // [in]    size of the struct
    LPDDRAWI_DDRAWSURFACE_LCL   lpCompSurface;  // [in]    pointer to buffer containing compressed data
    DWORD                       dwDataOffset;   // [in]    offset of relevant data from the beginning of buffer
    DWORD                       dwDataSize;     // [in]    size of relevant data
    LPVOID                      lpPrivate;      // Reserved for DirectDraw;
} DDMCBUFFERINFO, *LPDDMCBUFFERINFO;

typedef struct _DDHAL_GETDRIVERSTATEDATA {
    DWORD                       dwFlags;
    union
    {
        ULONG_PTR               dwhContext;
    };
    LPDWORD                     lpdwStates;
    DWORD                       dwLength;
    HRESULT                     ddRVal;
} DDHAL_GETDRIVERSTATEDATA;

typedef struct _DDHAL_ADDATTACHEDSURFACEDATA
{
    LPDDRAWI_DIRECTDRAW_GBL         lpDD;
    LPDDRAWI_DDRAWSURFACE_LCL       lpDDSurface;
    LPDDRAWI_DDRAWSURFACE_LCL       lpSurfAttached;
    HRESULT                         ddRVal;
    LPDDHALSURFCB_ADDATTACHEDSURFACE    AddAttachedSurface;
} DDHAL_ADDATTACHEDSURFACEDATA;

typedef struct _DDHAL_BEGINMOCOMPFRAMEDATA
{
    LPDDRAWI_DIRECTDRAW_LCL     lpDD;
    LPDDRAWI_DDMOTIONCOMP_LCL   lpMoComp;
    LPDDRAWI_DDRAWSURFACE_LCL   lpDestSurface;        // [in]  destination buffer in which to decoding this frame
    DWORD                       dwInputDataSize;      // [in]  size of other misc input data to begin frame
    LPVOID                      lpInputData;          // [in]  pointer to misc input data
    DWORD                       dwOutputDataSize;     // [in]  size of other misc output data to begin frame
    LPVOID                      lpOutputData;         // [in]  pointer to output misc data (allocated by client)
    HRESULT                     ddRVal;               // [out]
    LPDDHALMOCOMPCB_BEGINFRAME  BeginMoCompFrame;
} DDHAL_BEGINMOCOMPFRAMEDATA;

typedef struct _DDHAL_COLORCONTROLDATA
{
    LPDDRAWI_DIRECTDRAW_GBL     lpDD;       // driver struct
    LPDDRAWI_DDRAWSURFACE_LCL   lpDDSurface;    // surface
    LPDDCOLORCONTROL            lpColorData;    // color control information
    DWORD                       dwFlags;    // DDRAWI_GETCOLOR/DDRAWI_SETCOLOR
    HRESULT                     ddRVal;     // return value
    LPDDHALCOLORCB_COLORCONTROL ColorControl;   // PRIVATE: ptr to callback
} DDHAL_COLORCONTROLDATA;

typedef struct _DDHAL_CREATEMOCOMPDATA
{
    LPDDRAWI_DIRECTDRAW_LCL     lpDD;
    LPDDRAWI_DDMOTIONCOMP_LCL   lpMoComp;
    LPGUID                      lpGuid;
    DWORD                       dwUncompWidth;
    DWORD                       dwUncompHeight;
    DDPIXELFORMAT               ddUncompPixelFormat;
    LPVOID                      lpData;
    DWORD                       dwDataSize;
    HRESULT                     ddRVal;
    LPDDHALMOCOMPCB_CREATE      CreateMoComp;
} DDHAL_CREATEMOCOMPDATA;

typedef struct _DDHAL_DESTROYMOCOMPDATA
{
    LPDDRAWI_DIRECTDRAW_LCL     lpDD;
    LPDDRAWI_DDMOTIONCOMP_LCL   lpMoComp;
    HRESULT                     ddRVal;
    LPDDHALMOCOMPCB_DESTROY     DestroyMoComp;
} DDHAL_DESTROYMOCOMPDATA;

typedef struct _DDHAL_ENDMOCOMPFRAMEDATA
{
    LPDDRAWI_DIRECTDRAW_LCL     lpDD;
    LPDDRAWI_DDMOTIONCOMP_LCL   lpMoComp;
    LPVOID                      lpInputData;
    DWORD                       dwInputDataSize;
    HRESULT                     ddRVal;
    LPDDHALMOCOMPCB_ENDFRAME    EndMoCompFrame;
} DDHAL_ENDMOCOMPFRAMEDATA;

typedef struct _DDHAL_FLIPTOGDISURFACEDATA
{
    LPDDRAWI_DIRECTDRAW_GBL     lpDD;         // driver struct
    DWORD                       dwToGDI;          // TRUE if flipping to the GDI surface, FALSE if flipping away
    DWORD                       dwReserved;       // reserved for future use
    HRESULT            ddRVal;       // return value
    LPDDHAL_FLIPTOGDISURFACE    FlipToGDISurface; // PRIVATE: ptr to callback
} DDHAL_FLIPTOGDISURFACEDATA;

typedef struct _DDHAL_GETAVAILDRIVERMEMORYDATA
{
    LPDDRAWI_DIRECTDRAW_GBL lpDD;        // driver struct
    DDSCAPS                 DDSCaps;     // caps for type of surface memory
    DWORD                   dwTotal;     // total memory for this kind of surface
    DWORD                   dwFree;      // free memory for this kind of surface
    HRESULT                 ddRVal;      // return value
    LPDDHAL_GETAVAILDRIVERMEMORY   GetAvailDriverMemory; // PRIVATE: ptr to callback
    DDSCAPSEX               ddsCapsEx;       // Added in V6. Driver should check DDVERSION info
                                                 // to see if this field is present.
} DDHAL_GETAVAILDRIVERMEMORYDATA;

typedef struct _DDHAL_GETBLTSTATUSDATA
{
    LPDDRAWI_DIRECTDRAW_GBL     lpDD;       // driver struct
    LPDDRAWI_DDRAWSURFACE_LCL   lpDDSurface;    // surface struct
    DWORD                       dwFlags;    // flags
    HRESULT                     ddRVal;     // return value
    LPDDHALSURFCB_GETBLTSTATUS  GetBltStatus;   // PRIVATE: ptr to callback
} DDHAL_GETBLTSTATUSDATA;

typedef struct _DDHAL_GETFLIPSTATUSDATA
{
    LPDDRAWI_DIRECTDRAW_GBL     lpDD;       // driver struct
    LPDDRAWI_DDRAWSURFACE_LCL   lpDDSurface;    // surface struct
    DWORD                       dwFlags;    // flags
    HRESULT                     ddRVal;     // return value
    LPDDHALSURFCB_GETFLIPSTATUS GetFlipStatus;  // PRIVATE: ptr to callback
} DDHAL_GETFLIPSTATUSDATA;

typedef struct _DDHAL_GETINTERNALMOCOMPDATA
{
    LPDDRAWI_DIRECTDRAW_LCL     lpDD;
    LPGUID                      lpGuid;
    DWORD                       dwWidth;            // [in]   width of uncompressed data
    DWORD                       dwHeight;           // [in]   height of uncompressed data
    DDPIXELFORMAT               ddPixelFormat;      // [in]   pixel-format of uncompressed data
    DWORD                       dwScratchMemAlloc;  // [out]  amount of scratch memory will the hal allocate for its private use
    HRESULT                     ddRVal;             // [out]
    LPDDHALMOCOMPCB_GETINTERNALINFO  GetInternalMoCompInfo;
} DDHAL_GETINTERNALMOCOMPDATA;

typedef struct _DDHAL_GETMOCOMPCOMPBUFFDATA
{
    LPDDRAWI_DIRECTDRAW_LCL     lpDD;
    LPGUID                      lpGuid;
    DWORD                       dwWidth;            // [in]   width of uncompressed data
    DWORD                       dwHeight;           // [in]   height of uncompressed data
    DDPIXELFORMAT               ddPixelFormat;      // [in]   pixel-format of uncompressed data
    DWORD                       dwNumTypesCompBuffs;// [in/out] number of memory types required for comp buffers
    LPDDMCCOMPBUFFERINFO        lpCompBuffInfo;     // [in]   driver supplied info regarding comp buffers (allocated by client)
    HRESULT                     ddRVal;             // [out]
    LPDDHALMOCOMPCB_GETCOMPBUFFINFO  GetMoCompBuffInfo;
} DDHAL_GETMOCOMPCOMPBUFFDATA;

typedef struct _DDHAL_GETMOCOMPGUIDSDATA
{
    LPDDRAWI_DIRECTDRAW_LCL lpDD;
    DWORD               dwNumGuids;
    LPGUID              lpGuids;
    HRESULT             ddRVal;
    LPDDHALMOCOMPCB_GETGUIDS GetMoCompGuids;
} DDHAL_GETMOCOMPGUIDSDATA;

typedef struct _DDHAL_GETMOCOMPFORMATSDATA
{
    LPDDRAWI_DIRECTDRAW_LCL lpDD;
    LPGUID              lpGuid;
    DWORD               dwNumFormats;
    LPDDPIXELFORMAT     lpFormats;
    HRESULT             ddRVal;
    LPDDHALMOCOMPCB_GETFORMATS   GetMoCompFormats;
} DDHAL_GETMOCOMPFORMATSDATA;

typedef struct _DDHAL_GETSCANLINEDATA
{
    LPDDRAWI_DIRECTDRAW_GBL     lpDD;       // driver struct
    DWORD                       dwScanLine; // returned scan line
    HRESULT                     ddRVal;     // return value
    LPDDHAL_GETSCANLINE         GetScanLine;    // PRIVATE: ptr to callback
} DDHAL_GETSCANLINEDATA;

typedef struct _DDHAL_QUERYMOCOMPSTATUSDATA
{
    LPDDRAWI_DIRECTDRAW_LCL     lpDD;
    LPDDRAWI_DDMOTIONCOMP_LCL   lpMoComp;
    LPDDRAWI_DDRAWSURFACE_LCL   lpSurface;      // [in]  Surface being queried
    DWORD                       dwFlags;        // [in]  DDMCQUERY_XXX falgs
    HRESULT                     ddRVal;         // [out]
    LPDDHALMOCOMPCB_QUERYSTATUS QueryMoCompStatus;
} DDHAL_QUERYMOCOMPSTATUSDATA;

typedef struct _DDHAL_RENDERMOCOMPDATA
{
    LPDDRAWI_DIRECTDRAW_LCL     lpDD;
    LPDDRAWI_DDMOTIONCOMP_LCL   lpMoComp;
    DWORD                       dwNumBuffers;   // [in]  Number of entries in the lpMacroBlockInfo array
    LPDDMCBUFFERINFO            lpBufferInfo;   // [in]  Surfaces containing macro block info
    DWORD                       dwFunction;     // [in]  Function
    LPVOID                      lpInputData;
    DWORD                       dwInputDataSize;
    LPVOID                      lpOutputData;
    DWORD                       dwOutputDataSize;
    HRESULT                     ddRVal;         // [out]
    LPDDHALMOCOMPCB_RENDER      RenderMoComp;
} DDHAL_RENDERMOCOMPDATA;

typedef struct _DDHAL_SETCOLORKEYDATA
{
    LPDDRAWI_DIRECTDRAW_GBL     lpDD;       // driver struct
    LPDDRAWI_DDRAWSURFACE_LCL   lpDDSurface;    // surface struct
    DWORD                       dwFlags;    // flags
    DDCOLORKEY                  ckNew;      // new color key
    HRESULT                     ddRVal;     // return value
    LPDDHALSURFCB_SETCOLORKEY   SetColorKey;    // PRIVATE: ptr to callback
} DDHAL_SETCOLORKEYDATA;

typedef struct _DDHAL_SETOVERLAYPOSITIONDATA
{
    LPDDRAWI_DIRECTDRAW_GBL     lpDD;       // driver struct
    LPDDRAWI_DDRAWSURFACE_LCL   lpDDSrcSurface; // src surface
    LPDDRAWI_DDRAWSURFACE_LCL   lpDDDestSurface;// dest surface
    LONG                        lXPos;      // x position
    LONG                        lYPos;      // y position
    HRESULT                     ddRVal;     // return value
    LPDDHALSURFCB_SETOVERLAYPOSITION SetOverlayPosition; // PRIVATE: ptr to callback
} DDHAL_SETOVERLAYPOSITIONDATA;

typedef struct _DDHAL_UPDATEOVERLAYDATA
{
    LPDDRAWI_DIRECTDRAW_GBL     lpDD;       // driver struct
    LPDDRAWI_DDRAWSURFACE_LCL   lpDDDestSurface;// dest surface
    RECTL                       rDest;      // dest rect
    LPDDRAWI_DDRAWSURFACE_LCL   lpDDSrcSurface; // src surface
    RECTL                       rSrc;       // src rect
    DWORD                       dwFlags;    // flags
    DDOVERLAYFX                 overlayFX;  // overlay FX
    HRESULT                     ddRVal;     // return value
    LPDDHALSURFCB_UPDATEOVERLAY UpdateOverlay;  // PRIVATE: ptr to callback
} DDHAL_UPDATEOVERLAYDATA;

typedef struct _DDHAL_WAITFORVERTICALBLANKDATA
{
    LPDDRAWI_DIRECTDRAW_GBL     lpDD;       // driver struct
    DWORD                       dwFlags;    // flags
    DWORD                       bIsInVB;    // is in vertical blank
    ULONG_PTR                   hEvent;     // event
    HRESULT                     ddRVal;     // return value
    LPDDHAL_WAITFORVERTICALBLANK    WaitForVerticalBlank; // PRIVATE: ptr to callback
} DDHAL_WAITFORVERTICALBLANKDATA;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __WINE_DDRAWI_H */
