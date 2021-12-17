#include "PlatformerGame.h"
#include <box2d/box2d.h>
#include <imgui/imgui.h>

extern bool g_PhysicsDebugDraw;

class PlayerController : public Daemon::ScriptableEntity
{
public:
	virtual void OnCreate() override
	{
	}
	virtual void OnDestroy() override
	{

	}
	virtual void OnUpdate(float deltaTime) override
	{
		using namespace Daemon;
		Rigidbody2DComponent& rigidbody = GetComponent<Rigidbody2DComponent>();
		b2Body* body = reinterpret_cast<b2Body*>(rigidbody.RuntimeBody);

		m_OnGround = body->GetContactList();
		if (m_OnGround)
		{
			if ((Input::IsKeyPressed(Key::W) || Input::IsKeyPressed(Key::Space)))
			{
				body->ApplyLinearImpulse({ 0.0f, 1.0f }, body->GetPosition(), true);
			}
		}

		if (Input::IsKeyPressed(Key::A))
		{
			body->ApplyForce({ -(1.0f * m_Force), 0.0f }, body->GetPosition(), true);
		}
		else if (Input::IsKeyPressed(Key::D))
		{
			body->ApplyForce({ 1.0f * m_Force, 0.0f }, body->GetPosition(), true);
		}
	}
private:
	float m_Force = 10.0f;
	float m_JumpForce = 100.0f;
	bool m_OnGround = false;
};

class CameraController : public Daemon::ScriptableEntity
{
public:
	virtual void OnCreate() override
	{
	}
	virtual void OnDestroy() override
	{

	}
	virtual void OnUpdate(float deltaTime) override
	{
		using namespace Daemon;
		TransformComponent& transform = GetComponent<TransformComponent>();

		if (Input::IsKeyPressed(Key::A))
		{
			transform.Position -= glm::vec3(1.0f * m_Speed * deltaTime, 0.0f, 0.0f);
		}
		else if (Input::IsKeyPressed(Key::D))
		{
			transform.Position += glm::vec3(1.0f * m_Speed * deltaTime, 0.0f, 0.0f);
		}

		if (Input::IsKeyPressed(Key::W))
		{
			transform.Position += glm::vec3(0.0f, 1.0f * m_Speed * deltaTime, 0.0f);
		}
		else if (Input::IsKeyPressed(Key::S))
		{
			transform.Position -= glm::vec3(0.0f, 1.0f * m_Speed * deltaTime, 0.0f);
		}
	}
private:
	float m_Speed = 15.0f;
};

void PlatformerGame::OnAttach()
{
	Daemon::Entity camera = m_Scene.CreateEntity("Camera");
	Daemon::CameraComponent& cameraComponent = camera.AddComponent<Daemon::CameraComponent>();
	cameraComponent.Camera.SetOrthographicSize(50.0f);
	camera.AddComponent<Daemon::NativeScriptComponent>().Bind<CameraController>();

	m_Tilesheet = Daemon::Texture2D::Create("Assets/Textures/Platformer/Basic/tilesheet@2.png");

	//Daemon::Entity player = m_Scene.CreateEntity("Player");
	//Daemon::QuadData* playerData = new Daemon::QuadData();
	//playerData->SubSprite = Daemon::SubTexture2D::CreateFromCoords(tilesheet, { 11, 6 }, { 128, 128 });
	//player.AddComponent<Daemon::SpriteRendererComponent>(playerData);
	//player.AddComponent<Daemon::Circle2DColliderComponent>();
	//player.AddComponent<Daemon::Rigidbody2DComponent>(Daemon::Rigidbody2DComponent::Type::Dynamic);
	//player.AddComponent<Daemon::NativeScriptComponent>().Bind<PlayerController>();

	m_OutputSize = 64;
	m_OctaveCount = 7;
	m_NoiseSeed1D = new float[m_OutputSize];
	m_PerlinNoise1D = new float[m_OutputSize];

	for (int32_t i = 0; i < m_OutputSize; i++)
	{
		m_NoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;
	}

	Regenerate(64);

	m_Scene.OnRuntimeStart();
}

void PlatformerGame::OnDetach()
{
	m_Scene.OnRuntimeStop();
}

void PlatformerGame::OnUpdate(float deltaTime)
{
	m_Scene.OnRuntimeUpdate(deltaTime);
}

void PlatformerGame::OnEvent(Daemon::Event& e)
{
}

void PlatformerGame::OnGUI()
{
	if (ImGui::Begin("Renderer2D"))
	{
		Daemon::Renderer2D::Statistics stats = Daemon::Renderer2D::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);

		ImGuiIO& io = ImGui::GetIO();
		Daemon::Application& app = Daemon::Application::Get();
		ImGui::Text("Framerate: %.1f", 1 / app.GetFrameTime());
		ImGui::End();
	}

	if (ImGui::Begin("Physics"))
	{
		ImGui::SliderInt("##Octaves", &m_OctaveCount, 1, 7, "%d Octaves");
		ImGui::SliderFloat("##Bias", &m_ScalingBias, 0.2f, 10.0f, "%f Bias");
		ImGui::Checkbox("Random Seed", &m_RandomSeed);
		ImGui::SliderInt("Seed", &m_Seed, 0, RAND_MAX);
		if (ImGui::Button("Regenerate Noise Map"))
		{
			Regenerate(128);
		}
		if (ImGui::Button("Clear"))
		{
			if (m_Entities.size() > 0)
			{
				for (Daemon::Entity entity : m_Entities)
				{
					m_Scene.DestroyEntity(entity);
				}
			}
		}

		ImGui::Checkbox("Physics Debug Renderer", &g_PhysicsDebugDraw);
		ImGui::End();
	}
}

void PlatformerGame::Regenerate(int32_t height)
{
	if (m_Entities.size() > 0)
	{
		for (Daemon::Entity entity : m_Entities)
		{
			m_Scene.DestroyEntity(entity);
		}
	}

	if (m_RandomSeed)
	{
		m_Seed = rand();
	}
	Daemon::Math::PerlineNoise1D(m_OutputSize, m_Seed, m_OctaveCount, m_ScalingBias, m_PerlinNoise1D);
	for (int32_t x = 0; x < m_OutputSize; x++)
	{
		int y = -(m_PerlinNoise1D[x] * (float)height / 2.0f) + (float)height / 2.0f;
		if (y < height / 2)
		{
			for (int f = y; f < height / 2; f++)
			{
				Daemon::Entity entity = m_Scene.CreateEntity("entity");
				Daemon::TransformComponent& transform = entity.GetComponent<Daemon::TransformComponent>();
				transform.Position = { x - (m_OutputSize / 2.0f), -f, 0.0f };

				Daemon::QuadData* quad = new Daemon::QuadData();
				quad->SubSprite = Daemon::SubTexture2D::CreateFromCoords(m_Tilesheet, { 3, 6 }, { 128, 128 });
				Daemon::SpriteRendererComponent& spriteRenderer = entity.AddComponent<Daemon::SpriteRendererComponent>(quad);
				m_Entities.push_back(entity);
			}
		}
	}
}
