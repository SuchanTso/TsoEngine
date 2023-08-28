#include "TPch.h"
#include "OpenGLTexture2D.h"
#include "stb_image.h"
#include "glad/glad.h"

Tso::OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
{
	int width, height ,channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data) {
		TSO_CORE_ERROR( "failed to load {0}", path.c_str());
	}
	m_Width = width;
	m_Height = height;

	GLenum internalChannel = 0, rgb = 0;

	if (channels == 4) {
		internalChannel = GL_RGBA8;
		rgb = GL_RGBA;
	}
	else if (channels == 3) {
		internalChannel = GL_RGB8;
		rgb = GL_RGB;
	}
	else
	{
		TSO_CORE_ERROR("Invalid image file");
	}

#ifdef TSO_PLATFORM_WINDOWS
	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

	glTextureStorage2D(m_RendererID, 1, internalChannel, m_Width, m_Height);

	glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, rgb, GL_UNSIGNED_BYTE, data);
#else
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D,m_RendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, rgb, width, height, 0, rgb, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);

#endif

	stbi_image_free(data);

}

Tso::OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &m_RendererID);
}

void Tso::OpenGLTexture2D::Bind(const unsigned int slot) const
{
#ifdef TSO_PLATFORM_WINDOWS
	glBindTextureUnit(slot, m_RendererID);
#else
    glActiveTexture(GL_TEXTURE0 + slot);
#endif
    glBindTexture(GL_TEXTURE_2D, m_RendererID);


}
