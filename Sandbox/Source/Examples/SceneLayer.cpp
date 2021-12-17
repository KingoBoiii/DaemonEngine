#include "SceneLayer.h"
#include <imgui/imgui.h>

#include <box2d/box2d.h>

float g_RocketFuel = 100.0f;

class TestEntityScript : public Daemon::ScriptableEntity
{
public:
	virtual void OnCreate() override
	{
		using namespace Daemon;
		TransformComponent& transform = GetComponent<TransformComponent>();
		transform.Scale = { 2.75f, 1.75f, 1.0f };
		transform.Rotation.z = Math::DegreesToRadians(90.0f);

		m_SpaceRocket = Texture2D::Create("Assets/Textures/cartoon-rocket.png");
		QuadData* quadData = new QuadData();
		quadData->Sprite = m_SpaceRocket;
		SpriteRendererComponent& sr = AddComponent<SpriteRendererComponent>();
		sr.Type = SpriteRendererComponent::PolygonType::Quad;
		sr.Data = quadData;
	}

	virtual void OnDestroy() override
	{
	}

	virtual void OnUpdate(float deltaTime) override
	{
		using namespace Daemon;
		if (!m_RigidbodyInitialized)
		{
			m_Rigidbody = GetComponent<Rigidbody2DComponent>();
			m_RigidbodyInitialized = true;
		}

		b2Body* body = (b2Body*)m_Rigidbody.RuntimeBody;
		if (Input::IsKeyPressed(Key::Space))
		{
			if (g_RocketFuel > 0.5f)
			{
				
				body->ApplyLinearImpulse({ body->GetTransform().q.c, body->GetTransform().q.s * m_RocketForceThrust }, body->GetPosition(), true);
				g_RocketFuel -= 35.0f * deltaTime;
			}
		}

		m_RocketAngle = body->GetAngle();
		if (Input::IsKeyPressed(Key::A))
		{
			m_RocketAngle += 1.0f * deltaTime;
			body->SetTransform(body->GetPosition(), m_RocketAngle);
		}
		else if (Input::IsKeyPressed(Key::D))
		{
			m_RocketAngle -= 1.0f * deltaTime;
			body->SetTransform(body->GetPosition(), m_RocketAngle);
		}

		if (body->GetContactList())
		{
			g_RocketFuel += 15.0f * deltaTime;
			if (g_RocketFuel >= 100.0f)
			{
				g_RocketFuel = 100.0f;
			}
		}
		g_RocketFuel = std::max(g_RocketFuel, 0.0f);
	}
private:
	Daemon::Rigidbody2DComponent m_Rigidbody;
	bool m_RigidbodyInitialized = false;

	float m_RocketForceThrust = 2.0f;
	float m_RocketAngle = 0.0f;
	Daemon::Shared<Daemon::Texture2D> m_SpaceRocket;
};

