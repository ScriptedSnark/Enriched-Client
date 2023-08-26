#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "origin.h"

#include <sstream>

ConVar hud_origin("hud_origin", "0", FCVAR_BHL_ARCHIVE, "Enable origin HUD");

DEFINE_HUD_ELEM(CHudOrigin);

void CHudOrigin::Init()
{
	m_iFlags = HUD_ACTIVE | HUD_DRAW_ALWAYS;
}

void CHudOrigin::VidInit()
{
}

void CHudOrigin::Draw(float time)
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) || gEngfuncs.IsSpectateOnly())
		return;

	if (!hud_origin.GetBool())
		return;

	int r, g, b, a = 255;

	gHUD.GetHudColor(HudPart::Common, 0, r, g, b);
	ScaleColors(r, g, b, a);

	std::ostringstream out;
	out.setf(std::ios::fixed);
	out.precision(0);

	out << "X: " << m_Origin.x << "\n"
	    << "Y: " << m_Origin.y << "\n"
	    << "Z: " << m_Origin.z;

	gHUD.DrawMultilineString(ScreenWidth / 1.32, gHUD.m_scrinfo.iCharHeight * 4, out.str(), r, g, b);
}

void CHudOrigin::UpdateOrigin(const float simorg[3])
{
	m_Origin.x = simorg[0];
	m_Origin.y = simorg[1];
	m_Origin.z = simorg[2];
}