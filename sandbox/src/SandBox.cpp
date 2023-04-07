#include <TSO.h>
class SandBox :public Tso::Application {
public:
	SandBox() {

	}
	~SandBox() {

	}
};

Tso::Application* Tso::CreateApplication() {
	return new SandBox();
}