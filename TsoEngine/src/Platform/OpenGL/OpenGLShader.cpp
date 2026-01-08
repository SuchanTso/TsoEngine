#include "TPch.h"
#include "OpenGLShader.h"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <iostream>
#include "glad/glad.h"


namespace Tso {


    const char* OpenGLShader::GetShaderStageName(unsigned int stage){
        switch (stage) {
            case 0x8B31: return "Vertex Shader";   // GL_VERTEX_SHADER
            case 0x8B30: return "Fragment Shader"; // GL_FRAGMENT_SHADER
            case 0x8B32: return "Geometry Shader"; // GL_GEOMETRY_SHADER
            // 添加其他阶段...
        }
        return "Unknown Shader";
    }

    static const char* GLShaderStageToString(GLenum stage) {
        switch (stage) {
            case GL_VERTEX_SHADER:   return "vertex";
            case GL_FRAGMENT_SHADER: return "fragment";
            case GL_GEOMETRY_SHADER: return "geometry";
            case GL_COMPUTE_SHADER:  return "compute";
        }
        TSO_CORE_ASSERT(false, "Unknown shader stage!");
        return "unknown";
    }

	static GLenum StringToGLEnum(const std::string& type) {

		if (type == "vertex" || type == "Vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "fragMent") 
			return GL_FRAGMENT_SHADER;
		
		TSO_CORE_ASSERT(false, "wrong type find in shader");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& name ,std::string& vertexSrc,std::string& fragmentSrc) 
		: m_Name(name)
	{
		
        m_Source[GL_VERTEX_SHADER] = vertexSrc;
        m_Source[GL_FRAGMENT_SHADER] = fragmentSrc;

		Compile(m_Source);

	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
    :m_filePath(filePath)
	{
		std::string shaderSrcs = ReadFile(filePath);
		m_Source = PreProcess(shaderSrcs);
		Compile(m_Source);

		auto last_slash = filePath.find_last_of("/\\");
		auto start = last_slash == std::string::npos ? 0 : last_slash + 1;
		auto last_dot = filePath.rfind(".");
		int count = last_dot == std::string::npos ?  filePath.length() - start : last_dot - start;

		m_Name = filePath.substr(start, count);

	}

	void OpenGLShader::Bind() {
		glUseProgram(m_RendererId);
	}

	void OpenGLShader::UnBind() {
		glUseProgram(0);
	}

	void OpenGLShader::UploadMatrix3(const std::string& name, const glm::mat3& matrix)
	{
        int location = GetUniformLocation(name);
		if (location < 0) {
//			TSO_CORE_ERROR("[shader error] : not found uniform named {0}", name);
		}
		else {
			glProgramUniformMatrix3fv(m_RendererId, location, 1, GL_FALSE, glm::value_ptr(matrix));
		}
	}

	void OpenGLShader::UploadMatrix4(const std::string& name, const glm::mat4& matrix) {
        int location = GetUniformLocation(name);
		if (location < 0) {
//			TSO_CORE_ERROR("[shader error] : not found uniform named {0}", name);
		}
		else {
			glProgramUniformMatrix4fv(m_RendererId, location, 1, GL_FALSE, glm::value_ptr(matrix));
		}
	}

	void OpenGLShader::UploadFloat(const std::string& name, const float& value)
	{
        int location = GetUniformLocation(name);
		if (location < 0) {
//			TSO_CORE_ERROR("[shader error] : not found uniform named {0}", name);
		}
		else {
			glProgramUniform1f(m_RendererId, location, value);
		}
	}

	void OpenGLShader::UploadFloat2(const std::string& name, const glm::vec2& value)
	{
        int location = GetUniformLocation(name);
		if (location < 0) {
//			TSO_CORE_ERROR("[shader error] : not found uniform named {0}", name);
		}
		else {
			glProgramUniform2f(m_RendererId, location , value.x , value.y);
		}
	}

	void OpenGLShader::UploadFloat3(const std::string& name, const glm::vec3& value)
	{
		int location = GetUniformLocation(name);
		if (location < 0) {
//			TSO_CORE_ERROR("[shader error] : not found uniform named {0}", name);
		}
		else {
			glProgramUniform3f(m_RendererId, location, value.x, value.y , value.z);
		}
	}

	void OpenGLShader::UploadFloat4(const std::string& name, const glm::vec4& value)
	{
        int location = GetUniformLocation(name);
		if (location < 0) {
//			TSO_CORE_ERROR("[shader error] : not found uniform named {0}", name);
		}
		else {
			glProgramUniform4f(m_RendererId, location, value.x, value.y, value.z , value.w);
		}
	}

	void OpenGLShader::UploadInt(const std::string& name, const int& value)
	{
        int location = GetUniformLocation(name);
		if (location < 0) {
//			TSO_CORE_ERROR("[shader error] : not found uniform named {0}", name);
		}
		else {
			glProgramUniform1i(m_RendererId, location, value);
		}
	}

	void OpenGLShader::UploadIntArray(const std::string& name, const int* values, uint32_t count)
	{
		int location = GetUniformLocation(name);
		if (location < 0) {
			//			TSO_CORE_ERROR("[shader error] : not found uniform named {0}", name);
		}
		else {
			glProgramUniform1iv(m_RendererId, location, count , values);
		}
	}

void OpenGLShader::SetInt(const std::string& name , const int& value){
    UploadInt(name , value);
}

void OpenGLShader::SetIntArray(const std::string& name, const int* values, uint32_t count)
{
	UploadIntArray(name, values , count);

}



void OpenGLShader::SetFloat(const std::string& name , const float& value){
    UploadFloat(name , value);
}
void OpenGLShader::SetFloat2(const std::string& name , const glm::vec2& value){
    UploadFloat2(name , value);

}
void OpenGLShader::SetFloat3(const std::string& name , const glm::vec3& value){
    UploadFloat3(name , value);

}
void OpenGLShader::SetFloat4(const std::string& name , const glm::vec4& value){
    UploadFloat4(name , value);

}

void OpenGLShader::SetMatrix3(const std::string& name , const glm::mat3& matrix){
    UploadMatrix3(name , matrix);
}
void OpenGLShader::SetMatrix4(const std::string& name , const glm::mat4& matrix){
    UploadMatrix4(name , matrix);
}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSource)
	{
		auto program = glCreateProgram();
		std::unordered_map<GLenum, unsigned int>shaderIds;
		for (auto& kv : shaderSource) {
			GLenum type = kv.first;
			std::string shaderSrc = kv.second;


			unsigned int shader = glCreateShader(type);
			shaderIds[type] = shader;

			const char* source = shaderSrc.c_str();
			glShaderSource(shader, 1, &source, 0);

			glCompileShader(shader);

			int isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE) {
				int maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<char>infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				TSO_CORE_ERROR("[shader] vertex shader error : {0}", infoLog.data());

				glDeleteShader(shader);
				TSO_CORE_ASSERT(false , "");
				return;
			}
			glAttachShader(program, shader);

		}
		

		m_RendererId = program;


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
			TSO_CORE_ASSERT(false , "");
			return;
		}
		for (auto& kv : shaderSource) {
			GLenum type = kv.first;
			glDetachShader(m_RendererId, shaderIds[type]);
		}


	}

    bool OpenGLShader::Recompile(const std::unordered_map<unsigned int, std::string>& newSources) {
        // 1. 尝试编译新代码
        auto newProgram = glCreateProgram();
        bool compiled = true;
        std::unordered_map<GLenum, unsigned int>shaderIds;
        for (auto& kv : newSources) {
            GLenum type = kv.first;
            std::string shaderSrc = kv.second;


            unsigned int shader = glCreateShader(type);
            shaderIds[type] = shader;

            const char* source = shaderSrc.c_str();
            glShaderSource(shader, 1, &source, 0);

            glCompileShader(shader);

            int isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                int maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
                std::vector<char>infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                TSO_CORE_ERROR("[shader] vertex shader error : {0}", infoLog.data());

                glDeleteShader(shader);
                compiled = false;
            
                
            }
            glAttachShader(newProgram, shader);

        }
        
        
        if (newProgram == 0 || !compiled) {
            return false; // 编译失败，保持原样
        }

        // 2. 编译成功：替换 GPU 资源
        glDeleteProgram(m_RendererId);
        m_RendererId = newProgram;

        // 3. 更新内存中的源码缓存（这一步很重要！）
        // 如果不更新，下次获取 GetSource() 还是旧代码
        // m_OpenGLSourceCode 是你在类里存储源码的成员变量
        m_Source = newSources;
        
        if (!m_filePath.empty()) {
            bool saved = SaveToFile(m_filePath, newSources);
            if (saved) {
                TSO_CORE_INFO("Shader source saved to '{0}'", m_filePath);
            }
        } else {
            TSO_CORE_WARN("Shader recompiled but NOT saved: FilePath is empty (created from string?)");
        }

        return true;
    }

