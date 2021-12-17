#pragma once
#include <DaemonEngine.h>
#include <box2d/box2d.h>

class ExampleLayer : public Daemon::Layer
{
public:
	ExampleLayer()
		: Layer("Example Layer"), m_CameraController(1.0f), m_World(m_Gravity)
	{
	}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Daemon::Event& e) override;
	virtual void OnGUI() override;
private:
	Daemon::OrthographicCameraController m_CameraController;
	Daemon::Shared<Daemon::Framebuffer> m_Framebuffer;
	Daemon::Shared<Daemon::Texture2D> m_Texture;
	Daemon::Shared<Daemon::Texture2D> m_SpaceRocket;
	glm::vec3 m_SpaceRocketPosition = { 0.0f, -0.5f, 0.0f };
	glm::vec4 m_TextColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Box2D - Getting started
	// Creating world
	b2Vec2 m_Gravity = b2Vec2(0.0f, -10.0f);
	b2World m_World;

	// Creating a Ground Box
	b2Body* m_GroundBody;
	b2PolygonShape m_GroundBox;

	// Creating a Dynamic Body
	b2Body* m_Body;
	b2PolygonShape m_BodyBox;

	b2Body* m_Body1;
	b2PolygonShape m_BodyBox1;
};

