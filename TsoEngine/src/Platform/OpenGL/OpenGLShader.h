#pragma once
#include "Tso/Renderer/Shader.h"

namespace Tso {
	class OpenGLShader : public Shader {
	public:

		OpenGLShader(std::string& vertexSrc,std::string& fragmentSrc);

		virtual void Bind()override;

		virtual void UnBind()override;

	private:
		uint32_t m_RendererId;

	};
}