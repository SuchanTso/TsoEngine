#pragma once

#include "Tso/Renderer/FrameBuffer.h"

namespace Tso {
	class OpenGLFrameBuffer : public FrameBuffer {
	public:
		OpenGLFrameBuffer(){}
		OpenGLFrameBuffer(const FrameBufferInfo& info);

		~OpenGLFrameBuffer();

		static Ref<OpenGLFrameBuffer> Create(const FrameBufferInfo& info);

		virtual void Bind() override;
		virtual void UnBind() override;
        virtual void Resize(const uint32_t& width , const uint32_t& height) override;


		virtual uint32_t GetColorAttachment(const uint32_t& index = 0)const override {TSO_CORE_ASSERT(index < m_ColorAttachMents.size(),"invalid index") return m_ColorAttachMents[index]; }
		virtual uint32_t GetDepthAttachment()const override { return m_DepthAttachMent; }

	private:
		void Invalidate();

	private:

		FrameBufferInfo m_FrameInfo;

		uint32_t m_RendererID = 0;
		std::vector<uint32_t> m_ColorAttachMents;
		uint32_t m_DepthAttachMent = 0;



	};
}
