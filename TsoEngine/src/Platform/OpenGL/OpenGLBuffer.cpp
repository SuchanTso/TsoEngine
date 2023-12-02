#include "TPch.h"
#include "OpenGLBuffer.h"
#include "glad/glad.h"

namespace Tso {
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
		
#ifdef TSO_PLATFORM_WINDOWS
        glCreateBuffers(1, &m_RendererId);
#else
        glGenBuffers(1, &m_RendererId);
#endif
        
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
#ifdef TSO_PLATFORM_WINDOWS
		glCreateBuffers(1, &m_RendererId);
#else
		glGenBuffers(1, &m_RendererId);
#endif

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	void OpenGLVertexBuffer::Bind()const {
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGLVertexBuffer::UnBind()const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	//------------------------------------------indexBuffer---------------------------------------------------------------------
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) 
		:m_Count(count)
	{
#ifdef TSO_PLATFORM_WINDOWS
		glCreateBuffers(1, &m_RendererId);
#else
        glGenBuffers(1, &m_RendererId);
#endif
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , m_RendererId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	}

	void OpenGLIndexBuffer::Bind()const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGLIndexBuffer::UnBind()const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


}