#ifdef GL_RSURF_H_RECURSE_GUARD
#error Recursive header files inclusion detected in glquake.h
#else //GL_RSURF_H_RECURSE_GUARD

#define GL_RSURF_H_RECURSE_GUARD

#ifndef GL_RSURF_H_GUARD
#define GL_RSURF_H_GUARD
#pragma once

#ifdef _WIN32
void R_Hook();
#endif

#endif //GL_RSURF_H_GUARD

#undef GL_RSURF_H_RECURSE_GUARD
#endif //GL_RSURF_H_RECURSE_GUARD