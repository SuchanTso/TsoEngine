//
//  SceneCamera.cpp
//  TsoEngine
//
//  Created by SuchanTso on 2024/1/4.
//
#include "TPch.h"
#include "SceneCamera.h"
#include "glm/gtc/matrix_transform.hpp"


namespace Tso{

void SceneCamera::ReCalculateProjection(){
    if(m_ProjectionType == ProjectionType::Projection){
        m_Projection = glm::perspective(m_ProjectionFov, m_AspectRatio, m_ProjectionNearClip, m_ProjectionFarClip);
    }
    else if(m_ProjectionType == ProjectionType::Orthographic){
        float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoBottom = -m_OrthographicSize * 0.5f;
        float orthoTop = m_OrthographicSize * 0.5f;

        m_Projection = glm::ortho(orthoLeft, orthoRight,orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
    }
    else{
        TSO_CORE_ASSERT(false , "unknown type");
    }
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
    TSO_CORE_ASSERT(width > 0 && height > 0 , "");
    m_AspectRatio = (float)width / (float)height;
    ReCalculateProjection();
}

}
