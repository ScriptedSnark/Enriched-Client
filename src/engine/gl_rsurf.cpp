#ifdef _WIN32

#include "hud.h"
#include "cl_util.h"
#include "glquake.h"
#include "com_model.h"

ConVar hw_disable_dynamic_lightmaps("hw_disable_dynamic_lightmaps", "0", FCVAR_BHL_ARCHIVE);

typedef void (*_R_RenderDynamicLightmaps)(msurface_t *fa);
_R_RenderDynamicLightmaps ORIG_R_RenderDynamicLightmaps = nullptr;

void R_RenderDynamicLightmaps(msurface_t *fa)
{
	if (hw_disable_dynamic_lightmaps.GetBool())
		return;

	ORIG_R_RenderDynamicLightmaps(fa);
}

void R_Hook()
{
	Hook(R_RenderDynamicLightmaps);
}

#endif