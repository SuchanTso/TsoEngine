#include "BigWealth.h"

BigWealth::BigWealth(std::string mapPath , const uint32_t& width, const uint32_t& height)
	:m_MapFilePath(mapPath) , m_Width(width) , m_Height(height)
{
	SetGameState(GameState::LogIn);
}

void BigWealth::OnLogin()
{
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

}

void BigWealth::LoadMapResource(const std::string& mapPath)
{
}

