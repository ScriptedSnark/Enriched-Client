/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
//
// death notice
//
#include <string.h>
#include <stdio.h>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "death_notice.h"
#include "spectator.h"
#include "vgui/client_viewport.h"

struct DeathNoticeItem
{
	char szKiller[MAX_PLAYERNAME_LENGTH * 2];
	char szVictim[MAX_PLAYERNAME_LENGTH * 2];
	int iId; // the index number of the associated sprite
	int iSuicide;
	int iTeamKill;
	int iNonPlayerKill;
	float flDisplayTime;
	float *KillerColor;
	float *VictimColor;
};

#define MAX_DEATHNOTICES 4
static int DEATHNOTICE_DISPLAY_TIME = 6;

#define DEATHNOTICE_TOP 32

DeathNoticeItem rgDeathNoticeList[MAX_DEATHNOTICES + 1];

float g_ColorBlue[3] = { 0.6, 0.8, 1.0 };
float g_ColorRed[3] = { 1.0, 0.25, 0.25 };
float g_ColorGreen[3] = { 0.6, 1.0, 0.6 };
float g_ColorYellow[3] = { 1.0, 0.7, 0.0 };
float g_ColorGrey[3] = { 0.8, 0.8, 0.8 };

float *GetClientColor(int clientIndex)
{
	switch (GetPlayerInfo(clientIndex)->GetTeamNumber())
	{
	case 1:
		return g_ColorBlue;
	case 2:
		return g_ColorRed;
	case 3:
		return g_ColorYellow;
	case 4:
		return g_ColorGreen;
	case 0:
		return g_ColorYellow;

	default:
		return g_ColorGrey;
	}

	return NULL;
}

DEFINE_HUD_ELEM(CHudDeathNotice);

void CHudDeathNotice::Init(void)
{
	BaseHudClass::Init();

	HookMessage<&CHudDeathNotice::MsgFunc_DeathMsg>("DeathMsg");

	CVAR_CREATE("hud_deathnotice_time", "6", 0);
}

void CHudDeathNotice::InitHudData(void)
{
	memset(rgDeathNoticeList, 0, sizeof(rgDeathNoticeList));
}

void CHudDeathNotice::VidInit(void)
{
	m_HUD_d_skull = gHUD.GetSpriteIndex("d_skull");
}

void CHudDeathNotice::Draw(float flTime)
{
	int x, y, r, g, b;

	for (int i = 0; i < MAX_DEATHNOTICES; i++)
	{
		if (rgDeathNoticeList[i].iId == 0)
			break; // we've gone through them all

		if (rgDeathNoticeList[i].flDisplayTime < flTime)
		{ // display time has expired
			// remove the current item from the list
			memmove(&rgDeathNoticeList[i], &rgDeathNoticeList[i + 1], sizeof(DeathNoticeItem) * (MAX_DEATHNOTICES - i));
			i--; // continue on the next item;  stop the counter getting incremented
			continue;
		}

		rgDeathNoticeList[i].flDisplayTime = min(rgDeathNoticeList[i].flDisplayTime, gHUD.m_flTime + DEATHNOTICE_DISPLAY_TIME);

		// Only draw if the viewport will let me
		if (g_pViewport && g_pViewport->AllowedToPrintText())
		{
			// Draw the death notice
			y = DEATHNOTICE_TOP + 2 + (20 * i); //!!!

			int id = (rgDeathNoticeList[i].iId == -1) ? m_HUD_d_skull : rgDeathNoticeList[i].iId;
			x = ScreenWidth - ConsoleStringLen(rgDeathNoticeList[i].szVictim) - (gHUD.GetSpriteRect(id).right - gHUD.GetSpriteRect(id).left);

			if (!rgDeathNoticeList[i].iSuicide)
			{
				x -= (5 + ConsoleStringLen(rgDeathNoticeList[i].szKiller));

				// Draw killers name
				if (rgDeathNoticeList[i].KillerColor)
					gEngfuncs.pfnDrawSetTextColor(rgDeathNoticeList[i].KillerColor[0], rgDeathNoticeList[i].KillerColor[1], rgDeathNoticeList[i].KillerColor[2]);
				x = 5 + DrawConsoleString(x, y, rgDeathNoticeList[i].szKiller);
			}

			r = 255;
			g = 80;
			b = 0;
			if (rgDeathNoticeList[i].iTeamKill)
			{
				r = 10;
				g = 240;
				b = 10; // display it in sickly green
			}

			// Draw death weapon
			SPR_Set(gHUD.GetSprite(id), r, g, b);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(id));

			x += (gHUD.GetSpriteRect(id).right - gHUD.GetSpriteRect(id).left);

			// Draw victims name (if it was a player that was killed)
			if (rgDeathNoticeList[i].iNonPlayerKill == FALSE)
			{
				if (rgDeathNoticeList[i].VictimColor)
					gEngfuncs.pfnDrawSetTextColor(rgDeathNoticeList[i].VictimColor[0], rgDeathNoticeList[i].VictimColor[1], rgDeathNoticeList[i].VictimColor[2]);
				x = DrawConsoleString(x, y, rgDeathNoticeList[i].szVictim);
			}
		}
	}
}

