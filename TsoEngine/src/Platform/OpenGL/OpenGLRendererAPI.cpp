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

		glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);//src , dst
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray) {
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}


}