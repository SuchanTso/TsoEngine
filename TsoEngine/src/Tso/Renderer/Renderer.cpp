#include "TPch.h"
#include "Renderer.h"

namespace Tso {

	//RenderAPI Renderer::s_RenderAPI = RenderAPI::OpenGL;


	void Renderer::BeginScene() {

	}

	void Renderer::EndScene() {

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray) {

		RenderCommand::DrawIndexed(vertexArray);

	}

}