#pragma once
#include "Tso/Core/Core.h"
#include "Project.h"

namespace YAML
{
    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        
        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;
            
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        
        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;
            
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        
        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;
            
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::mat3> {
        static bool decode(const Node& node, glm::mat3& rhs) {
            if (!node.IsSequence() || node.size() != 9) return false;
            // glm::mat3 是列主序，这里假设 YAML 也是列主序或者你需要转置
            for (int i = 0; i < 9; i++) rhs[i / 3][i % 3] = node[i].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::mat4> {
        static bool decode(const Node& node, glm::mat4& rhs) {
            if (!node.IsSequence() || node.size() != 16) return false;
            for (int i = 0; i < 16; i++) rhs[i / 4][i % 4] = node[i].as<float>();
            return true;
        }
    };

    // 矩阵通常比较大，不建议用 Flow (单行)，这里简单处理为扁平数组
    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat3& v) {
        out << YAML::Flow << YAML::BeginSeq;
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) out << v[i][j];
        out << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat4& v) {
        out << YAML::Flow << YAML::BeginSeq;
        for(int i=0; i<4; i++) for(int j=0; j<4; j++) out << v[i][j];
        out << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }
    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    
}

namespace Tso {

	class ProjectSerielizer {
	public:
		ProjectSerielizer() = delete;
		ProjectSerielizer(Ref<Project> project);
		~ProjectSerielizer() = default;

		bool Serielize(const std::string& prjPath);
		bool Deserieleze(const std::string& prjPath);

	private:
		Ref<Project> m_Project = nullptr;
	};


}
