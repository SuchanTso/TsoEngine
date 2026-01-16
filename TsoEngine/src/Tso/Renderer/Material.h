//
//  Material.hpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/10/29.
//

#ifndef Material_hpp
#define Material_hpp

// Tso/Renderer/Material.h

#pragma once
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <string>
#include <map>
#include "Tso/Core/UUID.h"

namespace Tso {

    class Material {
    public:
        Material(Ref<Shader> shader, const std::string& name = "Unnamed Material");
        
        Material(const std::string& name = "Unnamed Material");

        static Ref<Material> Create(Ref<Shader> shader, const std::string& name = "Unnamed Material");
        
        static Ref<Material> Create(const std::string& name = "nnamed Material");
        
        static Ref<Material> Create(const std::string& path , const UUID& uuid);

        void Bind() const;

        void Unbind() const;

        // --- Uniform 设置接口 ---
        void SetFloat(const std::string& name, float value);
        void SetInt(const std::string& name, int value);
        void SetVec2(const std::string& name, const glm::vec2& value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetMat3(const std::string& name, const glm::mat3& value);
        void SetMat4(const std::string& name, const glm::mat4& value);
        
        std::unordered_map<std::string, float>& GetFloatParams() { return m_FloatParams; }
        std::unordered_map<std::string, glm::vec3>& GetVec3Params() { return m_Vec3Params; }
        std::unordered_map<std::string, Ref<Texture2D>>& GetTextureParams() { return m_TextureParams; }

        // 设置纹理
        void SetTexture(const std::string& name, const Ref<Texture2D>& texture);
        
        // --- Getter ---
        void SetShader(Ref<Shader>shader){m_Shader = shader;}
        Ref<Shader> GetShader() const { return m_Shader; }
        const std::string& GetName() const { return m_Name; }
        UUID& GetRendererID(){return m_RendererID;}
        void SetRendererID(const UUID& uuid){m_RendererID = uuid;}
        void SetPureColor(const glm::vec4& color){m_Color = color;}
        glm::vec4& GetPureColor(){return m_Color;}
        
        std::string& GetPath(){return m_filePath;}
        void SetPath(const std::string& filePath){m_filePath = filePath;}
        
        void Serealize();
        
        static Ref<Material> Desealize(const std::string& path , const UUID& uuid);
        
        void SetUUID(const UUID& uuid){m_UUID = uuid;}
        
        UUID& GetUUID(){return m_UUID;}
        
        void DefineInstanceParam(const std::string& name, int slotIndex);
        
        int GetInstanceParamSlot(const std::string& name) const;
        
        void ClearInstanceSlots(){m_InstanceParamMap.clear();}
        
        const std::unordered_map<std::string, int>& GetInstanceParams() const { return m_InstanceParamMap; }

        bool operator>=(Material& other) {
            return other.m_UUID >= other.m_UUID;
        }
    private:
        Ref<Shader> m_Shader;
        std::string m_Name;
        std::string m_filePath;
        UUID m_RendererID = 0;
        glm::vec4 m_Color = glm::vec4(1.f);
        std::unordered_map<std::string, float> m_FloatParams;
        std::unordered_map<std::string, int> m_IntParams;
        std::unordered_map<std::string, glm::vec2> m_Vec2Params;
        std::unordered_map<std::string, glm::vec3> m_Vec3Params;
        std::unordered_map<std::string, glm::vec4> m_Vec4Params;
        std::unordered_map<std::string, glm::mat3> m_Mat3Params;
        std::unordered_map<std::string, glm::mat4> m_Mat4Params;
        
        std::unordered_map<std::string, Ref<Texture2D>> m_TextureParams;
        
        UUID m_UUID;
        std::unordered_map<std::string, int> m_InstanceParamMap;

    };

}

#endif /* Material_hpp */
