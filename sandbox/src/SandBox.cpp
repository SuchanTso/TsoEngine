#include "TSO.h"

class TestLayer : public Tso::Layer{
public:
  TestLayer():
    Layer("testLayer"){
  
  }
    
    void OnUpdate()override{
        //TSO_INFO("testLayer OnUpdate");
//        printf("testLayer OnUpdate\n");
        if (Tso::Input::IsKeyPressed(TSO_KEY_TAB)) {
            TSO_INFO("tab pressed");
        }
    }
    
    void OnEvent(Tso::Event& event)override{
        TSO_INFO("testLayer Event:{0}",event.ToString());
    }
};

class SandBox :public Tso::Application {
public:
	SandBox() {
        PushLayer(new TestLayer());
        //PushOverlay(new Tso::ImGuiLayer());
	}
	~SandBox() {

	}
};

Tso::Application* Tso::CreateApplication() {
	return new SandBox();
}
