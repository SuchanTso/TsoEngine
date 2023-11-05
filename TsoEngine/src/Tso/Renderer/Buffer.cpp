#include "TPch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Tso {


	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: {
			TSO_CORE_ASSERT(false , "API:None is not support now !");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: {
			return new OpenGLVertexBuffer(vertices, size);
		}
		default:
			break;
		}
		TSO_CORE_ASSERT(false, "no API is chosen!");
		return nullptr;
	}

	VertexBuffer* VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: {
			TSO_CORE_ASSERT(false, "API:None is not support now !");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: {
			return new OpenGLVertexBuffer(size);
		}
		default:
			break;
		}
		TSO_CORE_ASSERT(false, "no API is chosen!");
		return nullptr;
	}


	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count) {

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: {
			TSO_CORE_ASSERT(false, "API:None is not support now !");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: {
			return new OpenGLIndexBuffer(indices, count);
		}
		default:
			break;
		}
		TSO_CORE_ASSERT(false, "no API is chosen!");
		return nullptr;


	}


}