#pragma once 

#include "TSO.h"
#include "Tso/Renderer/OrthographicCameraController.h"
#include "glm/glm.hpp"
#include "Tso/Renderer/Renderer2D.h"

class SandBox2D : public Tso::Layer {
public:
	SandBox2D();
	~SandBox2D() = default;

	virtual void OnImGuiRender() override;

	virtual void OnUpdate(Tso::TimeStep ts)override;

	virtual void OnEvent(Tso::Event& event)override;



private:

	Tso::OrthographicCameraController m_CameraController;
	glm::vec3 m_TrianglePos;
    Tso::Ref<Tso::Shader> m_Shader;
    Tso::Ref<Tso::ShaderLibrary> m_ShaderLibrary;
    Tso::Ref<Tso::Texture2D> m_Texture;
    float m_MoveSpeed = 1.0f;



};
