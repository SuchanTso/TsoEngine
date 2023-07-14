#include "TPch.h"
#include "OpenGLShader.h"
#include "glad/glad.h"

namespace Tso {

	OpenGLShader::OpenGLShader(std::string& vertexSrc,std::string& fragmentSrc) {
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const char* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		glCompileShader(vertexShader);

		int isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			int maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<char>infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			TSO_CORE_ERROR("[shader] vertex shader error : {0}", infoLog.data());

			glDeleteShader(vertexShader);
			TSO_CORE_ASSERT(false);
			return;
		}


		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		glCompileShader(fragmentShader);

		isCompiled = 0;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			int maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<char>infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			TSO_CORE_ERROR("[shader] frag shader error : {0}", infoLog.data());

			glDeleteShader(fragmentShader);
			TSO_CORE_ASSERT(false);
			return;
		}

		m_RendererId = glCreateProgram();

		glAttachShader(m_RendererId, vertexShader);
		glAttachShader(m_RendererId, fragmentShader);

		glLinkProgram(m_RendererId);

		int isLinked = 0;

		glGetProgramiv(m_RendererId, GL_LINK_STATUS, &isLinked);

		if (isLinked == GL_FALSE) {
			int maxLength = 0;
			glGetProgramiv(m_RendererId, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<char>infoLog(maxLength);
			glGetProgramInfoLog(m_RendererId, maxLength, &maxLength, &infoLog[0]);

			TSO_CORE_ERROR("[shader program] link error : {0}", infoLog.data());

			glDeleteProgram(m_RendererId);
			TSO_CORE_ASSERT(false);
			return;
		}

		glDetachShader(m_RendererId, vertexShader);
		glDetachShader(m_RendererId, fragmentShader);



	}

	void OpenGLShader::Bind() {
		glUseProgram(m_RendererId);
	}

	void OpenGLShader::UnBind() {
		glUseProgram(0);
	}

}