//
//  CollideListener.cpp
//  TsoEngine
//
//  Created by user on 2024/1/19.
//
#include "TPch.h"
#include "CollideListener.h"
#include "box2d/b2_contact.h"
#include "Component.h"

namespace Tso{

void NativeContactListener::BeginContact(b2Contact* contact){
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    // 获取关联的body以及用户数据
    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();
    NativeScriptComponent* userDataARef = (NativeScriptComponent*)bodyA->GetUserData().pointer;
    NativeScriptComponent* userDataBRef = (NativeScriptComponent*)bodyB->GetUserData().pointer;

    if(userDataARef && userDataARef->Instance){
        userDataARef->Instance->OnCollide() ;
    }
    if(userDataBRef && userDataBRef->Instance){
        userDataBRef->Instance->OnCollide() ;
    }
    // 将b2BodyUserData转换回GameObject指针
    
    
}




}
