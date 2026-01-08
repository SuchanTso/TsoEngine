#include "TPch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Tso {


	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: {
			TSO_CORE_ASSERT(false , "API:None is not support now !");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: {
			return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}
		default:
			break;
		}
		TSO_CORE_ASSERT(false, "no API is chosen!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: {
			TSO_CORE_ASSERT(false, "API:None is not support now !");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: {
            return CreateRef<OpenGLVertexBuffer>(size);
		}
		default:
			break;
		}
		TSO_CORE_ASSERT(false, "no API is chosen!");
		return nullptr;
	}


	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: {
			TSO_CORE_ASSERT(false, "API:None is not support now !");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: {
            return CreateRef<OpenGLIndexBuffer>(indices, count);// OpenGLIndexBuffer(indices, count);
		}
		default:
			break;
		}
		TSO_CORE_ASSERT(false, "no API is chosen!");
		return nullptr;


	}


}
