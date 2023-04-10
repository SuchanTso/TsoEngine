#pragma once
#ifdef TSO_PLATFORM_WINDOWS
extern Tso::Application* Tso::CreateApplication();

int main(int argc,char** argv) {
	//this main function is the same as been write in client
	//because every time u execute the client app, it should include this file
	Tso::Log::Init();
	TSO_CORE_ERROR("initialized Log!");
	int a = 2;
	TSO_INFO(" hello ! a = {0}",a);


	auto app = Tso::CreateApplication();
	app->Run();
	delete app;

	return 0;
	}
#elif defined TSO_PLATFORM_MACOSX
extern Tso::Application* Tso::CreateApplication();

int main(int argc,char** argv) {
    //this main function is the same as been write in client
    //because every time u execute the client app, it should include this file
//    Tso::Log::Init();
//    TSO_CORE_ERROR("initialized Log!");
    int a = 2;
//    TSO_INFO(" hello ! a = {0}",a);


    auto app = Tso::CreateApplication();
    app->Run();
    delete app;

    return 0;
    }




#endif
