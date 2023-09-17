//
//  OrthographicCameraController.cpp
//  TsoEngine
//
//  Created by user on 2023/8/29.
//

#include "TPch.h"
#include "OrthographicCameraController.h"

namespace Tso{

OrthographicCameraController::OrthographicCameraController(float aspectRatio , bool enableRotation)
:m_AspectRatio(aspectRatio) , m_EnableRotation(enableRotation) , m_ZoomLevel(1.0f),
    m_Camera(-aspectRatio , aspectRatio , -1.0 , 1.0)
{
    TSO_CORE_INFO("cameraController coustruct : aspectRatio = {0}" , aspectRatio);
    
}

void OrthographicCameraController::OnUpdate(TimeStep ts) {
    
    if (Input::IsKeyPressed(TSO_KEY_W)) {
        m_CameraPosition.y += m_MoveSpeed * ts;
    }
    if (Input::IsKeyPressed(TSO_KEY_S)) {
        m_CameraPosition.y -= m_MoveSpeed * ts;
    }
    if (Input::IsKeyPressed(TSO_KEY_A)) {
        m_CameraPosition.x -= m_MoveSpeed * ts;
    }
    if (Input::IsKeyPressed(TSO_KEY_D)) {
        m_CameraPosition.x += m_MoveSpeed * ts;
    }
    if(m_EnableRotation){
        if (Input::IsKeyPressed(TSO_KEY_LEFT)) {
            m_CameraRotation += m_RotationSpeed * ts;
        }
        else if (Input::IsKeyPressed(TSO_KEY_RIGHT)) {
            m_CameraRotation -= m_RotationSpeed * ts;
        }
    }
    
    m_Camera.SetPosition(m_CameraPosition);
    m_Camera.SetRotationZ(m_CameraRotation);
}

void OrthographicCameraController::OnEvent(Event& e){
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolledEvent));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResizeEvent));

    
}

bool OrthographicCameraController::OnMouseScrolledEvent(MouseScrolledEvent& e){
    float yOffset = e.GetYOffset();
    m_ZoomLevel += yOffset;
    m_ZoomLevel = std::max(m_ZoomLevel , 0.1f);
    m_MoveSpeed = m_ZoomLevel;
    m_Camera.SetPorjectionMatrix(-m_AspectRatio * m_ZoomLevel , m_AspectRatio * m_ZoomLevel , -m_ZoomLevel , m_ZoomLevel);
    
    return true;
}

bool OrthographicCameraController::OnWindowResizeEvent(WindowResizeEvent& e)
{
    m_AspectRatio = e.GetWidth() * 1.0f / e.GetHeight();

    TSO_CORE_INFO("cameraController resizeEvent : aspectRatio = {0}", m_AspectRatio);


    m_Camera.SetPorjectionMatrix(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

    return false;
}





}



