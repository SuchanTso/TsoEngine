#pragma once
#include <glm/glm.hpp>

namespace Tso {
	class Shader
	{
	public:
		static Shader* Create(std::string& vertexSrc, std::string& fragmentSrc);
		static Shader* Create(const std::string& filePath);

		virtual ~Shader(){}

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		//virtual void UploadMatrix4(const std::string& name , const glm::mat4& matrix) = 0;

	};

}