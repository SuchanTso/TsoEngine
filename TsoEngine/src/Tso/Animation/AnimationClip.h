//
//  AnimationClip.hpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/11.
//
#pragma once
#ifndef AnimationClip_hpp
#define AnimationClip_hpp
#include "Tso/Core/UUID.h"

namespace Tso {

    struct AnimationFrame {
        float Duration;
        glm::vec2 SpriteIndex;
        glm::vec2 SpriteSize;
//        UUID TextureUUID; // sprite animation textures should be gathered in one giant texture. so this uuid may invalid for now
    };

    class AnimationClip {
    public:
//        AnimationClip(const std::string& name);
        AnimationClip(const std::string& path);

        void AddFrame(const glm::vec2& spriteIndex,const glm::vec2& spriteSize, float duration);

        const std::string& GetName() const { return m_Name; }
        std::vector<AnimationFrame>& GetFramesMutable(){return m_Frames;}
        void SetName(const std::string& name){m_Name = name;}
        const std::vector<AnimationFrame>& GetFrames() const { return m_Frames; }
        float GetTotalDuration() const { return m_TotalDuration; }
        bool IsLooping() const { return m_Loop; }
        void SetLooping(bool loop) { m_Loop = loop; }
        UUID& GetTextureUUID(){return m_TextureUUID;}
        void SetTextureUUID(const UUID& uuid){m_TextureUUID = uuid;}

        UUID& GetUUID(){return m_UUID;}
        void SetUUID(const UUID& uuid){m_UUID = uuid;}
        
        std::string& GetPath(){return m_Path;}
        void SetPath(const std::string& path){m_Path = path;}
        
        void Deseralize(const std::string& filepath);
        
        static Ref<AnimationClip> Create(const std::string& name , const std::string& path);
        
    private:
        std::string m_Name;
        std::vector<AnimationFrame> m_Frames;
        float m_TotalDuration = 0.0f;
        bool m_Loop = true;
        UUID m_TextureUUID = 0;
        UUID m_UUID;
        std::string m_Path = "";
    };
}


#endif /* AnimationClip_hpp */
