#pragma once
#include "Tso/Renderer/Shader.h"

typedef unsigned int GLenum;
namespace Tso {
	class OpenGLShader : public Shader {
	public:

		OpenGLShader(const std::string& name , std::string& vertexSrc,std::string& fragmentSrc);
		OpenGLShader(const std::string& filePath);


		virtual void Bind()override;

		virtual void UnBind()override;

		virtual std::string GetName() override { return m_Name; }

		void UploadMatrix3(const std::string& name, const glm::mat3& matrix);
		void UploadMatrix4(const std::string& name, const glm::mat4& matrix);

		void UploadFloat(const std::string& name, const float& value);
		void UploadFloat2(const std::string& name, const glm::vec2& value);
		void UploadFloat3(const std::string& name, const glm::vec3& value);
		void UploadFloat4(const std::string& name, const glm::vec4& value);

		void UploadInt(const std::string& name, const int& value);

	private:
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSource);
		std::string ReadFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& shaderSources);
        int GetUniformLocation(const std::string& name);





	private:
		uint32_t m_RendererId;
        std::unordered_map<std::string, int>m_UniformCache;
		std::string m_Name;

	};
}
