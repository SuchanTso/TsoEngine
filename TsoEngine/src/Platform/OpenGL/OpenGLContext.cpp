#include "TPch.h"
#include "OpenGLContext.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <GL/GL.h>


namespace Tso {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	:m_windowHandle(windowHandle)
	{
		TSO_CORE_ASSERT(windowHandle , "window handle is null!")
	}

	void OpenGLContext::Init() {
		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		TSO_CORE_ASSERT(status, "unable to load Glad");

	}

	void OpenGLContext::SwapBuffers() {
		glfwSwapBuffers(m_windowHandle);

	}

}