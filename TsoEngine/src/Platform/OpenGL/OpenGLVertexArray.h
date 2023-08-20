#pragma once

#include "Tso/Renderer/VertexArray.h"

namespace Tso {

	class OpenGLVertexArray : public VertexArray {
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual void Bind()const override;

		virtual void UnBind()const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual std::vector<Ref<VertexBuffer>> GetVertexBuffers()const override { return m_VertexBuffers; }
		virtual Ref<IndexBuffer> GetIndexBuffer()const override { return m_IndexBuffer; }

	private:
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;

		uint32_t m_RendererId;

	};

}