#pragma once
#include "glm/glm.hpp"


namespace Tso {

	class OrthographicCamera {

	public:
		
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetPorjectionMatrix(const glm::mat4& proj) { m_ProjMatrix = proj; RecalculateMatrix();}

		void SetViewMatrix(const glm::mat4& view) { m_ViewMatrix = view; RecalculateMatrix();}

		void SetRotationZ(const float& rotation_z) { m_rotationZ = rotation_z; RecalculateMatrix(); }

		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateMatrix(); }
        
        void SetPorjectionMatrix(float left, float right, float bottom, float top);

        

		glm::mat4 GetProjectionMatrix() { return m_ProjMatrix; }

		glm::mat4 GetViewMatrix() { return m_ViewMatrix; }

		float GetRotationZ() { return m_rotationZ; }

		glm::mat4 GetProjViewMatrix() { return m_ProjViewMatrix; }

	private:
		void RecalculateMatrix();


	private:

		glm::mat4 m_ProjMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjViewMatrix;
		glm::vec3 m_Position;
		float m_rotationZ;

	};

}
