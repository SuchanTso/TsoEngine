#pragma once 

#include "TSO.h"
#include "Tso/Renderer/OrthographicCameraController.h"
#include "glm/glm.hpp"
#include "Tso/Renderer/Renderer2D.h"
#include "Tso/Renderer/FrameBuffer.h"

class SandBox2D : public Tso::Layer {
    
    struct MovaData{
        glm::vec2 originPos;
        glm::vec2 targetPos;
        float startTime;
        
    };
    
public:
	SandBox2D();
	~SandBox2D() = default;

	virtual void OnImGuiRender() override;

	virtual void OnUpdate(Tso::TimeStep ts)override;

	virtual void OnEvent(Tso::Event& event)override;

private:
    template<typename T>
    T LinearInterpretMove(const float & start , const float& duration , const float& timestamp ,const T& pos , const T& target,bool& movable);
    
    bool OnMouseButton(Tso::MouseButtonPressedEvent& e);
    
    bool OnMouseMove(Tso::MouseMovedEvent& e);

private:

	Tso::OrthographicCameraController m_CameraController;
	glm::vec3 m_TrianglePos;
    Tso::Ref<Tso::Shader> m_Shader;
    Tso::Ref<Tso::ShaderLibrary> m_ShaderLibrary;
    Tso::Ref<Tso::Texture2D> m_Texture , m_TileTexture;
    Tso::Ref<Tso::SubTexture2D> m_subTexture , m_sub1;
    Tso::Ref<Tso::FrameBuffer> m_FrameBuffer;

    float m_MoveSpeed = 1.0f;
    bool m_LpMovable = false;
    float m_Time = 0.0;
    MovaData m_MoveData;
    
    float m_MouseX = 0.f , m_MouseY = 0.f;



};
