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

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

	glTextureStorage2D(m_RendererID, 1, internalChannel, m_Width, m_Height);

	glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, rgb, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

}

Tso::OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &m_RendererID);
}

void Tso::OpenGLTexture2D::Bind(const unsigned int slot) const
{

	glBindTextureUnit(slot, m_RendererID);

}
