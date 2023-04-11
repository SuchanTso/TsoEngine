#include "TSO.h"

class TestLayer : public Tso::Layer{
public:
  TestLayer():
    Layer("testLayer"){}
    
    void OnUpdate()override{
        TSO_INFO("testLayer OnUpdate");
        printf("testLayer OnUpdate\n");
    }
    
    void OnEvent(Tso::Event& event)override{
        TSO_INFO("testLayer Event:{0}",event.ToString());
        printf("testLayer [%s]\n",event.ToString().c_str());
    }
};

class SandBox :public Tso::Application {
public:
	SandBox() {
        PushLayer(new TestLayer());
	}
	~SandBox() {

	}
};

Tso::Application* Tso::CreateApplication() {
	return new SandBox();
}
