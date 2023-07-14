#include "TPch.h"
#include "OpenGLBuffer.h"
#include "glad/glad.h"

namespace Tso {
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
		
		glCreateBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	}

	void OpenGLVertexBuffer::Bind()const {
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGLVertexBuffer::UnBind()const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//------------------------------------------indexBuffer---------------------------------------------------------------------
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) 
		:m_Count(count)
	{

		glCreateBuffers(1, &m_RendererId);
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