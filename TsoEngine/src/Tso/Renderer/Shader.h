#pragma once
#include "glm/glm.hpp"
#include "Tso/Core/UUID.h"

namespace Tso {
	class Shader
	{
	public:
		static Ref<Shader> Create(const std::string& name , std::string& vertexSrc, std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& filePath);

		virtual std::string GetName() = 0;
        
        virtual void SetName(const std::string& name) = 0;

		virtual ~Shader(){}

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

        
        virtual void SetInt(const std::string& name , const int& value) = 0;
		virtual void SetIntArray(const std::string& name, const int* values , uint32_t count) = 0;

        
        virtual void SetFloat(const std::string& name , const float& value) = 0;
        virtual void SetFloat2(const std::string& name , const glm::vec2& value) = 0;
        virtual void SetFloat3(const std::string& name , const glm::vec3& value) = 0;
        virtual void SetFloat4(const std::string& name , const glm::vec4& value) = 0;
        
        virtual void SetMatrix3(const std::string& name , const glm::mat3& matrix) = 0;
        virtual void SetMatrix4(const std::string& name , const glm::mat4& matrix) = 0;
        
        virtual std::unordered_map<unsigned int, std::string>& GetSource() = 0;
        
        virtual const char* GetShaderStageName(unsigned int stage) = 0;
        
        virtual bool Recompile(const std::unordered_map<unsigned int, std::string>& newSources) = 0;
        
        virtual std::string& GetPath() = 0;
        
        virtual void SetPath(const std::string& filePath) = 0;
        
        virtual void SetUUID(const UUID& uuid) = 0;
        
        virtual UUID& GetUUID() = 0;

	};


	class ShaderLibrary {
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string&name , const Ref<Shader>& shader);


		Ref<Shader> Load(const std::string& path);
		Ref<Shader> Load(const std::string& name , const std::string& path);
		Ref<Shader> Get(const std::string& name);



	private:
		std::unordered_map<std::string, Ref<Shader> > m_Shaders;

	};

}
