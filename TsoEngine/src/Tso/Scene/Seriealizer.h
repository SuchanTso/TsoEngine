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
        
        Ref<Texture2D> GetTextureByPath(std::string& path);


	private:
		Scene* m_Scene;
        std::unordered_map<std::string, Ref<Texture2D>> m_TextureCache;
		std::unordered_map<uint64_t, uint64_t> m_ParentMap;
	};

}
