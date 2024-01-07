//
//  SceneCamera.hpp
//  TsoEngine
//
//  Created by SuchanTso on 2024/1/4.
//

#ifndef SceneCamera_hpp
#define SceneCamera_hpp

#include <stdio.h>
#include "Tso/Renderer/Camera.h"

namespace Tso {
    
class SceneCamera : public Camera{
public:
    enum class ProjectionType{
        Projection = 0,
        Orthographic = 1
    };
public:
    SceneCamera() {}
    ~SceneCamera() = default;
    
    ProjectionType GetProjectionType(){return m_ProjectionType;}
    void SetProjectionType(ProjectionType type){m_ProjectionType = type; ReCalculateProjection();}

    float GetProjectionFov() { return m_ProjectionFov; }
    void SetProjectionFov(const float& fov) { m_ProjectionFov = fov; ReCalculateProjection(); }

    float GetProjectionNearClip() { return m_ProjectionNearClip; }
    void SetProjectionNearClip(const float& nearClip) { m_ProjectionNearClip = nearClip; ReCalculateProjection(); }

    float GetProjectionFarClip() { return m_ProjectionFarClip; }
    void SetProjectionFarClip(const float& farClip) { m_ProjectionFarClip = farClip; ReCalculateProjection(); }

    float GetOrthographicSize() { return m_OrthographicSize; }
    void SetOrthographicSize(const float& size) { m_OrthographicSize = size; ReCalculateProjection(); }

    float GetOrthographicNearClip() { return m_OrthographicNear; }
    void SetOrthographicNearClip(const float& nearClip) { m_OrthographicNear = nearClip; ReCalculateProjection(); }

    float GetOrthographicFarClip() { return m_OrthographicFar; }
    void SetOrthographicFarClip(const float& farClip) { m_OrthographicFar = farClip; ReCalculateProjection(); }
    

    void SetViewportSize(uint32_t width, uint32_t height);

private:
    void ReCalculateProjection();
    
private:
    ProjectionType m_ProjectionType = ProjectionType::Orthographic;
    float m_ProjectionFov = glm::radians(45.f);
    float m_ProjectionNearClip = 0.01f;
    float m_ProjectionFarClip = 1000.f;
    
    
    float m_OrthographicSize = 10.0f;
    float m_OrthographicNear = -1.0f;
    float m_OrthographicFar = 1.0f;
    
    float m_AspectRatio = 1.667f;
};
    
}

#endif /* SceneCamera_hpp */
