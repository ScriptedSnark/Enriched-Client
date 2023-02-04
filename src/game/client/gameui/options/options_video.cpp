#include <vgui_controls/Label.h>
#include <vgui_controls/ComboBox.h>
#include <KeyValues.h>
#include "client_vgui.h"
#include "options_video.h"
#include "hud.h"
#include "cvar_check_button.h"
#include "cvar_text_entry.h"

CVideoSubOptions::CVideoSubOptions(vgui2::Panel *parent)
    : BaseClass(parent, "VideoSubOptions")
{
	SetSize(100, 100); // Silence "parent not sized yet" warning

	LoadControlSettings(VGUI2_ROOT_DIR "resource/options/VideoSubOptions.res");
}

void CVideoSubOptions::OnResetData()
{

}

void CVideoSubOptions::OnApplyChanges()
{

}
