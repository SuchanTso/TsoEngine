#pragma once 

#include "TSO.h"
#include "Tso/Renderer/OrthographicCameraController.h"
#include "glm/glm.hpp"
#include "Tso/Renderer/Renderer2D.h"
#include "BigWealth.h"

class BigWealthGameLayer : public Tso::Layer {
public:

	BigWealthGameLayer();
	~BigWealthGameLayer() = default;

	virtual void OnImGuiRender() override;

	virtual void OnUpdate(Tso::TimeStep ts)override;

	virtual void OnEvent(Tso::Event& event)override;

private:
	Tso::OrthographicCameraController m_CameraController;
	Tso::Ref<Tso::ShaderLibrary> m_ShaderLibrary = nullptr;
	Tso::Ref<Tso::Shader> m_Shader;
	Tso::Ref<Tso::Texture2D> m_Texture;
    
    BigWealth m_BigWealthLogic;

};
