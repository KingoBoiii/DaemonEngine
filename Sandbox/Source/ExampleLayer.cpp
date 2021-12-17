#include "ExampleLayer.h"
#include <imgui/imgui.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>

void ExampleLayer::OnAttach()
{
	KE_TRACE("ExampleLayer::OnAttach()");

	Daemon::Application& app = Daemon::Application::Get();
	float aspectRatio = (float)app.GetWindow().GetWidth() / (float)app.GetWindow().GetHeight();
	m_CameraController.Resize(aspectRatio);
	m_CameraController.SetZoomLevel(10.0f);

	m_Texture = Daemon::Texture2D::Create("Assets/Textures/Checkerboard.png");
	m_SpaceRocket = Daemon::Texture2D::Create("Assets/Textures/cartoon-rocket.png");

	// setting up ground body/box
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	m_GroundBody = m_World.CreateBody(&groundBodyDef);
	m_GroundBox.SetAsBox(25.0f, 0.5f); // Needs to be HALF the width and HALF the height (Renderer size { 1.0f, 1.0f }, Box2D size { 0.5f, 0.5f })
	m_GroundBody->CreateFixture(&m_GroundBox, 0.0f);

	// setting up dynamic body/box
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 5.0f);
	m_Body = m_World.CreateBody(&bodyDef);
	m_BodyBox.SetAsBox(0.5f, 0.5f); // Needs to be HALF the width and HALF the height (Renderer size { 1.0f, 1.0f }, Box2D size { 0.5f, 0.5f })

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &m_BodyBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	m_Body->CreateFixture(&fixtureDef);

	// Test body!
	b2BodyDef bodyDef1;
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(25.2f, 5.0f);
	m_Body1 = m_World.CreateBody(&bodyDef1);
	m_BodyBox1.SetAsBox(0.5f, 0.5f); // Needs to be HALF the width and HALF the height (Renderer size { 1.0f, 1.0f }, Box2D size { 0.5f, 0.5f })

	b2FixtureDef fixtureDef1;
	fixtureDef1.shape = &m_BodyBox1;
	fixtureDef1.density = 1.0f;
	fixtureDef1.friction = 0.3f;
	m_Body1->CreateFixture(&fixtureDef1);
}

void ExampleLayer::OnDetach()
{
	KE_TRACE("ExampleLayer::OnDetach()");
}

void ExampleLayer::OnUpdate(float deltaTime)
{
	static float rotation = 90.0f;
	constexpr float rotationSpeed = 50.0f;
	if (Daemon::Input::IsKeyPressed(Daemon::Key::Left))
	{
		rotation += rotationSpeed * deltaTime;
	}
	else if (Daemon::Input::IsKeyPressed(Daemon::Key::Right))
	{
		rotation -= rotationSpeed * deltaTime;
	}

	m_CameraController.OnUpdate(deltaTime);

	constexpr float timeStep = 1.0f / 60.0f;
	constexpr int32 velocityIterations = 6;
	constexpr int32 positionIterations = 2;
	m_World.Step(timeStep, velocityIterations, positionIterations);

	Daemon::Renderer::Clear(0);
	Daemon::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Daemon::Renderer2D::DrawQuad({ m_GroundBody->GetPosition().x, m_GroundBody->GetPosition().y }, { 50.0f, 1.0f }, { 0.35f, 0.35f, 0.35f, 0.35f });
	Daemon::Renderer2D::DrawQuad({ m_Body->GetPosition().x, m_Body->GetPosition().y }, { 1.0f, 1.0f }, m_Texture);
	Daemon::Renderer2D::DrawQuad({ m_Body1->GetPosition().x, m_Body1->GetPosition().y }, { 1.0f, 1.0f }, m_TextColor);

	Daemon::Renderer2D::DrawRotatedQuad({ -1.0f, -1.0f }, 45.0f, { 1.0f, 1.0f }, m_Texture, { 0.3f, 0.8f, 1.0f, 1.0f });
	Daemon::Renderer2D::DrawRotatedQuad({ 0.0f, -0.5f }, rotation, { 0.75f, 0.4f }, m_SpaceRocket);
	Daemon::Renderer2D::EndScene();
}

void ExampleLayer::OnEvent(Daemon::Event& e)
{
	m_CameraController.OnEvent(e);
}

void ExampleLayer::OnGUI()
{
	if (ImGui::Begin("Renderer2D"))
	{
		Daemon::Renderer2D::Statistics stats = Daemon::Renderer2D::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);

		ImGuiIO& io = ImGui::GetIO();
		Daemon::Application& app = Daemon::Application::Get();
		ImGui::Text("Framerate: %.1f", 1 / app.GetFrameTime());
		static bool checked = app.GetWindow().GetVSync();
		if (ImGui::Checkbox("VSync", &checked))
		{
			app.GetWindow().SetVSync(checked);
		}

		ImGui::ColorPicker4("Text Color", &m_TextColor.x);
		ImGui::End();
	}

	if (ImGui::Begin("Memory"))
	{
		ImGui::Text("Total Allocated: %d bytes", Daemon::MemoryManager::Metrics.TotalAllocatedMemory);
		ImGui::Text("Total Freed: %d bytes", Daemon::MemoryManager::Metrics.TotalFreedMemory);
		ImGui::Text("Current Memory Usage: %d bytes", Daemon::MemoryManager::Metrics.GetCurrentMemoryUsage());
		ImGui::End();
	}
}