bool OpenGLShader::SaveToFile(const std::string& filepath, const std::unordered_map<GLenum, std::string>& sources) {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        TSO_CORE_ERROR("Could not open file '{0}' for writing shader", filepath);
        return false;
    }

    for (auto& [stage, source] : sources) {
        std::string stageName = GLShaderStageToString(stage);
        
        // 1. 写入分隔标记
        out << "#type " << stageName << "\n";
        
        // 2. 写入源码
        // 注意：InputText 有可能删除了换行符，或者多加了换行符，这里最好保证格式整洁
        out << source;
        
        // 保证每个阶段之间有换行
        if (source.empty() || source.back() != '\n') {
            out << "\n";
        }
    }

    out.close();
    return true;
}

	std::string OpenGLShader::ReadFile(const std::string& filePath)
	{
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		std::string result = "";

		if (in) {

			in.seekg(0 , std::ios::end);
			result.resize(in.tellg());
			in.seekg(0 , std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}

		else {
			TSO_CORE_ERROR("Unknown file path '{0}'" , filePath.c_str());
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& shaderSources)
	{
		std::unordered_map<GLenum, std::string> shaderMap;
		std::string distinctToken = "#type";
		size_t pos = shaderSources.find(distinctToken);
		size_t typeLength = distinctToken.length();
		while (pos != std::string::npos) {
			size_t endofLine = shaderSources.find_first_of("\r\n" , pos);
			TSO_CORE_ASSERT(endofLine != std::string::npos, "Shader syntax error!");
			std::string type = shaderSources.substr(pos + typeLength + 1, endofLine - (pos + typeLength + 1));

			size_t nextLinePos = shaderSources.find_first_not_of("\r\n", endofLine);
			pos = shaderSources.find(distinctToken, nextLinePos);

			shaderMap[StringToGLEnum(type)] = shaderSources.substr(nextLinePos , pos - (nextLinePos == std::string::npos ? shaderSources.size() - 1 : nextLinePos));

			
		}
		return shaderMap;
	}

int OpenGLShader::GetUniformLocation(const std::string& name){
    int res = -1;
    if(m_UniformCache.find(name) != m_UniformCache.end()){
        res = m_UniformCache[name];
        if(res < 0){
            res = glGetUniformLocation(m_RendererId, name.c_str());
            m_UniformCache[name] = res;
        }
    }
    else{
        res = glGetUniformLocation(m_RendererId, name.c_str());
        m_UniformCache[name] = res;
        if(res < 0){
            TSO_CORE_ERROR("[shader error] : not found uniform named {0}", name);
        }
    }
    
    return res;
}



}
