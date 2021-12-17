#include <DaemonEngine.h>
#include <DaemonEngine/EntryPoint.h>

#include "ExampleLayer.h"
#include "Examples/SceneLayer.h"
#include "Examples/PlatformerGame.h"

class SandboxApp : public Daemon::Application
{
public:
	SandboxApp(const Daemon::ApplicationSpecification specification)
		: Application(specification)
	{
	}

	virtual void OnInit() override
	{
		PushLayer(new ExampleLayer());
		//PushLayer(new SceneLayer());
		//PushLayer(new PlatformerGame());
	}
};

extern Daemon::Application* Daemon::CreateApplication()
{
	Daemon::ApplicationSpecification specification;
	specification.Name = "Sandbox";
	specification.EnableImGui = true;
	return new SandboxApp(specification);
}