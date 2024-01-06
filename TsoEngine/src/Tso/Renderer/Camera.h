//
//  Camera.h
//  TsoEngine
//
//  Created by SuchanTso on 2024/1/4.
//

#ifndef Camera_h
#define Camera_h
#include "glm/glm.hpp"


namespace Tso {

class Camera{
public:
        Camera() = default;
        Camera(const glm::mat4& projection)
            : m_Projection(projection) {}

        virtual ~Camera() = default;

        const glm::mat4& GetProjection() const { return m_Projection; }
    protected:
        glm::mat4 m_Projection = glm::mat4(1.0f);
};

}

#endif /* Camera_h */
