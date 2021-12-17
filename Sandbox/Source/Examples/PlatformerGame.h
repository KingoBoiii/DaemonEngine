#pragma once
#include <DaemonEngine.h>

class PlatformerGame : public Daemon::Layer
{
public:
	PlatformerGame()
		: Layer("Platformer Game Layer"), m_Scene(Daemon::Scene())
	{
	}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Daemon::Event& e) override;
	virtual void OnGUI() override;
private:
	void Regenerate(int32_t height);
private:
	Daemon::Scene m_Scene;

	std::vector<Daemon::Entity> m_Entities;
	Daemon::Shared<Daemon::Texture2D> m_Tilesheet;
	int32_t m_OutputSize = 256;
	bool m_RandomSeed = false;
	int32_t m_Seed = 1;
	float* m_NoiseSeed1D = nullptr;
	float* m_PerlinNoise1D = nullptr;
	int m_OctaveCount = 1;
	float m_ScalingBias = 2.0f;
};
