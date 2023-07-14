#include "TPch.h"
#include "OpenGLVertexArray.h"
#include "glad/glad.h"

namespace Tso {

	static GLenum ConvertShaderDataType2OpenGLType(ShaderDataType type) {
		switch (type)
		{
			case Tso::ShaderDataType::None:		return 0;
			case Tso::ShaderDataType::Float:	return GL_FLOAT;
			case Tso::ShaderDataType::Float2:	return GL_FLOAT;
			case Tso::ShaderDataType::Float3:	return GL_FLOAT;
			case Tso::ShaderDataType::Float4:	return GL_FLOAT;
			case Tso::ShaderDataType::Mat3:		return GL_FLOAT;
			case Tso::ShaderDataType::Mat4:		return GL_FLOAT;
			case Tso::ShaderDataType::Int:		return GL_INT;
			case Tso::ShaderDataType::Int2:		return GL_INT;
			case Tso::ShaderDataType::Int3:		return GL_INT;
			case Tso::ShaderDataType::Int4:		return GL_INT;
			case Tso::ShaderDataType::Bool:		return GL_BOOL;
		}

		TSO_CORE_ASSERT(false, "Unknown ShaderDataType here!");
		return 0;
	}


	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_RendererId);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1 , &m_RendererId);
	}

	void OpenGLVertexArray::Bind()const {
		glBindVertexArray(m_RendererId);
	}

	void OpenGLVertexArray::UnBind()const {
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
		TSO_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererId);
		vertexBuffer->Bind();
		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ConvertShaderDataType2OpenGLType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			index++;
		}
		m_VertexBuffers.push_back(vertexBuffer);

	}
	

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
		glBindVertexArray(m_RendererId);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}