void SceneLayer::OnAttach()
{
	KE_TRACE("ExampleLayer::OnAttach()");

	std::string test = "Hello, world!123";
	std::string test_cap = std::string(test.begin(), test.end());
	std::transform(test.begin(), test.end(), test_cap.begin(), ::toupper);

	Daemon::Entity camera = m_Scene.CreateEntity("Camera");
	camera.AddComponent<Daemon::CameraComponent>();

#if 0
	{	// Create crosshair
		Daemon::Entity circle = m_Scene.CreateEntity("CircleEntity");
		Daemon::CircleData* circleData = new Daemon::CircleData();
		circleData->Color = { 1.0f, 0.0f, 0.0f, 1.0f };
		circle.AddComponent<Daemon::SpriteRendererComponent>(Daemon::SpriteRendererComponent::PolygonType::Circle, circleData);

		Daemon::Entity horizontalLine = m_Scene.CreateEntity("HorizontalLine");
		Daemon::LineData* lineData1 = new Daemon::LineData();
		lineData1->StartPosition = { -1.0f, 0.0f };
		lineData1->EndPosition = { 1.0f, 0.0f };
		lineData1->Color = { 0.0f, 1.0f, 0.0f, 1.0f };
		horizontalLine.AddComponent<Daemon::SpriteRendererComponent>(Daemon::SpriteRendererComponent::PolygonType::Line, lineData1);

		Daemon::Entity verticalLine = m_Scene.CreateEntity("VerticalLine");
		Daemon::LineData* lineData2 = new Daemon::LineData();
		lineData2->StartPosition = { 0.0f, -1.0f };
		lineData2->EndPosition = { 0.0f, 1.0f };
		lineData2->Color = { 0.0f, 1.0f, 0.0f, 1.0f };
		verticalLine.AddComponent<Daemon::SpriteRendererComponent>(Daemon::SpriteRendererComponent::PolygonType::Line, lineData2);
	}
#endif

	Daemon::Entity entity = m_Scene.CreateEntity("DynamicBody");
	entity.AddComponent<Daemon::Box2DColliderComponent>();
	entity.AddComponent<Daemon::Rigidbody2DComponent>(Daemon::Rigidbody2DComponent::Type::Dynamic);
	entity.AddComponent<Daemon::NativeScriptComponent>().Bind<TestEntityScript>();

	Daemon::Entity ground = m_Scene.CreateEntity("GroundBody");
	Daemon::TransformComponent& groundTransform = ground.GetComponent<Daemon::TransformComponent>();
	groundTransform.Position = { 0.0f, -10.0f, 0.0f };
	groundTransform.Scale = { 35.0f, 1.0f, 0.0f };

	Daemon::QuadData* groundData = new Daemon::QuadData();
	groundData->Color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	ground.AddComponent<Daemon::SpriteRendererComponent>(groundData);
	ground.AddComponent<Daemon::Box2DColliderComponent>();
	ground.AddComponent<Daemon::Rigidbody2DComponent>();

	{	// Create Texture Atlas sprite
		Daemon::Shared<Daemon::Texture2D> textureAtlast = Daemon::Texture2D::Create("Assets/Textures/TopDown/tiles_packed.png");
		Daemon::Entity beachNW = m_Scene.CreateEntity("beach_NW");
		Daemon::QuadData* beachNWData = new Daemon::QuadData();
		beachNWData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(textureAtlast, { 1, 6 }, { 16, 16 });
		beachNW.AddComponent<Daemon::SpriteRendererComponent>(beachNWData);
		Daemon::TransformComponent& beachNWTransform = beachNW.GetComponent<Daemon::TransformComponent>();
		beachNWTransform.Position = { -2.0f, 2.0f, 0.0f };
		beachNWTransform.Scale = { 2.0f, 2.0f, 1.0f };

		Daemon::Entity beachN = m_Scene.CreateEntity("beach_N");
		Daemon::QuadData* beachNData = new Daemon::QuadData();
		beachNData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(textureAtlast, { 2, 6 }, { 16, 16 });
		beachN.AddComponent<Daemon::SpriteRendererComponent>(beachNData);
		Daemon::TransformComponent& beachNTransform = beachN.GetComponent<Daemon::TransformComponent>();
		beachNTransform.Position = { 0.0f, 2.0f, 0.0f };
		beachNTransform.Scale = { 2.0f, 2.0f, 1.0f };

		Daemon::Entity beachNE = m_Scene.CreateEntity("beach_NE");
		Daemon::QuadData* beachNEData = new Daemon::QuadData();
		beachNEData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(textureAtlast, { 3, 6 }, { 16, 16 });
		beachNE.AddComponent<Daemon::SpriteRendererComponent>(beachNEData);
		Daemon::TransformComponent& beachNETransform = beachNE.GetComponent<Daemon::TransformComponent>();
		beachNETransform.Position = { 2.0f, 2.0f, 0.0f };
		beachNETransform.Scale = { 2.0f, 2.0f, 1.0f };

		Daemon::Entity beachMW = m_Scene.CreateEntity("beach_MW");
		Daemon::QuadData* beachMWData = new Daemon::QuadData();
		beachMWData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(textureAtlast, { 1, 5 }, { 16, 16 });
		beachMW.AddComponent<Daemon::SpriteRendererComponent>(beachMWData);
		Daemon::TransformComponent& beachMWTransform = beachMW.GetComponent<Daemon::TransformComponent>();
		beachMWTransform.Position = { -2.0f, 0.0f, 0.0f };
		beachMWTransform.Scale = { 2.0f, 2.0f, 1.0f };

		Daemon::Entity beachM = m_Scene.CreateEntity("beach_M");
		Daemon::QuadData* beachMData = new Daemon::QuadData();
		beachMData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(textureAtlast, { 2, 5 }, { 16, 16 });
		beachM.AddComponent<Daemon::SpriteRendererComponent>(beachMData);
		Daemon::TransformComponent& beachMTransform = beachM.GetComponent<Daemon::TransformComponent>();
		beachMTransform.Position = { 0.0f, 0.0f, 0.0f };
		beachMTransform.Scale = { 2.0f, 2.0f, 1.0f };

		Daemon::Entity beachME = m_Scene.CreateEntity("beach_ME");
		Daemon::QuadData* beachMEData = new Daemon::QuadData();
		beachMEData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(textureAtlast, { 3, 5 }, { 16, 16 });
		beachME.AddComponent<Daemon::SpriteRendererComponent>(beachMEData);
		Daemon::TransformComponent& beachMETransform = beachME.GetComponent<Daemon::TransformComponent>();
		beachMETransform.Position = { 2.0f, 0.0f, 0.0f };
		beachMETransform.Scale = { 2.0f, 2.0f, 1.0f };

		Daemon::Entity beachSW = m_Scene.CreateEntity("beach_SW");
		Daemon::QuadData* beachSWData = new Daemon::QuadData();
		beachSWData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(textureAtlast, { 1, 4 }, { 16, 16 });
		beachSW.AddComponent<Daemon::SpriteRendererComponent>(beachSWData);
		Daemon::TransformComponent& beachSWTransform = beachSW.GetComponent<Daemon::TransformComponent>();
		beachSWTransform.Position = { -2.0f, -2.0f, 0.0f };
		beachSWTransform.Scale = { 2.0f, 2.0f, 1.0f };

		Daemon::Entity beachS = m_Scene.CreateEntity("beach_S");
		Daemon::QuadData* beachSData = new Daemon::QuadData();
		beachSData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(textureAtlast, { 2, 4 }, { 16, 16 });
		beachS.AddComponent<Daemon::SpriteRendererComponent>(beachSData);
		Daemon::TransformComponent& beachSTransform = beachS.GetComponent<Daemon::TransformComponent>();
		beachSTransform.Position = { 0.0f, -2.0f, 0.0f };
		beachSTransform.Scale = { 2.0f, 2.0f, 1.0f };

		Daemon::Entity beachSE = m_Scene.CreateEntity("beach_SE");
		Daemon::QuadData* beachSEData = new Daemon::QuadData();
		beachSEData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(textureAtlast, { 3, 4 }, { 16, 16 });
		beachSE.AddComponent<Daemon::SpriteRendererComponent>(beachSEData);
		Daemon::TransformComponent& beachSETransform = beachSE.GetComponent<Daemon::TransformComponent>();
		beachSETransform.Position = { 2.0f, -2.0f, 0.0f };
		beachSETransform.Scale = { 2.0f, 2.0f, 1.0f };
	}

	m_Scene.OnRuntimeStart();
}

