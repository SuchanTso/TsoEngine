#include "TPch.h"
#include"FrameBuffer.h"
#include "Tso/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Tso {
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferInfo& info) {
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: {
			TSO_CORE_ASSERT(false, "API:None is not support now !");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: {
			return OpenGLFrameBuffer::Create(info);
		}
		default:
			break;
		}
		TSO_CORE_ASSERT(false, "no API is chosen!");
		return nullptr;
	}
}