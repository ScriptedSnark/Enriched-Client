//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef VGUI1_TOGGLEBUTTON_H
#define VGUI1_TOGGLEBUTTON_H

#include<VGUI.h>
#include<VGUI_Button.h>

namespace vgui
{

class VGUIAPI ToggleButton : public Button
{
public:
	ToggleButton(const char* text,int x,int y,int wide,int tall);
	ToggleButton(const char* text,int x,int y);
};

}

#endif