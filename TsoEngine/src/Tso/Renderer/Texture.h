#pragma once
#include "Tso/Core.h"
#include <string>

namespace Tso {

	class Texture {

	public:
		~Texture(){}
		virtual uint32_t GetWidth()const = 0;

		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(const unsigned int slot = 0)const = 0;

	};

	class Texture2D : public Texture {
	public:
		~Texture2D() {}
		virtual uint32_t GetWidth()const override = 0;

		virtual uint32_t GetHeight() const override = 0;

		virtual void Bind(const unsigned int slot = 0)const override = 0;

		static Ref<Texture2D> Create(std::string& path);
	};


}
