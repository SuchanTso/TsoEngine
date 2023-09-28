#pragma once

#include "MapUnit.h"
class MapManager{
    
public:
    MapManager(){}
    MapManager(const std::string& mapPath);
    ~MapManager() = default;
    
    
    void OnLoad();
    void OnTouch(const uint32_t& playerID , const uint32_t& mapID);
    
private:
    std::string m_MapPath;
    std::UnorderedMap<uint32_t , Tso::Ref<MapUnit>> m_Map;
    

};
