#include "TPch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Tso {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();

}