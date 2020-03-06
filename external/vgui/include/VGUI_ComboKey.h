//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef VGUI1_COMBOKEY_H
#define VGUI1_COMBOKEY_H

#include<VGUI.h>

namespace vgui
{

enum KeyCode;

class ComboKey
{
public:
	ComboKey(KeyCode code,KeyCode modifier);
public:
	bool isTwoCombo(KeyCode code,KeyCode modifier);
protected:
	bool check(KeyCode code);
protected:
	KeyCode _keyCode[2];
friend class Panel;
};

}


#endif