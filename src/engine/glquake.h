#ifdef GLQUAKE_H_RECURSE_GUARD
#error Recursive header files inclusion detected in glquake.h
#else //GLQUAKE_H_RECURSE_GUARD

#define GLQUAKE_H_RECURSE_GUARD

#ifndef GLQUAKE_H_GUARD
#define GLQUAKE_H_GUARD
#pragma once

#ifdef _WIN32

#include "hud.h"
#include "cl_util.h"

#include "reGS_enginehook.h"
#include "sptlib/Utils.hpp"
#include "minhook/MinHook.h"
#include "reGS_patterns.hpp"

extern Utils utils;

#include "FileSystem.h"

#endif

#endif //GLQUAKE_H_GUARD

#undef GLQUAKE_H_RECURSE_GUARD
#endif //GLQUAKE_H_RECURSE_GUARD