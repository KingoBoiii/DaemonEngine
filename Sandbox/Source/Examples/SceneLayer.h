#pragma once
#include <DaemonEngine.h>

class SceneLayer : public Daemon::Layer
{
public:
	SceneLayer()
		: Layer("Scene Example Layer"), m_Scene(Daemon::Scene())
	{
	}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Daemon::Event& e) override;
	virtual void OnGUI() override;
private:
	bool OnWindowResize(Daemon::WindowResizeEvent& e);
private:
	Daemon::Scene m_Scene;
};