void SceneLayer::OnDetach()
{
	m_Scene.OnRuntimeStop();

	KE_TRACE("ExampleLayer::OnDetach()");
}

void SceneLayer::OnUpdate(float deltaTime)
{
	m_Scene.OnRuntimeUpdate(deltaTime);
}

void SceneLayer::OnEvent(Daemon::Event& e)
{
	Daemon::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Daemon::WindowResizeEvent>(KE_BIND_EVENT_FN(SceneLayer::OnWindowResize));
}

void SceneLayer::OnGUI()
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

		ImGui::Text("%.2f", g_RocketFuel);
		ImGui::End();
	}

	if (ImGui::Begin("Memory"))
	{
		char sep = std::use_facet<std::numpunct<char>>(std::cout.getloc()).thousands_sep();

		ImGui::Text("Total Allocated: %d bytes", Daemon::MemoryManager::Metrics.TotalAllocatedMemory);
		ImGui::Text("Total Freed: %d bytes", Daemon::MemoryManager::Metrics.TotalFreedMemory);
		ImGui::Text("Current Memory Usage:", Daemon::MemoryManager::Metrics.GetCurrentMemoryUsage());
		ImGui::Text("| %d Bytes", Daemon::MemoryManager::Metrics.GetCurrentMemoryUsage());
		ImGui::Text("| %d kB", (Daemon::MemoryManager::Metrics.GetCurrentMemoryUsage() / 1024));
		ImGui::Text("| %d MB", (Daemon::MemoryManager::Metrics.GetCurrentMemoryUsage() / 1024 / 1024));
		ImGui::Text("| %d GB", (Daemon::MemoryManager::Metrics.GetCurrentMemoryUsage() / 1024 / 1024 / 1024));
		ImGui::End();
	}
}

bool SceneLayer::OnWindowResize(Daemon::WindowResizeEvent& e)
{
	m_Scene.SetViewportSize(e.GetWidth(), e.GetHeight());
	return false;
}
