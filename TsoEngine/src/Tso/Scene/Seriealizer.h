#pragma once
#include "Entity.h"

namespace Tso {

	class Seriealizer {
	public:
		Seriealizer() {};
		Seriealizer(Scene* scene);
		~Seriealizer() = default;


		void SeriealizeScene(const std::string& path);


	private:
		Ref<Scene> m_Scene;
	};

}