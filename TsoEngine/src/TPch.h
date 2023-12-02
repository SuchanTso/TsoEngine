//
//  TPch.hpp
//  TsoEngine
//
//  Created by user on 2023/4/10.
//
#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "glm/glm.hpp"

#include "Tso/Core/Keycode.h"
#include "Tso/Core/Log.h"
#include "Tso/Core/TimeStep.h"
#include "Tso/Core/Core.h"
#include "Tso/Core/Input.h"
#include "Tso/Event/Event.h"


#include "Tso/Renderer/Buffer.h"
#include "Tso/Renderer/VertexArray.h"
#include "Tso/Renderer/Shader.h"
#include "Tso/Renderer/Texture.h"

#include "Tso/Renderer/Renderer.h"
#include "Tso/Renderer/RenderCommand.h"


#ifdef TSO_PLATFORM_WINDOWS
  //  #include <Windows.h>
#endif
