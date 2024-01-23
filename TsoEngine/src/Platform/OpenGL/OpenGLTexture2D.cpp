#include "TPch.h"
#include "OpenGLTexture2D.h"
#include "stb_image.h"

namespace Tso {


static GLenum ConvertImageFormatToInternalGLFormat(const ImageFormat& iamgeFormat){
    switch (iamgeFormat) {
        case ImageFormat::RGB8:return GL_RGB8;
        case ImageFormat::RGBA8:return GL_RGBA8;
            
        TSO_CORE_ASSERT(false , "unsupport ImageFormat");
    }
}

static GLenum ConvertImageFormatToGLFormat(const ImageFormat& iamgeFormat){
    switch (iamgeFormat) {
        case ImageFormat::RGB8:return GL_RGB;
        case ImageFormat::RGBA8:return GL_RGBA;
            
        TSO_CORE_ASSERT(false , "unsupport ImageFormat");
    }
}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
:m_TexturePath(path)
{
    int width, height ,channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        TSO_CORE_ERROR( "failed to load {0}", path.c_str());
    }
    m_Width = width;
    m_Height = height;
    
    
    if (channels == 4) {
        m_InternalChannel = GL_RGBA8;
        m_RGB = GL_RGBA;
    }
    else if (channels == 3) {
        m_InternalChannel = GL_RGB8;
        m_RGB = GL_RGB;
    }
    else
    {
        TSO_CORE_ERROR("Invalid image file");
    }
    
    Invalidate(data);
    
    stbi_image_free(data);
    
}

OpenGLTexture2D::OpenGLTexture2D(const int& width , const int& height){
    m_Width = width;
    m_Height = height;
    
    m_InternalChannel = GL_RGBA8, m_RGB = GL_RGBA;
}

OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec){
    m_Width = spec.Width;
    m_Height = spec.Height;
    m_InternalChannel = ConvertImageFormatToInternalGLFormat(spec.Format);
    m_RGB = ConvertImageFormatToGLFormat(spec.Format);
    m_GenMipmap = spec.GenerateMips;
}


OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &m_RendererID);
}

void OpenGLTexture2D::Bind(const unsigned int slot) const
{
#ifdef TSO_PLATFORM_WINDOWS
    glBindTextureUnit(slot, m_RendererID);
#else
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
#endif
    
    
    
}

void OpenGLTexture2D::Invalidate(void* data){
#ifdef TSO_PLATFORM_WINDOWS
    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    
    glTextureStorage2D(m_RendererID, 1, m_InternalChannel, m_Width, m_Height);
    
    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
#else
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D,m_RendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_RGB, m_Width, m_Height, 0, m_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(m_GenMipmap){
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D,0);
    
#endif
}


void OpenGLTexture2D::SetData(void *data , const uint32_t& size){
    
    uint32_t bpp = m_RGB == GL_RGBA ? 4 : 3;
    TSO_CORE_ASSERT(size == m_Width * m_Height * bpp, "incomplete size of texture!");
    Invalidate(data);
}

}
