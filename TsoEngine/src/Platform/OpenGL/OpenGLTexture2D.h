#pragma once
#include "Tso/Renderer/Texture.h"
#include "glad/glad.h"


namespace Tso {

	class OpenGLTexture2D : public Texture2D {

	public:
		OpenGLTexture2D(const std::string& path);
        
        OpenGLTexture2D(const int& width , const int& height);

		~OpenGLTexture2D();

		virtual uint32_t GetWidth()const override { return m_Width; }

		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(const unsigned int slot = 0)const override;
        
        virtual void SetData(void* data)override;
        
        virtual std::string& GetPath()override{return m_TexturePath;}

		virtual bool operator==(const Texture& other)const override {
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

	private:

		uint32_t m_Width, m_Height;

		unsigned int m_RendererID;
        
        GLenum  m_InternalChannel = 0, m_RGB = 0;
        
        std::string m_TexturePath = "";
	};

}
