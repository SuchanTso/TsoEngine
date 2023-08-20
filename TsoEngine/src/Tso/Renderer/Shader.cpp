#include "TPch.h"
#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer.h"

namespace Tso {
	Shader* Shader::Create(std::string& vertexSrc, std::string& fragmentSrc) {
		switch (Renderer::GetAPI())
		{
			case  RendererAPI::API::None: {
				TSO_CORE_ASSERT(false, "None RenderAPI is not supported!");
				return nullptr;
			}
			case  RendererAPI::API::OpenGL: {
				TSO_CORE_INFO("OpenGL is chosen as Render API");
				return new OpenGLShader(vertexSrc, fragmentSrc);
			}
			default: {
				TSO_CORE_ASSERT(false, "None RenderAPI chosen!");
				return nullptr;
			}
		}
	}


	Shader* Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case  RendererAPI::API::None: {
			TSO_CORE_ASSERT(false, "None RenderAPI is not supported!");
			return nullptr;
		}
		case  RendererAPI::API::OpenGL: {
			TSO_CORE_INFO("OpenGL is chosen as Render API");
			return new OpenGLShader(filePath);
		}
		default: {
			TSO_CORE_ASSERT(false, "None RenderAPI chosen!");
			return nullptr;
		}
		}
	}
}