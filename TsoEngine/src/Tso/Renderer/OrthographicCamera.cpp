#include "TPch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Tso {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		:m_ProjMatrix(glm::ortho(left, right , bottom , top , -1.f , 1.f)) , m_ViewMatrix(glm::mat4(1.0f)) , m_Position(glm::vec3(0.f)) ,m_rotationZ(0.f){
			m_ProjViewMatrix = m_ProjMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateMatrix() {
		glm::mat4 transform = glm::translate(glm::mat4(1.0), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_rotationZ), glm::vec3(0.f, 0.f, 1.0f));
		m_ViewMatrix = glm::inverse(transform);
		m_ProjViewMatrix = m_ProjMatrix * m_ViewMatrix;
	}
}
