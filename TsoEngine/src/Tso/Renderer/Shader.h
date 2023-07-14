#pragma once
#include "Renderer.h"

namespace Tso {
	class Shader
	{
	public:
		static Shader* Create(std::string& vertexSrc, std::string& fragmentSrc);

		virtual ~Shader(){}

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

	};

}