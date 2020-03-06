//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef VGUI1_FOCUSCHANGESIGNAL_H
#define VGUI1_FOCUSCHANGESIGNAL_H

#include<VGUI.h>

namespace vgui
{

class Panel;

class VGUIAPI FocusChangeSignal
{
public:
	virtual void focusChanged(bool lost,Panel* panel)=0;
};

}

#endif