//
//  MapUnit.hpp
//  Sandbox
//
//  Created by SuchanTso on 2023/9/28.
//

#ifndef MapUnit_hpp
#define MapUnit_hpp
#include "TSO.h"
#include "rapidjson/document.h"
#include "Player.h"

class MapUnit{
public:
    
    void OnLoad(rapidjson::Value& value);
    void OnPass(Tso::Ref<Player> player);
    void OnTouch(Tso::Ref<Player> player);
    
private:
    uint32_t m_UnitID = 0;
    uint32_t m_OwnerID = 0;//0 is invalid ID
    std::string m_DisplayText = "";
    bool m_AllowPassby = true;
    long long m_Price = 0;
    
};


#endif /* MapUnit_hpp */
