#include "TPch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Tso {

	//RenderAPI Renderer::s_RenderAPI = RenderAPI::OpenGL;
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;


	void Renderer::BeginScene(OrthographicCamera& camera) {
		m_SceneData->ProjViewMatrix = camera.GetProjViewMatrix();
	}

	void Renderer::EndScene() {

	}

	void Renderer::Submit(const Ref<VertexArray>& vertexArray , const Ref<Shader>& shader , const glm::mat4 transform) {

		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadMatrix4("u_ProjViewMat", m_SceneData->ProjViewMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadMatrix4("u_Transform", transform);

		RenderCommand::DrawIndexed(vertexArray);

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewPort(0 , 0 , width , height);

	}

}