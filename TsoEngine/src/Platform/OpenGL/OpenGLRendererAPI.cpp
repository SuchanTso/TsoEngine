#include "TPch.h"
#include "OpenGLRendererAPI.h"
#include "glad/glad.h"

namespace Tso {

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) {
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);

		glEnable(GL_DEPTH_TEST);

		glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);//src , dst
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) {
		uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
        TSO_CORE_INFO("openGL get window resized ({0} , {1} , {2} , {3})" , x , y , width , height);
		glViewport(x , y , width , height);
	}




}
