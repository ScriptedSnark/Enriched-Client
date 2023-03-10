/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== h_export.cpp ========================================================

  Entity classes exported by Halflife.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

// Holds engine functionality callbacks
enginefuncs_t g_engfuncs;
globalvars_t *gpGlobals;

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "winsani_in.h"
#include "windows.h"
#include "winsani_out.h"

// Required DLL entry point
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
	}
	return TRUE;
}

#endif

#ifdef _WIN32
#define SERVER_DLLEXPORT __stdcall
#else
#define SERVER_DLLEXPORT __attribute__((visibility("default")))
#endif

extern "C" void SERVER_DLLEXPORT GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals)
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;

	char gd[MAX_PATH];
	GET_GAME_DIR(gd);
	g_iIsAg = strcmp(gd, "ag") == 0 ? 1 : 0;
}
