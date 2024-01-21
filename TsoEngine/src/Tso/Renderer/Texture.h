#pragma once
#include "Tso/Core/Core.h"

namespace Tso {

enum class ImageFormat{
    RGB8 = 0,
    RGBA8 = 1,
};


struct TextureSpecification{
    int Width = 1;
    int Height = 1;
    ImageFormat Format = ImageFormat::RGBA8;
    bool GenerateMips = false;
};

	class Texture {

	public:
		~Texture(){}
		virtual uint32_t GetWidth()const = 0;

		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(const unsigned int slot = 0)const = 0;

		virtual bool operator==(const Texture& other)const = 0;

	};

	class Texture2D : public Texture {
	public:
		~Texture2D() {}
		virtual uint32_t GetWidth()const override = 0;

		virtual uint32_t GetHeight() const override = 0;

		virtual void Bind(const unsigned int slot = 0)const override = 0;
        
        virtual void SetData(void* data , const uint32_t& size) = 0;
        
        virtual std::string& GetPath() = 0;
        
        virtual uint32_t GetTextureID() = 0;

		static Ref<Texture2D> Create(std::string& path);
        
        static Ref<Texture2D> Create(const int& width , const int& height);
        
        static Ref<Texture2D> Create(const TextureSpecification& spec);

	};


}
