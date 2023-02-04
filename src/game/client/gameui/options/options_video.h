#ifndef CVIDEOSUBOPTIONS_H
#define CVIDEOSUBOPTIONS_H
#include <vgui_controls/PropertyPage.h>

namespace vgui2
{
class Label;
class ComboBox;
}

class CCvarCheckButton;
class CCvarTextEntry;

class CVideoSubOptions : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CVideoSubOptions, vgui2::PropertyPage);

public:
	CVideoSubOptions(vgui2::Panel *parent);

	virtual void OnResetData();
	virtual void OnApplyChanges();

private:
	vgui2::Label *m_pTimeLabel = nullptr;
};

#endif
