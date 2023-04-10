#pragma once
//client api deifination

#include "Tso/Application.h"
#ifdef TSO_PLATFORM_WINDOWS
#include "Tso/Log.h"
//i didn't find a way to import spdlog in macos temporarily
#endif

#include "Tso/Event/Event.h"

//entry point --------------------------------
#include "Tso/EntryPoint.h"
//--------------------------------------------
