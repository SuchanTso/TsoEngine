//
//  AnimationClip.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2026/1/11.
//
#include "TPch.h"
#include "AnimationClip.h"
#include "Tso/Project/ProjectSerielizer.h"

namespace Tso{

AnimationClip::AnimationClip(const std::string& name)
: m_Name(name) {}


Ref<AnimationClip> AnimationClip::Create(const std::string& name , const std::string& path){
    YAML::Node data;
    try {
        data = YAML::LoadFile(path);
    }
    catch (YAML::ParserException e) {
        TSO_CORE_ASSERT(false ,"Failed to load animation file '{0}'\n     {1}", path, e.what());
        return nullptr;
    }
    auto animationClip = CreateRef<AnimationClip>(name);
    auto textureUUID = data["Texture"].as<uint64_t>();
    animationClip->SetTextureUUID(textureUUID);
    animationClip->SetPath(path);
    auto frames = data["Frames"];
    for(auto frame : frames){
        glm::vec2 spriteIndex = frame["SpriteIndex"].as<glm::vec2>();
        glm::vec2 spriteSize = frame["SpriteSize"].as<glm::vec2>();
        float duration = frame["Duration"].as<float>();
//        int index = frame["Index"].as<int>();//TODO: sort by index
        animationClip->AddFrame(spriteIndex, spriteSize, duration);
    }

    return animationClip;
}



void AnimationClip::Deseralize(const std::string& filepath) {
    if (filepath.empty()) {
        TSO_CORE_ERROR("Cannot save animation [{0}] to an empty path", m_Name);
        return;
    }

    YAML::Emitter out;
    {
        out << YAML::BeginMap; // Root
        out << YAML::Key << "Texture" << YAML::Value << m_TextureUUID;
        // frames may have different texture one day

        out << YAML::Key << "Frames" << YAML::Value << YAML::BeginSeq;

        for(int i = 0 ; i < m_Frames.size() ; i++){
            auto& frame = m_Frames[i];
            out << YAML::BeginMap;
            out << YAML::Key << "SpriteIndex" << YAML::Value << frame.SpriteIndex;
            out << YAML::Key << "SpriteSize" << YAML::Value << frame.SpriteSize;
            out << YAML::Key << "Duration" << YAML::Value << frame.Duration;
            out << YAML::Key << "Index" << YAML::Value << i;
            out << YAML::EndMap;
        }

        

        out << YAML::EndSeq; // Frames
        out << YAML::EndMap; // Root
    }

    std::ofstream fout(filepath);
    fout << out.c_str();
    
    TSO_CORE_INFO("Animation clip saved successfully to: {0}", filepath);
}

void AnimationClip::AddFrame(const glm::vec2& spriteIndex,const glm::vec2& spriteSize, float duration){
    m_Frames.push_back({duration, spriteIndex , spriteSize});
    m_TotalDuration += duration;
}



}
