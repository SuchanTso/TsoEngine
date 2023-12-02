#include "TPch.h"

#include "OpenGLFrameBuffer.h"
#include "glad/glad.h"

namespace Tso {
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferInfo& info)
		:m_FrameInfo(info)
	{

		Invalidate();

	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachMent);
		glDeleteTextures(1, &m_DepthAttachMent);
	}
	Ref<OpenGLFrameBuffer> OpenGLFrameBuffer::Create(const FrameBufferInfo& info)
	{

		return std::make_shared<OpenGLFrameBuffer>(info);
	}
	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	}
	void OpenGLFrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

void OpenGLFrameBuffer::Resize(const uint32_t& width , const uint32_t& height){
    m_FrameInfo.width = width;
    m_FrameInfo.height = height;
    Invalidate();
}

	void OpenGLFrameBuffer::Invalidate()
	{
        
        if(m_RendererID){
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(1, &m_ColorAttachMent);
            glDeleteTextures(1, &m_DepthAttachMent);
        }
        
#ifdef TSO_PLATFORM_WINDOWS
		glCreateFramebuffers(1, &m_RendererID);
#else
        glGenFramebuffers(1 , &m_RendererID);
#endif
        
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

#ifdef TSO_PLATFORM_WINDOWS
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachMent);
#else
        glGenTextures(1, &m_ColorAttachMent);
#endif
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachMent);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_FrameInfo.width, m_FrameInfo.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachMent, 0);


#ifdef TSO_PLATFORM_WINDOWS
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachMent);
#else
        glGenTextures(1, &m_DepthAttachMent);
#endif
//		glBindTexture(GL_TEXTURE_2D, m_DepthAttachMent);
//		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_FrameInfo.width, m_FrameInfo.height);
//
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachMent, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { 
			TSO_CORE_ERROR("FrameBuffer is incomplete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
}
