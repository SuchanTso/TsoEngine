#include "TPch.h"
#include "Scene.h"
#include "Entity.h"

namespace Tso {


Entity Scene::CreateEntity(){
    entt::entity entityID = m_Registry.create();
    Entity res = Entity(entityID , this , "");
    return res;
}



}
