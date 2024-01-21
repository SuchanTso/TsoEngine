#include "TPch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"

namespace Tso {




	Ref<Texture2D> Texture2D::Create(std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case  RendererAPI::API::None: {
				TSO_CORE_ASSERT(false, "None RenderAPI is not supported!");
				return nullptr;
			}
			case  RendererAPI::API::OpenGL: {
				TSO_CORE_INFO("OpenGL is chosen as Render API");
				return std::make_shared<OpenGLTexture2D>(path);
			}
			default: {
				TSO_CORE_ASSERT(false, "None RenderAPI chosen!");
				return nullptr;
			}
		}
	}


Ref<Texture2D> Texture2D::Create(const int& width , const int& height){
    switch (Renderer::GetAPI())
    {
        case  RendererAPI::API::None: {
            TSO_CORE_ASSERT(false, "None RenderAPI is not supported!");
            return nullptr;
        }
        case  RendererAPI::API::OpenGL: {
            TSO_CORE_INFO("OpenGL is chosen as Render API");
            return std::make_shared<OpenGLTexture2D>(width , height);
        }
        default: {
            TSO_CORE_ASSERT(false, "None RenderAPI chosen!");
            return nullptr;
        }
    }
}

Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec){
    switch (Renderer::GetAPI())
    {
        case  RendererAPI::API::None: {
            TSO_CORE_ASSERT(false, "None RenderAPI is not supported!");
            return nullptr;
        }
        case  RendererAPI::API::OpenGL: {
            TSO_CORE_INFO("OpenGL is chosen as Render API");
            return std::make_shared<OpenGLTexture2D>(spec);
        }
        default: {
            TSO_CORE_ASSERT(false, "None RenderAPI chosen!");
            return nullptr;
        }
    }
}


}
