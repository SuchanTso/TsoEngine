#pragma once 

#include "TSO.h"
#include "Tso/Renderer/OrthographicCameraController.h"
#include "glm/glm.hpp"

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

};