#include "TPch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Tso {
	Ref<VertexArray> VertexArray::Create() {
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: {
			TSO_CORE_ASSERT(false, "API:None is not support now !");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: {
            return CreateRef<OpenGLVertexArray>();
		}
		default:
			break;
		}
		TSO_CORE_ASSERT(false, "no API is chosen!");
		return nullptr;
	}
}
