#include "kepch.h"
#include "SceneHierarchyPanel.h"
#include "DaemonEngine/Scene/Components.h"

#include "DaemonEngine/ImGui/ImGui.h"

namespace Daemon
{

	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Shared<Scene>& scene)
		: m_SceneContext(scene)
	{
	}

	void SceneHierarchyPanel::SetSceneContext(const Shared<Scene>& scene)
	{
		m_SceneContext = scene;
		m_SelectedEntity = {};
	}

	void SceneHierarchyPanel::OnGUI()
	{
		bool hierarchyPanelOpened = ImGui::Begin("Scene Hierarchy");
		ImRect windowRect = { ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax() };

		if (m_SceneContext)
		{
			for (auto entity : m_SceneContext->m_Registry.view<TagComponent>())
			{
				DrawEntityNode({ entity, m_SceneContext.get() });
			}

			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::MenuItem("Empty Entity"))
					{
						m_SceneContext->CreateEntity("Empty Entity");
					}
					if (ImGui::BeginMenu("2D"))
					{
						if (ImGui::MenuItem("Sprite"))
						{
							Entity entity = m_SceneContext->CreateEntity("Sprite");
							QuadData* data = new QuadData();
							entity.AddComponent<SpriteRendererComponent>(SpriteRendererComponent::PolygonType::Quad, data);
						}
						if (ImGui::MenuItem("Circle"))
						{
							Entity entity = m_SceneContext->CreateEntity("Circle");
							CircleData* data = new CircleData();
							entity.AddComponent<SpriteRendererComponent>(SpriteRendererComponent::PolygonType::Circle, data);
						}
						if (ImGui::MenuItem("Line"))
						{
							Entity entity = m_SceneContext->CreateEntity("Line");
							LineData* data = new LineData();
							entity.AddComponent<SpriteRendererComponent>(SpriteRendererComponent::PolygonType::Line, data);
						}
						if (ImGui::MenuItem("Rect"))
						{
							Entity entity = m_SceneContext->CreateEntity("Rect");
							RectData* data = new RectData();
							entity.AddComponent<SpriteRendererComponent>(SpriteRendererComponent::PolygonType::Rect, data);
						}
						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}
				ImGui::EndPopup();
			}
		}

		if (hierarchyPanelOpened)
		{
			ImGui::End();
		}

		bool propertiesPanelOpened = ImGui::Begin("Properties");
		if (m_SelectedEntity)
		{
			DrawComponents(m_SelectedEntity);
		}
		if (propertiesPanelOpened)
		{
			ImGui::End();
		}
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		const char* name = "Unnamed Entity";
		if (entity.HasComponent<TagComponent>())
			name = entity.GetComponent<TagComponent>().Tag.c_str();

		ImGuiTreeNodeFlags flags = (entity == m_SelectedEntity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		const bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name);
		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		// Defer deletion until end of node UI
		if (entityDeleted)
		{
			m_SceneContext->DestroyEntity(entity);
			if (entity == m_SelectedEntity)
				m_SelectedEntity = {};

		}
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool canBeRemoved = true)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			ImGui::PushID((void*)typeid(T).hash_code());
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx("##dummy_id", treeNodeFlags, name.c_str());
			bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
			ImGui::PopStyleVar();

			bool resetValues = false;
			bool removeComponent = false;

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }) || right_clicked)
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Reset"))
					resetValues = true;

				if (canBeRemoved)
				{
					if (ImGui::MenuItem("Remove component"))
						removeComponent = true;
				}

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent || resetValues)
				entity.RemoveComponent<T>();

			if (resetValues)
				entity.AddComponent<T>();

			ImGui::PopID();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		ImGui::AlignTextToFramePadding();

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, 256);
			memcpy(buffer, tag.c_str(), tag.length());
			ImGui::PushItemWidth(contentRegionAvailable.x * 0.5f);
			if (ImGui::InputText("##Tag", buffer, 256))
			{
				tag = std::string(buffer);
			}
			ImGui::PopItemWidth();
		}

		ImVec2 textSize = ImGui::CalcTextSize("Add Component");
		ImGui::SameLine(contentRegionAvailable.x - (textSize.x + GImGui->Style.FramePadding.y));
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponentPanel");

		if (ImGui::BeginPopup("AddComponentPanel"))
		{
			if (!m_SelectedEntity.HasComponent<CameraComponent>())
			{
				if (ImGui::Button("Camera"))
				{
					m_SelectedEntity.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::Button("Sprite Renderer"))
				{
					QuadData* data = new QuadData();
					m_SelectedEntity.AddComponent<SpriteRendererComponent>(data);
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectedEntity.HasComponent<Rigidbody2DComponent>())
			{
				if (ImGui::Button("Rigidbody 2D"))
				{
					m_SelectedEntity.AddComponent<Rigidbody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectedEntity.HasComponent<Box2DColliderComponent>())
			{
				if (ImGui::Button("Box Collider 2D"))
				{
					m_SelectedEntity.AddComponent<Box2DColliderComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectedEntity.HasComponent<Circle2DColliderComponent>())
			{
				if (ImGui::Button("Circle Collider 2D"))
				{
					m_SelectedEntity.AddComponent<Circle2DColliderComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}

		DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
		{
			UI::BeginPropertyGrid();
			UI::Property("Position", component.Position);

			glm::vec3 rotation = glm::degrees(component.Rotation);
			UI::Property("Rotation", rotation);
			component.Rotation = glm::radians(rotation);

			UI::Property("Scale", component.Scale);
			UI::EndPropertyGrid();
		}, false);

		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cc)
		{
			UI::BeginPropertyGrid();
			// Projection Type
			const char* projTypeStrings[] = { "Perspective", "Orthographic" };
			int currentProj = (int)cc.Camera.GetProjectionType();
			if (UI::PropertyDropdown("Projection", projTypeStrings, 2, &currentProj))
			{
				cc.Camera.SetProjectionType((SceneCamera::ProjectionType)currentProj);
			}

			float size = cc.Camera.GetOrthographicSize();
			if (UI::Property("Orthographic Size", size))
			{
				cc.Camera.SetOrthographicSize(size);
			}
			UI::EndPropertyGrid();
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& mc)
		{
			UI::BeginPropertyGrid();
			const char* polygonTypes[] = { "Quad", "Line", "Circle", "Rect" };
			int32_t currentPolygonType = (int32_t)mc.Type;
			if (UI::PropertyDropdown("Type", polygonTypes, 4, &currentPolygonType))
			{
				mc.Type = (SpriteRendererComponent::PolygonType)currentPolygonType;
			}
			switch (mc.Type)
			{
				case SpriteRendererComponent::PolygonType::Quad:
				{
					QuadData* data = reinterpret_cast<QuadData*>(mc.Data);
					UI::PropertyColor("Color", data->Color);

					static bool flipTexture = false;
					ImGui::Text("Flip");
					ImGui::NextColumn();
					ImGui::Checkbox("##fliptexture", &flipTexture);
					ImGui::NextColumn();

					ImGui::Text("Texture");
					ImGui::NextColumn();
					ImGui::Button("", ImVec2(100.0f, 0.0f));
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							if (data->Sprite)
							{
								KE_CORE_INFO("Changing texture for sprite - Deleting old one...");
								data->Sprite.reset();
								data->Sprite = nullptr;
							}

							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
							TextureSpecification textureSpecification;
							textureSpecification.FlipVertically = flipTexture;
							data->Sprite = Texture2D::Create(texturePath.string(), textureSpecification);
						}
						ImGui::EndDragDropTarget();
					}
				} break;
				case SpriteRendererComponent::PolygonType::Line:
				{
					LineData* data = reinterpret_cast<LineData*>(mc.Data);
					UI::PropertyColor("Color", data->Color);
					UI::Property("Start", data->StartPosition);
					UI::Property("End", data->EndPosition);
				} break;
				case SpriteRendererComponent::PolygonType::Circle:
				{
					CircleData* data = reinterpret_cast<CircleData*>(mc.Data);
					UI::PropertyColor("Color", data->Color);
					UI::Property("Radius", data->Radius);
				} break;
				case SpriteRendererComponent::PolygonType::Rect:
				{
					RectData* data = reinterpret_cast<RectData*>(mc.Data);
					UI::PropertyColor("Color", data->Color);
					UI::Property("Size", data->Size);
				} break;
				default: KE_CORE_ASSERT(false, "Unknown Polygon Type"); break;
			}
			UI::EndPropertyGrid();
		});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](Rigidbody2DComponent& rb2dc)
		{
#if 0
			UI::BeginPropertyGrid();

			// Rigidbody2D Type
			const char* rb2dTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			UI::PropertyDropdown("Type", rb2dTypeStrings, 3, (int*)&rb2dc.BodyType);

			if (rb2dc.BodyType == RigidBody2DComponent::Type::Dynamic)
			{
				UI::BeginPropertyGrid();
				UI::Property("Fixed Rotation", rb2dc.FixedRotation);
				UI::EndPropertyGrid();
			}

			UI::EndPropertyGrid();
#endif
		});

		DrawComponent<Box2DColliderComponent>("Box Collider 2D", entity, [](Box2DColliderComponent& bc2dc)
		{
			//UI::BeginPropertyGrid();

			//UI::Property("Offset", bc2dc.Offset);
			//UI::Property("Size", bc2dc.Size);
			//UI::Property("Density", bc2dc.Density);
			//UI::Property("Friction", bc2dc.Friction);

			//UI::EndPropertyGrid();
		});

		DrawComponent<Circle2DColliderComponent>("Circle Collider 2D", entity, [](Circle2DColliderComponent& cc2dc)
		{
			//UI::BeginPropertyGrid();

			//UI::Property("Offset", cc2dc.Offset);
			//UI::Property("Radius", cc2dc.Radius);
			//UI::Property("Density", cc2dc.Density);
			//UI::Property("Friction", cc2dc.Friction);

			//UI::EndPropertyGrid();
		});

	}

}