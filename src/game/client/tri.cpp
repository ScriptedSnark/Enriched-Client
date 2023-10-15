//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

// Triangle rendering, if any

#include "hud.h"
#include "cl_util.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "Exports.h"
#include "hud/spectator.h"

#include "particleman.h"
#include "tri.h"

CSysModule *g_hParticleManModule = NULL;
IParticleMan *g_pParticleMan = NULL;

// buz start

#define SURF_PLANEBACK      2
#define SURF_DRAWSKY        4
#define SURF_DRAWSPRITE     8
#define SURF_DRAWTURB       0x10
#define SURF_DRAWTILED      0x20
#define SURF_DRAWBACKGROUND 0x40
#define SURF_UNDERWATER     0x80
#define SURF_DONTWARP       0x100
#define BACKFACE_EPSILON    0.01

// 0-2 are axial planes
#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2

#include "r_studioint.h"
#include "com_model.h"
#include "studio.h"
#include "StudioModelRenderer.h"
#include "GameStudioModelRenderer.h"
#include "sdl_rt.h"

#define GL_TEXTURE0_ARB 0x84C0
#define GL_TEXTURE1_ARB 0x84C1
typedef void(APIENTRYP PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;

extern engine_studio_api_t IEngineStudio;
extern CGameStudioModelRenderer g_StudioRenderer;

void ClearBuffer(void);
extern bool g_bShadows;

mleaf_t *Mod_PointInLeaf(Vector p, model_t *model) // quake's func
{
	mnode_t *node = model->nodes;
	while (1)
	{
		if (node->contents < 0)
			return (mleaf_t *)node;
		mplane_t *plane = node->plane;
		float d = DotProduct(p, plane->normal) - plane->dist;
		if (d > 0)
			node = node->children[0];
		else
			node = node->children[1];
	}

	return NULL; // never reached
}

model_t *g_pworld;
int g_visframe;
int g_framecount;
Vector g_lightvec;

void RecursiveDrawWorld(mnode_t *node)
{
	if (node->contents == CONTENTS_SOLID)
		return;

	if (node->visframe != g_visframe)
		return;

	if (node->contents < 0)
		return; // faces already marked by engine

	// recurse down the children, Order doesn't matter
	RecursiveDrawWorld(node->children[0]);
	RecursiveDrawWorld(node->children[1]);

	// draw stuff
	int c = node->numsurfaces;
	if (c)
	{
		msurface_t *surf = g_pworld->surfaces + node->firstsurface;

		for (; c; c--, surf++)
		{
			if (surf->visframe != g_framecount)
				continue;

			if (surf->flags & (SURF_DRAWSKY | SURF_DRAWTURB | SURF_UNDERWATER))
				continue;

			// cull from light vector

			float dot;
			mplane_t *plane = surf->plane;

			switch (plane->type)
			{
			case PLANE_X:
				dot = g_lightvec[0];
				break;
			case PLANE_Y:
				dot = g_lightvec[1];
				break;
			case PLANE_Z:
				dot = g_lightvec[2];
				break;
			default:
				dot = DotProduct(g_lightvec, plane->normal);
				break;
			}

			if ((dot > 0) && (surf->flags & SURF_PLANEBACK))
				continue;

			if ((dot < 0) && !(surf->flags & SURF_PLANEBACK))
				continue;

			glpoly_t *p = surf->polys;
			float *v = p->verts[0];

			glBegin(GL_POLYGON);
			for (int i = 0; i < p->numverts; i++, v += VERTEXSIZE)
			{
				glTexCoord2f(v[3], v[4]);
				glVertex3fv(v);
			}
			glEnd();
		}
	}
}

// buz end

//---------------------------------------------------
// Particle Manager
//---------------------------------------------------
void CL_LoadParticleMan()
{
	char szPDir[512];

	if (gEngfuncs.COM_ExpandFilename(PARTICLEMAN_DLLNAME, szPDir, sizeof(szPDir)) == FALSE)
	{
		g_pParticleMan = NULL;
		g_hParticleManModule = NULL;
		return;
	}

	g_hParticleManModule = Sys_LoadModule(szPDir);
	CreateInterfaceFn particleManFactory = Sys_GetFactory(g_hParticleManModule);

	if (particleManFactory == NULL)
	{
		g_pParticleMan = NULL;
		g_hParticleManModule = NULL;
		return;
	}

	g_pParticleMan = (IParticleMan *)particleManFactory(PARTICLEMAN_INTERFACE, NULL);

	if (g_pParticleMan)
	{
		g_pParticleMan->SetUp(&gEngfuncs);

		// Add custom particle classes here BEFORE calling anything else or you will die.
		g_pParticleMan->AddCustomParticleClassSize(sizeof(CBaseParticle));
	}
}

void CL_UnloadParticleMan()
{
	Sys_UnloadModule(g_hParticleManModule);

	g_pParticleMan = NULL;
	g_hParticleManModule = NULL;
}

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void CL_DLLEXPORT HUD_DrawNormalTriangles(void)
{
	//	RecClDrawNormalTriangles();

	ClearBuffer();

	CHudSpectator::Get()->DrawOverview();

	// buz start
	if (g_bShadows && IEngineStudio.IsHardware())
	{
		if (!glActiveTextureARB)
			glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)GetSDL()->GL_GetProcAddress("glActiveTextureARB");

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		// buz: workaround half-life's bug, when multitexturing left enabled after
		// rendering brush entities
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);

		glDepthMask(GL_FALSE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glColor4f(0.5, 0.5, 0.5, 1);

		glStencilFunc(GL_NOTEQUAL, 0, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glEnable(GL_STENCIL_TEST);

		// get current visframe number
		g_pworld = gEngfuncs.GetEntityByIndex(0)->model;
		mleaf_t *leaf = Mod_PointInLeaf(g_StudioRenderer.m_vRenderOrigin, g_pworld);
		g_visframe = leaf->visframe;

		// get current frame number
		g_framecount = g_StudioRenderer.m_nFrameCount;

		// get light vector
		g_StudioRenderer.GetShadowVector(g_lightvec);

		// draw world
		RecursiveDrawWorld(g_pworld->nodes);

		glPopAttrib();
	}

	g_bShadows = false;
	// buz end
}

#if defined(_TFC)
void RunEventList(void);
#endif

/*
=================
HUD_DrawTransparentTriangles

Render any triangles with transparent rendermode needs here
=================
*/
void CL_DLLEXPORT HUD_DrawTransparentTriangles(void)
{
	//	RecClDrawTransparentTriangles();

#if defined(_TFC)
	RunEventList();
#endif

	if (g_pParticleMan)
		g_pParticleMan->Update();
}
