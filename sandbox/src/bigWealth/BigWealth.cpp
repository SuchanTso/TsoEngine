#include "BigWealth.h"

BigWealth::BigWealth(std::string mapPath , const uint32_t& width, const uint32_t& height)
	:m_MapFilePath(mapPath) , m_Width(width) , m_Height(height)
{
	SetGameState(GameState::LogIn);
}

void BigWealth::OnLogin()
{
    //test code//////////////////////
    m_RoomID = 1;
    m_Players[1] = std::make_shared<Player>("asset/lp2.png" , 1);
    
    //test code ///////////////////////
    
    
    
	if (m_RoomID == -1) {
		m_RoomID = 1;//temp code ,get a room ID form server in case multi rooms are needed
	}

	if (m_RoomID > 0 && m_PlayerID == -1) {
		//TODO: connect to server and create a player instance
	}
	
	
	if (true/* network permit to start*/) {
		SetGameState(GameState::Init);
	}
	
}

void BigWealth::OnInit(const std::string& mapPath)
{
	m_MapFilePath = mapPath;
	//LoadPlayerModel();
	LoadMapResource(mapPath);
    
    //init player
    SetGameState(GameState::Ready);
    //notify server that i'm ready
    
    for(auto it = m_Players.begin() ; it != m_Players.end() ; it++){
        it->second->OnLoad(glm::vec3(0.0 , 0.0 , 0.1));
    }

    
    //test code set state gamble
    SetGameState(GameState::Gambling);
}

void BigWealth::OnAction(){
    
}

void BigWealth::OnGambling(){
    //request get now gamble playerID
    
    int nowPlayerID = 1;//test code
    short gambleRes = -1;
    if(m_Players.find(nowPlayerID) != m_Players.end()){
        gambleRes = m_Players[nowPlayerID]->OnGamble(nowPlayerID);
    }
    if(gambleRes < 0){
        TSO_ERROR("Unresonable res from gambling : {0}" , gambleRes);
        SetGameState(GameState::GameOver);
        return;
    }
    //call network to return your gamble result
    
    
}

void BigWealth::OnCaculate(){
    
}


void BigWealth::OnRender(){
    for(auto it = m_Players.begin() ; it != m_Players.end() ; it++){
        it->second->OnRender();
    }
}



void BigWealth::LoadMapResource(const std::string& mapPath)
{
    //rapidJson read map @字文辉 + 毛珂
}

