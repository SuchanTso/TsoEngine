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
    enum class ProjectionType{
        Projection = 0,
        Orthographic = 1
    };
public:
    SceneCamera();
    ~SceneCamera() = default;
    
    ProjectionType GetProjectionType(){return m_ProjectionType;}
    void SetProjectionType(ProjectionType& type){m_ProjectionType = type;}
    
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
    
    float m_AspectRatio = 0.0f;
};
    
}

#endif /* SceneCamera_hpp */