// This message handler may be better off elsewhere
int CHudDeathNotice::MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf)
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ(pbuf, iSize);

	int killer = READ_BYTE();
	int victim = READ_BYTE();

	char killedwith[32];
	strcpy(killedwith, "d_");
	strncat(killedwith, READ_STRING(), 32);

	if (g_pViewport)
		g_pViewport->DeathMsg(killer, victim);

	CHudSpectator::Get()->DeathMessage(victim);
	int i;
	for (i = 0; i < MAX_DEATHNOTICES; i++)
	{
		if (rgDeathNoticeList[i].iId == 0)
			break;
	}
	if (i == MAX_DEATHNOTICES)
	{ // move the rest of the list forward to make room for this item
		memmove(rgDeathNoticeList, rgDeathNoticeList + 1, sizeof(DeathNoticeItem) * MAX_DEATHNOTICES);
		i = MAX_DEATHNOTICES - 1;
	}

	if (g_pViewport)
		g_pViewport->GetAllPlayersInfo();

	// Get the Killer's name
	CPlayerInfo *killerInfo = nullptr;
	const char *killer_name;
	if (killer != 0 && (killerInfo = GetPlayerInfo(killer)->Update())->IsConnected())
	{
		killer_name = killerInfo->GetName();
		rgDeathNoticeList[i].KillerColor = GetClientColor(killer);
		strncpy(rgDeathNoticeList[i].szKiller, killer_name, MAX_PLAYERNAME_LENGTH);
		rgDeathNoticeList[i].szKiller[MAX_PLAYERNAME_LENGTH - 1] = 0;
	}
	else
	{
		killer_name = "";
		rgDeathNoticeList[i].szKiller[0] = 0;
	}

	// Get the Victim's name
	const char *victim_name = NULL;
	// If victim is -1, the killer killed a specific, non-player object (like a sentrygun)
	if (((char)victim) != -1)
		victim_name = GetPlayerInfo(victim)->Update()->GetName();
	if (!victim_name)
	{
		victim_name = "";
		rgDeathNoticeList[i].szVictim[0] = 0;
	}
	else
	{
		rgDeathNoticeList[i].VictimColor = GetClientColor(victim);
		strncpy(rgDeathNoticeList[i].szVictim, victim_name, MAX_PLAYERNAME_LENGTH);
		rgDeathNoticeList[i].szVictim[MAX_PLAYERNAME_LENGTH - 1] = 0;
	}

	// Is it a non-player object kill?
	if (((char)victim) == -1)
	{
		rgDeathNoticeList[i].iNonPlayerKill = TRUE;

		// Store the object's name in the Victim slot (skip the d_ bit)
		strcpy(rgDeathNoticeList[i].szVictim, killedwith + 2);
	}
	else
	{
		if (killer == victim || killer == 0)
			rgDeathNoticeList[i].iSuicide = TRUE;

		if (!strcmp(killedwith, "d_teammate"))
			rgDeathNoticeList[i].iTeamKill = TRUE;
	}

	// Find the sprite in the list
	int spr = gHUD.GetSpriteIndex(killedwith);

	rgDeathNoticeList[i].iId = spr;

	DEATHNOTICE_DISPLAY_TIME = CVAR_GET_FLOAT("hud_deathnotice_time");
	rgDeathNoticeList[i].flDisplayTime = gHUD.m_flTime + DEATHNOTICE_DISPLAY_TIME;

	if (rgDeathNoticeList[i].iNonPlayerKill)
	{
		ConsolePrint(rgDeathNoticeList[i].szKiller);
		ConsolePrint(" killed a ");
		ConsolePrint(rgDeathNoticeList[i].szVictim);
		ConsolePrint("\n");
	}
	else
	{
		// record the death notice in the console
		if (rgDeathNoticeList[i].iSuicide)
		{
			ConsolePrint(rgDeathNoticeList[i].szVictim);

			if (!strcmp(killedwith, "d_world"))
			{
				ConsolePrint(" died");
			}
			else
			{
				ConsolePrint(" killed self");
			}
		}
		else if (rgDeathNoticeList[i].iTeamKill)
		{
			ConsolePrint(rgDeathNoticeList[i].szKiller);
			ConsolePrint(" killed his teammate ");
			ConsolePrint(rgDeathNoticeList[i].szVictim);
		}
		else
		{
			ConsolePrint(rgDeathNoticeList[i].szKiller);
			ConsolePrint(" killed ");
			ConsolePrint(rgDeathNoticeList[i].szVictim);
		}

		if (killedwith && *killedwith && (*killedwith > 13) && strcmp(killedwith, "d_world") && !rgDeathNoticeList[i].iTeamKill)
		{
			ConsolePrint(" with ");

			// replace the code names with the 'real' names
			if (!strcmp(killedwith + 2, "egon"))
				strcpy(killedwith, "d_gluon gun");
			if (!strcmp(killedwith + 2, "gauss"))
				strcpy(killedwith, "d_tau cannon");

			ConsolePrint(killedwith + 2); // skip over the "d_" part
		}

		ConsolePrint("\n");
	}

	return 1;
}