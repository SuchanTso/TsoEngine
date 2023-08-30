#include "TPch.h"
#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer.h"

namespace Tso {
	Ref<Shader> Shader::Create(const std::string& name ,std::string& vertexSrc, std::string& fragmentSrc) {
		switch (Renderer::GetAPI())
		{
			case  RendererAPI::API::None: {
				TSO_CORE_ASSERT(false, "None RenderAPI is not supported!");
				return nullptr;
			}
			case  RendererAPI::API::OpenGL: {
				TSO_CORE_INFO("OpenGL is chosen as Render API");
				return std::make_shared<OpenGLShader>(name , vertexSrc, fragmentSrc);
			}
			default: {
				TSO_CORE_ASSERT(false, "None RenderAPI chosen!");
				return nullptr;
			}
		}
	}


	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case  RendererAPI::API::None: {
			TSO_CORE_ASSERT(false, "None RenderAPI is not supported!");
			return nullptr;
		}
		case  RendererAPI::API::OpenGL: {
			TSO_CORE_INFO("OpenGL is chosen as Render API");
			return std::make_shared <OpenGLShader>(filePath);
		}
		default: {
			TSO_CORE_ASSERT(false, "None RenderAPI chosen!");
			return nullptr;
		}
		}
	}
	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto name = shader->GetName();
		TSO_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "[Shader Library] : failed to add shader , because {0} already exist !", name);
		m_Shaders[name] = shader;

	}
	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		TSO_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "[Shader Library] : failed to add shader , because {0} already exist !", name);
		m_Shaders[name] = shader;
	}
	Ref<Shader> ShaderLibrary::Load(const std::string& path)
	{
		auto shader = Shader::Create(path);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& path)
	{
		auto shader = Shader::Create(path);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		return m_Shaders[name];
	}


	
}