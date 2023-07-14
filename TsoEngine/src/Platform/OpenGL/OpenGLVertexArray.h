#pragma once

#include "Tso/Renderer/VertexArray.h"

namespace Tso {

	class OpenGLVertexArray : public VertexArray {
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual void Bind()const override;

		virtual void UnBind()const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers()const override { return m_VertexBuffers; }
		virtual std::shared_ptr<IndexBuffer> GetIndexBuffer()const override { return m_IndexBuffer; }

	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		uint32_t m_RendererId;

	};

}