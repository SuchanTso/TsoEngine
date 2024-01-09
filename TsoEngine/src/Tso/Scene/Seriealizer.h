#pragma once
#include "Entity.h"

namespace Tso {

	class Seriealizer {
	public:
		Seriealizer() {};
		Seriealizer(Scene* scene);
        ~Seriealizer() {};


		void SeriealizeScene(const std::string& path);
        
        bool DeseriealizeScene(const std::string& path);


	private:
		Scene* m_Scene;
	};

}
