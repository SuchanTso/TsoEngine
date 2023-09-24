#pragma once
extern Tso::Application* Tso::CreateApplication();

int main(int argc,char** argv) {
	//this main function is the same as been write in client
	//because every time u execute the client app, it should include this file
	Tso::Log::Init();
	TSO_CORE_ERROR("initialized Log!");


	auto app = Tso::CreateApplication();
	app->Run();
	delete app;

	return 0;
	}

