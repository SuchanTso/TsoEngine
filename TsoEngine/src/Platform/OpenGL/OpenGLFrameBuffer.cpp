#include "TPch.h"

#include "OpenGLFrameBuffer.h"
#include "glad/glad.h"

namespace Tso {

static GLenum ConvertColorFormatToGLInternalFormat(const FrameBufferFormat& framebufferFormat){
    switch (framebufferFormat) {
        case RGBA8 : return GL_RGBA8;
        case RGB8  : return GL_RGB8;
        case RED_INTEGER: return GL_R32I;
            
        default:
            TSO_CORE_ASSERT(false , "{} format is not supported now" , framebufferFormat);
            break;
    }
}

static GLenum ConvertColorFormatToGLFormat(const FrameBufferFormat& framebufferFormat){
    switch (framebufferFormat) {
        case RGBA8 : return GL_RGBA;
        case RGB8  : return GL_RGB;
        case RED_INTEGER: return GL_RED_INTEGER;

            
        default:
            TSO_CORE_ASSERT(false , "{} format is not supported now" , framebufferFormat);
            break;
    }
}

static bool isDepthAttachment(const FrameBufferFormat& framebufferFormat){
    switch (framebufferFormat) {
        case RGBA8 :
        case RGB8  : return false;
        case RED_INTEGER : return false;
        case DEPTH24_STENCIL8:return true;
            
        default:
            TSO_CORE_ASSERT(false , "{} format is not supported now" , framebufferFormat);
            break;
    }
    return false;
}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferInfo& info)
		:m_FrameInfo(info)
	{

		Invalidate();

	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachMents.size(), m_ColorAttachMents.data());
        m_ColorAttachMents.clear();
		glDeleteTextures(1, &m_DepthAttachMent);
	}
	Ref<OpenGLFrameBuffer> OpenGLFrameBuffer::Create(const FrameBufferInfo& info)
	{

		return std::make_shared<OpenGLFrameBuffer>(info);
	}
	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_FrameInfo.width, m_FrameInfo.height);

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
        glDeleteTextures(m_ColorAttachMents.size(), m_ColorAttachMents.data());
        m_ColorAttachMents.clear();
        glDeleteTextures(1, &m_DepthAttachMent);
    }
        
#ifdef TSO_PLATFORM_WINDOWS
		glCreateFramebuffers(1, &m_RendererID);
#else
        glGenFramebuffers(1 , &m_RendererID);
#endif
        
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    
    for(int i = 0 ; i < m_FrameInfo.format.size() ; i++){
        if(isDepthAttachment(m_FrameInfo.format[i])){
#ifdef TSO_PLATFORM_WINDOWS
            glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachMent);
#else
            glGenTextures(1, &m_DepthAttachMent);
#endif
            glBindTexture(GL_TEXTURE_2D, m_DepthAttachMent);
            //        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_FrameInfo.width, m_FrameInfo.height);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_FrameInfo.width, m_FrameInfo.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);//TODO: make depth format convert function when needed @Suchan
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachMent, 0);
            m_DepthAttachmentSpec = m_FrameInfo.format[i];
        }
        
        else{
            uint32_t colorAttachment = 0;
#ifdef TSO_PLATFORM_WINDOWS
            glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachment);
#else
            glGenTextures(1, &colorAttachment);
#endif
            glBindTexture(GL_TEXTURE_2D, colorAttachment);
            
            glTexImage2D(GL_TEXTURE_2D, 0, ConvertColorFormatToGLInternalFormat(m_FrameInfo.format[i]),
                         m_FrameInfo.width, m_FrameInfo.height, 0, ConvertColorFormatToGLFormat(m_FrameInfo.format[i]),
                         GL_UNSIGNED_BYTE, nullptr);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            int offset = m_ColorAttachMents.size();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + offset, GL_TEXTURE_2D, colorAttachment, 0);
            
            m_ColorAttachMents.push_back(colorAttachment);
            m_ColorAttachmentSpec.push_back(m_FrameInfo.format[i]);
        }
        
    }
    
    
    
    if (m_ColorAttachMents.size() > 1){
        TSO_CORE_ASSERT(m_ColorAttachMents.size() <= 4 , "support attachment no more than 4");
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(m_ColorAttachMents.size(), buffers);
    }
    else if (m_ColorAttachMents.empty()){
        // Only depth-pass
        glDrawBuffer(GL_NONE);
    }
    
    
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            TSO_CORE_ASSERT(false,"FrameBuffer is incomplete!");
        }
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}



int OpenGLFrameBuffer::ReadOnePixel(const int& attachmentIndex , const int& x , const int& y){
    TSO_CORE_ASSERT(attachmentIndex < m_ColorAttachMents.size(),"invalid attachment index");
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    int pixelData;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}

void OpenGLFrameBuffer::ClearAttachment(const int& attachmentIndex , const int& value){
    TSO_CORE_ASSERT(attachmentIndex < m_ColorAttachMents.size(),"invalid attachment index");
    
#ifdef TSO_PLATFORM_WINDOWS
    auto& spec = m_ColorAttachmentSpec[attachmentIndex];
    glClearTexImage(m_ColorAttachMents[attachmentIndex] , 0 , ConvertColorFormatToGLFormat(spec) , GL_INT , &v);
#else
    //to be implement in macOS
    GLint values[4] = {value};
    //    glClearBufferiv(GL_COLOR, m_ColorAttachmentSpec[attachmentIndex] , values);
#endif
}


}
