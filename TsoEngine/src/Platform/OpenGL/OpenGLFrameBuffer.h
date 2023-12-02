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


		virtual uint32_t GetColorAttachment()const override { return m_ColorAttachMent; }
		virtual uint32_t GetDepthAttachment()const override { return m_DepthAttachMent; }

	private:
		void Invalidate();

	private:

		FrameBufferInfo m_FrameInfo;

		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachMent = 0;
		uint32_t m_DepthAttachMent = 0;



	};
}
