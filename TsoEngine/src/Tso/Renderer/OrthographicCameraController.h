//
//  OrthographicCameraController.hpp
//  TsoEngine
//
//  Created by user on 2023/8/29.
//

#ifndef OrthographicCameraController_hpp
#define OrthographicCameraController_hpp

#include <stdio.h>
#include "Tso/Renderer/OrthographicCamera.h"
#include "Tso/Event/MouseEvent.h"
#include "Tso/Event/ApplicationEvent.h"


namespace Tso{

class OrthographicCameraController{
    
public:
    OrthographicCameraController(float aspectRatio , bool enableRotation = false);
    
    OrthographicCamera& GetCamera(){ return m_Camera; }
    
    void OnUpdate(TimeStep ts);
    
    void OnResize(const float& width , const float& height);
    
    void OnEvent(Event& e);
private:

    bool OnMouseScrolledEvent(MouseScrolledEvent& e);

    bool OnWindowResizeEvent(WindowResizeEvent& e);
    
private:
    OrthographicCamera m_Camera;
    float m_AspectRatio = 1.0f;
    float m_ZoomLevel = 1.0f;
    bool m_EnableRotation = false;
    glm::vec3 m_CameraPosition = glm::vec3(0.f);
    float m_CameraRotation = 0.f;
    float m_MoveSpeed = 1.0f;
    float m_RotationSpeed = 180.0f;
    
};


}
#endif /* OrthographicCameraController_hpp */
