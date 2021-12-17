#include "kepch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "ScriptableEntity.h"

#include "DaemonEngine/Core/Application.h"

#include "DaemonEngine/Renderer/Renderer.h"
#include "DaemonEngine/Renderer/Renderer2D.h"

#include <box2d/box2d.h>

extern bool g_PhysicsDebugDraw = false;

namespace Daemon
{

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		m_Registry.clear();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TagComponent>(name);
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (m_Registry.valid(entity))
		{
			m_Registry.destroy(entity);
		}
	}

	void Scene::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	void Scene::OnRuntimeStart()
	{
		// Native scripting 
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity{ entity, this };
				nsc.Instance->OnCreate();
			});
		}

		// Create all Box2D bodies
		{
			auto group = m_Registry.group<Rigidbody2DComponent>(entt::get<TransformComponent>);
			m_Physics2DBodyEntityBuffer = new Entity[group.size()];
			uint32_t physicsBodyEntityBufferIndex = 0;
			for (auto entity : group)
			{
				Entity e = { entity, this };
				auto [rigidBody2D, transform] = group.get<Rigidbody2DComponent, TransformComponent>(entity);

				b2BodyDef bodyDef;
				switch (rigidBody2D.BodyType)
				{
					case Rigidbody2DComponent::Type::Static:	bodyDef.type = b2_staticBody; break;
					case Rigidbody2DComponent::Type::Dynamic:	bodyDef.type = b2_dynamicBody; break;
					case Rigidbody2DComponent::Type::Kinematic: bodyDef.type = b2_kinematicBody; break;
					default: break;
				}
				bodyDef.position.Set(transform.Position.x, transform.Position.y);
				bodyDef.angle = transform.Rotation.z;
				bodyDef.angularDamping = rigidBody2D.AngularDamping;
				bodyDef.linearDamping = rigidBody2D.LinearDamping;

				b2Body* body = Physics2D::CreateBody(&bodyDef);
				Entity* entityStorage = &m_Physics2DBodyEntityBuffer[physicsBodyEntityBufferIndex++];
				*entityStorage = e;
				//body->SetUserData((void*)entityStorage);
				rigidBody2D.RuntimeBody = body;
			}
		}

		// Create all Box2D fixtures
		{
			// Circle colliders
			{
				auto group = m_Registry.group<Circle2DColliderComponent>(entt::get<TransformComponent>);
				for (auto entity : group)
				{
					Entity e = { entity, this };

					auto& rigidBody2D = e.GetComponent<Rigidbody2DComponent>();
					KE_CORE_ASSERT(rigidBody2D.RuntimeBody);
					b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);

					auto [circle2DCollider, transform] = group.get<Circle2DColliderComponent, TransformComponent>(entity);
					b2CircleShape circleShape;
					circleShape.m_p.Set(transform.Position.x, transform.Position.y);
					circleShape.m_radius = circle2DCollider.Radius;

					b2FixtureDef fixtureDef;
					fixtureDef.shape = &circleShape;
					fixtureDef.density = circle2DCollider.Density;
					fixtureDef.friction = circle2DCollider.Friction;
					circle2DCollider.RuntimeFixture = body->CreateFixture(&fixtureDef);
				}
			}
			// Box colliders
			{
				auto group = m_Registry.group<Box2DColliderComponent>(entt::get<TransformComponent>);
				for (auto entity : group)
				{
					Entity e = { entity, this };

					auto& rigidBody2D = e.GetComponent<Rigidbody2DComponent>();
					KE_CORE_ASSERT(rigidBody2D.RuntimeBody);
					b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);

					auto [box2DCollider, transform] = group.get<Box2DColliderComponent, TransformComponent>(entity);
					b2PolygonShape polygonShape;
					if (box2DCollider.UseTransformSize)
					{
						glm::vec2 transformSize = transform.Scale;
						polygonShape.SetAsBox(transformSize.x / 2.0f, transformSize.y / 2.0f);
					}
					else
					{
						polygonShape.SetAsBox(box2DCollider.Size.x, box2DCollider.Size.y);
					}

					b2FixtureDef fixtureDef;
					fixtureDef.shape = &polygonShape;
					fixtureDef.density = box2DCollider.Density;
					fixtureDef.friction = box2DCollider.Friction;
					box2DCollider.RuntimeFixture = body->CreateFixture(&fixtureDef);
				}
			}
		}
	}

	void Scene::OnRuntimeUpdate(float deltaTime)
	{
		// -------- Box2D -------- 
		{
			Physics2D::Step();
			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto entity : view)
			{
				Entity e = { entity, this };
				Rigidbody2DComponent& rb2d = e.GetComponent<Rigidbody2DComponent>();
				b2Body* body = static_cast<b2Body*>(rb2d.RuntimeBody);

				TransformComponent& transform = e.GetComponent<TransformComponent>();
				transform.Position.x = body->GetPosition().x;
				transform.Position.y = body->GetPosition().y;
				transform.Rotation.z = body->GetAngle();
			}
		}

		// -------- Native Scripting -------- 
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				nsc.Instance->OnUpdate(deltaTime);
			});
		}

		// -------- 2D Rendering -------- 
		{
			// Find main camera
			Camera* mainCamera = nullptr;
			glm::mat4 cameraTransform;
			{
				auto view = m_Registry.view<TransformComponent, CameraComponent>();
				for (auto entity : view)
				{
					auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

					if (camera.Primary)
					{
						mainCamera = &camera.Camera;
						cameraTransform = transform.GetTransform();
						break;
					}
				}
			}

			// Render
			Renderer::Clear(0);
			if (mainCamera)
			{
				Renderer2D::BeginScene(*mainCamera, cameraTransform);
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::DrawSprite(transform, sprite, (int)entity);
				}
				if (g_PhysicsDebugDraw)
				{
					Physics2D::DebugDraw();
				}
				Renderer2D::EndScene();
			}
		}
	}

	void Scene::OnEditorUpdate(float deltaTime, EditorCamera& editorCamera)
	{
		Renderer::Clear(0);
		Renderer2D::BeginScene(editorCamera);
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawSprite(transform, sprite, (int)entity);
		}
		if (g_PhysicsDebugDraw)
		{
			Physics2D::DebugDraw();
		}
		Renderer2D::EndScene();
	}

	void Scene::OnRuntimeStop()
	{
		m_Registry.clear();
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

#pragma region Components Added
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		//static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, Rigidbody2DComponent& component)
	{
		auto group = m_Registry.group<Rigidbody2DComponent>(entt::get<TransformComponent>);
		m_Physics2DBodyEntityBuffer = new Entity[group.size()];
		uint32_t physicsBodyEntityBufferIndex = group.size() - 1;

		auto& transform = entity.GetComponent<TransformComponent>();

		b2BodyDef bodyDef;
		switch (component.BodyType)
		{
			case Rigidbody2DComponent::Type::Static:	bodyDef.type = b2_staticBody; break;
			case Rigidbody2DComponent::Type::Dynamic:	bodyDef.type = b2_dynamicBody; break;
			case Rigidbody2DComponent::Type::Kinematic: bodyDef.type = b2_kinematicBody; break;
			default: break;
		}
		bodyDef.position.Set(transform.Position.x, transform.Position.y);
		bodyDef.angle = transform.Rotation.z;
		bodyDef.angularDamping = component.AngularDamping;
		bodyDef.linearDamping = component.LinearDamping;

		b2Body* body = Physics2D::CreateBody(&bodyDef);
		Entity* entityStorage = &m_Physics2DBodyEntityBuffer[physicsBodyEntityBufferIndex++];
		*entityStorage = entity;
		component.RuntimeBody = body;
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, Box2DColliderComponent& component)
	{
		auto& rigidBody2D = entity.GetComponent<Rigidbody2DComponent>();
		KE_CORE_ASSERT(rigidBody2D.RuntimeBody);
		b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);

		auto& transform = entity.GetComponent<TransformComponent>();
		auto& box2DCollider = entity.GetComponent<Box2DColliderComponent>();

		b2PolygonShape polygonShape;
		if (box2DCollider.UseTransformSize)
		{
			glm::vec2 transformSize = transform.Scale;
			polygonShape.SetAsBox(transformSize.x / 2.0f, transformSize.y / 2.0f);
		}
		else
		{
			polygonShape.SetAsBox(box2DCollider.Size.x, box2DCollider.Size.y);
		}

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &polygonShape;
		fixtureDef.density = box2DCollider.Density;
		fixtureDef.friction = box2DCollider.Friction;
		box2DCollider.RuntimeFixture = body->CreateFixture(&fixtureDef);
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, Circle2DColliderComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, SpriteRendererComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded(Entity entity, NativeScriptComponent& component)
	{
	}
#pragma endregion

#pragma region Component Removed
	template<typename T>
	void Scene::OnComponentRemoved(Entity entity, T& component)
	{
		//static_assert(false);
	}

	template<>
	void Scene::OnComponentRemoved(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved(Entity entity, Box2DColliderComponent& component)
	{
		auto& rigidBody2D = entity.GetComponent<Rigidbody2DComponent>();
		KE_CORE_ASSERT(rigidBody2D.RuntimeBody);
		b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);

		b2Fixture* fixture = static_cast<b2Fixture*>(component.RuntimeFixture);
		body->DestroyFixture(fixture);
	}

	template<>
	void Scene::OnComponentRemoved(Entity entity, Circle2DColliderComponent& component)
	{
		auto& rigidBody2D = entity.GetComponent<Rigidbody2DComponent>();
		KE_CORE_ASSERT(rigidBody2D.RuntimeBody);
		b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);

		b2Fixture* fixture = static_cast<b2Fixture*>(component.RuntimeFixture);
		body->DestroyFixture(fixture);
	}

	template<>
	void Scene::OnComponentRemoved(Entity entity, CameraComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved(Entity entity, NativeScriptComponent& component)
	{
	}
#pragma endregion

}