#pragma once

//main game logic storage

// created by suchanTso
#include "TSO.h"
#include "Player.h"


enum GameState {
	None		= 0,		//do nothing
	LogIn		= 1,		//login and fetch a player ID
	Init		= 2,		//load resources such as model and textures
    Ready       = 3,        //done Init map and player , ready to start
	Actiong		= 4,		//time to use your feature card
	Gambling	= 5,		//roll and roll
	Calculate	= 6,		//time to move and trigger map event.
	GameOver	= 7,		//end of game

};

class BigWealth {
public:
	BigWealth() {}
	BigWealth(std::string mapPath , const uint32_t& width, const uint32_t& height);
    ~BigWealth() = default;
	GameState& GetGameState() { return m_State; }
	void SetGameState(const GameState& state) { m_State = state; }

	void OnLogin();
	void OnInit(const std::string& mapPath );
    void OnAction();
    void OnGambling();
    void OnCaculate();
    void OnRender();

private:
	void LoadMapResource(const std::string& mapPath);


private:
	GameState m_State = GameState::None;
	uint32_t m_RoomID = 0;
	int m_PlayerID = -1;// -1 means i have no player to control
	std::string m_MapFilePath;
	uint32_t m_Width = 100, m_Height = 100;//set world scale 100 * 100 squares
	std::unordered_map<int, Tso::Ref<Player>> m_Players;
};
