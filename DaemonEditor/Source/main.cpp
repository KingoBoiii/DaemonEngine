#include <DaemonEngine/EntryPoint.h>
#include "EditorLayer.h"

class DaemonEditorApplication : public Daemon::Application
{
public:
	DaemonEditorApplication(const Daemon::ApplicationSpecification specification)
		: Application(specification)
	{
	}

	virtual void OnInit() override
	{
		PushLayer(new Daemon::EditorLayer());
	}
};

extern Daemon::Application* Daemon::CreateApplication()
{
	Daemon::ApplicationSpecification specification;
	specification.EnableImGui = true;
	return new DaemonEditorApplication(specification);
}