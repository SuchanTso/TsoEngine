//
//  CollideListener.cpp
//  TsoEngine
//
//  Created by user on 2024/1/19.
//
#include "TPch.h"
#include "CollideListener.h"
#include "box2d/b2_contact.h"
#include "Tso/Scene/Component.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Scene/Entity.h"

#include "Tso/Scripting/ScriptingEngine.h"

namespace Tso{
    NativeContactListener::NativeContactListener(Scene* scene)
        :m_Scene(scene)
    {
    }
    void NativeContactListener::BeginContact(b2Contact* contact) {
        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();

        // 获取关联的body以及用户数据
        b2Body* bodyA = fixtureA->GetBody();
        b2Body* bodyB = fixtureB->GetBody();
        uint32_t* userDataARef = (uint32_t*)bodyA->GetUserData().pointer;
        uint32_t* userDataBRef = (uint32_t*)bodyB->GetUserData().pointer;

        Entity entityA = { (entt::entity)*userDataARef , m_Scene };
        Entity entityB = { (entt::entity)*userDataBRef , m_Scene };

        TSO_CORE_INFO("entity {} 's name is {}", entityA.GetUUID(), entityA.GetComponent<TagComponent>().m_Name);
        TSO_CORE_INFO("entity {} 's name is {}", entityB.GetUUID(), entityB.GetComponent<TagComponent>().m_Name);

        ScriptingEngine::OnCollideEntity(entityA, entityB);
        ScriptingEngine::OnCollideEntity(entityB, entityA);


        //ScriptingEngine::I

        /*if(userDataARef && userDataARef->Instance){
            userDataARef->Instance->OnCollide() ;
        }
        if(userDataBRef && userDataBRef->Instance){
            userDataBRef->Instance->OnCollide() ;
        }*/
        // 将b2BodyUserData转换回GameObject指针

    }
}

