#pragma once

#include "Tso/Renderer/RendererAPI.h"

namespace Tso {

	class OpenGLRendererAPI : public RendererAPI {
	public:

		virtual void SetClearColor(const glm::vec4& color)override;

		virtual void Clear() override;

		virtual void Init() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;


	};

}