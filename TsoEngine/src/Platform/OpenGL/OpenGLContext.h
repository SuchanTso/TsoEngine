#pragma once
#include "Tso/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Tso {
	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_windowHandle;
	};
}