#include "EditorLayer.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <DaemonEngine/Scene/SceneSerializer.h>
#include <DaemonEngine/Utils/PlatformUtils.h>

#include <ImGuizmo/ImGuizmo.h>
#include <glm/glm/gtc/type_ptr.hpp>

extern bool g_PhysicsDebugDraw;
extern bool g_EnableSecondaryViewport = false;

namespace Daemon
{

    extern const std::filesystem::path g_AssetPath;

    extern float g_Padding;
    extern float g_ThumbnailSize;

	void EditorLayer::OnAttach()
	{
        // Setup scene and scene renderer
        m_Scene = CreateShared<Scene>();
        m_SceneRenderer = SceneRenderer::Create(m_Scene);

        Application& app = Application::Get();
        m_EditorCamera = EditorCamera(glm::radians(45.0f), 0.1f, 100.0f);

        Entity camera = m_Scene->CreateEntity("Camera");
        camera.AddComponent<CameraComponent>();

        Entity entity = m_Scene->CreateEntity("Entity");
        QuadData* quadData = new QuadData();
        quadData->Sprite = Texture2D::Create("Assets/Textures/container2.png");
        entity.AddComponent<SpriteRendererComponent>(quadData);

        FramebufferSpecification specification;
        specification.Width = app.GetWindow().GetWidth();
        specification.Height = app.GetWindow().GetHeight();
        specification.Attachments = { FramebufferTextureFormat::RGBA8 };
        specification.ClearColor = { 1.0f, 0.2f, 0.2f, 0.2f };
        m_GameViewport = Framebuffer::Create(specification);

        // Create panels
        m_SceneHierarchyPanel.SetSceneContext(m_Scene);

		m_Scene->OnRuntimeStart();
    }

	void EditorLayer::OnDetach()
	{
		m_Scene->OnRuntimeStop();
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
        m_EditorCamera.OnUpdate(deltaTime);

        m_SceneRenderer->BeginScene();
        m_SceneRenderer->m_Framebuffer->ClearAttachment(1, -1); // 1045220557
        //m_SceneRenderer->m_Framebuffer->ClearAttachment(1, 1045220557); 

        m_Scene->OnEditorUpdate(deltaTime, m_EditorCamera);

        // Make top-left corner (0,0)
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
        int mouseX = static_cast<int>(mx);
        int mouseY = static_cast<int>(my);

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            int pixelData = m_SceneRenderer->m_Framebuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixelData == 1045220557 || pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_Scene.get()); // Should be -1
        }
        m_SceneRenderer->EndScene();

        if (g_EnableSecondaryViewport)
        {
            m_GameViewport->Bind();
            m_GameViewport->Clear();
            m_Scene->OnRuntimeUpdate(deltaTime);
            m_GameViewport->Unbind();
        }
    }

    void EditorLayer::OnGUI()
    {
        // In 99% case you should be able to just call DockSpaceOverViewport() and ignore all the code below!
        // In this specific demo, we are not using DockSpaceOverViewport() because:
        // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
        // - we allow the host window to have padding (when opt_padding == true)
        // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
        // TL;DR; this demo is more complicated than what you would normally use.
        // If we removed all the options we are showcasing, this demo would become:
        //     void ShowExampleAppDockSpace()
        //     {
        //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        //     }

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        bool p_open = true;
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &p_open, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ShowTopMenu();

        ImGui::End();

        // Panels
        m_SceneHierarchyPanel.OnGUI();
        m_ContentBrowserPanel.OnGUI();

        if (g_EnableSecondaryViewport)
        {
            ShowGameViewport();
        }
        ShowEditorViewport();

        ImGui::Begin("Settings");
        {
            ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, "Viewport");
            std::string name = m_HoveredEntity ? m_HoveredEntity.GetComponent<TagComponent>().Tag : "None";
            ImGui::Text("| Hovered Entity: %s", name.c_str());
            ImGui::Spacing();

            ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, "Performance");
            float frameTime = Application::Get().GetFrameTime();
            float frameRate = frameTime * 100.0f;
            ImGui::Text("| Frame Time: %.2fms", frameRate);
            ImGui::Text("| Frames Per Second: %.2f FPS", 1.0f / frameTime);
            ImGui::Spacing();

            ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, "Memory");
            ImGui::Text("| Total Allocated: %d bytes", Daemon::MemoryManager::Metrics.TotalAllocatedMemory);
            ImGui::Text("| Total Freed: %d bytes", Daemon::MemoryManager::Metrics.TotalFreedMemory);
            ImGui::Text("| Current Memory Usage: %d bytes", Daemon::MemoryManager::Metrics.GetCurrentMemoryUsage());
            ImGui::Spacing();

            ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, "Physics");
            ImGui::Checkbox("Physics Debug Renderer", &g_PhysicsDebugDraw);
        }
        ImGui::End();

        ImGui::Begin("ImGui Settings");
        {
            ImGuiIO& io = ImGui::GetIO(); (void)io;

            ImGui::Columns(2);
            //{

            //    ImGui::Text("Texture");
            //    ImGui::NextColumn();
            //    float width = ImGui::GetContentRegionAvailWidth();
            //    ImGui::Button("", ImVec2(width, 0.0f));
            //    if (ImGui::BeginDragDropTarget())
            //    {
            //        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            //        {
            //            const wchar_t* path = reinterpret_cast<const wchar_t*>(payload->Data);
            //            std::filesystem::path assetPath = std::filesystem::path(g_AssetPath / path);
            //            std::string assetPathString = assetPath.string();

            //            io.FontDefault = io.Fonts->AddFontFromFileTTF(assetPathString.c_str(), io.FontDefault->FontSize);
            //        }
            //        ImGui::EndDragDropTarget();
            //    }

            //}
            //ImGui::NextColumn();

            {
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("Font Size");
                ImGui::NextColumn();
                ImGui::PushItemWidth(-1);
                ImGui::SliderFloat("", &io.FontDefault->FontSize, 12.0f, 32.0f);
            }
            ImGui::Columns(1);

            {
                ImGui::PushID("namespace0");
                ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                ImGui::Separator();
                const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
                bool open = ImGui::TreeNodeEx("##dummy_id", treeNodeFlags, "Content Browser");
                bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
                ImGui::PopStyleVar();

                if (open)
                {
                    ImGui::Columns(2);
                    {
                        ImGui::SetColumnWidth(0, 100.0f);
                        ImGui::Text("Thumbnail Size");
                        ImGui::NextColumn();
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderFloat("##Thumbnail_Size", &g_ThumbnailSize, 16, 512);
                    }
                    ImGui::NextColumn();

                    {
                        ImGui::SetColumnWidth(0, 100.0f);
                        ImGui::Text("Padding");
                        ImGui::NextColumn();
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderFloat("##Padding", &g_Padding, 0, 32);
                    }

                    ImGui::Columns(1);
                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
        }
        ImGui::End();

        ImGui::ShowDemoWindow();
    }

	void EditorLayer::OnEvent(Event& e)
	{
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(KE_BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
        dispatcher.Dispatch<MouseButtonPressedEvent>(KE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressedEvent));
    }

    void EditorLayer::NewScene()
    {
        m_HoveredEntity = {};

        m_Scene = CreateShared<Scene>();
        m_Scene->SetViewportSize(1600, 900);
        m_SceneHierarchyPanel.SetSceneContext(m_Scene);
    }

    void EditorLayer::OpenScene()
    {
        m_HoveredEntity = {};

        std::string filepath = FileDialogs::OpenFile("Kingo Scene (*.kingo)\0*.kingo\0");
        if (!filepath.empty())
        {
            OpenScene(filepath);
        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        m_HoveredEntity = {};

        m_Scene = CreateShared<Scene>();
        m_Scene->SetViewportSize(1600, 900);
        m_SceneHierarchyPanel.SetSceneContext(m_Scene);

        SceneSerializer serializer(m_Scene);
        serializer.Deserialize(path.string());
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("Kingo Scene (*.kingo)\0*.kingo\0");
        if (!filepath.empty())
        {
            SceneSerializer serializer(m_Scene);
            serializer.Serialize(filepath);
        }
    }

    void EditorLayer::ShowGameViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        bool viewportedOpened = ImGui::Begin("Game Viewport");
        auto viewportSize = ImGui::GetContentRegionAvail();
        m_Scene->SetViewportSize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
        m_GameViewport->Resize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
        //ImGui::Image(m_SceneRenderer->GetFinalImage(), viewportSize, { 0, 1 }, { 1, 0 });
        ImGui::Image(m_GameViewport->GetColorAttachment(0), viewportSize, { 0, 1 }, { 1, 0 });
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::ShowEditorViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        bool viewportedOpened = ImGui::Begin("Viewport");
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

        auto viewportSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize.x != viewportSize.x || m_ViewportSize.y != viewportSize.y)
        {
            uint32_t vpWidth = static_cast<uint32_t>(viewportSize.x);
            uint32_t vpHeight = static_cast<uint32_t>(viewportSize.y);

            m_Scene->SetViewportSize(vpWidth, vpHeight);
            m_EditorCamera.SetViewportSize(vpWidth, vpHeight);
            m_SceneRenderer->SetViewportSize(vpWidth, vpHeight);

            m_ViewportSize = { viewportSize.x, viewportSize.y };
        }

        ImGui::Image(m_SceneRenderer->GetFinalImage(), { m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1, 0 });

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const wchar_t* path = (const wchar_t*)payload->Data;
                OpenScene(std::filesystem::path(g_AssetPath / path));
            }
            ImGui::EndDragDropTarget();
        }

        // Gizmos
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_GizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            // ImGuizmo viewport
            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapValue = m_GizmoType != ImGuizmo::OPERATION::ROTATE ? 0.5f : 45.0f;
            float snapValues[3] = { snapValue, snapValue, snapValue };

            glm::mat4 cameraProjection = m_EditorCamera.GetProjection();
            if (RendererAPI::Current() != RendererAPIType::OpenGL)
            {
                Math::InvertYComponent(cameraProjection);
            }
            ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetView()), glm::value_ptr(cameraProjection),
                                 (ImGuizmo::OPERATION)m_GizmoType, 
                                 ImGuizmo::MODE::WORLD, 
                                 glm::value_ptr(transform), 
                                 nullptr, 
                                 snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 position, rotation, scale;
                Math::DecomposeTransform(transform, position, rotation, scale);

                glm::vec3 deltaRotation = rotation - tc.Rotation;
                tc.Position = position;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::ShowTopMenu()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                {
                    NewScene();
                }
                if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
                {
                    OpenScene();
                }
                if (ImGui::MenuItem("Save Scene"))
                {
                }
                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+O"))
                {
                    SaveSceneAs();
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Restart with"))
                {
                    if (ImGui::MenuItem("DirectX 11"))
                    {
                        RendererAPI::SetAPI(RendererAPIType::DirectX11);
                        Application::Get().Restart();
                    }
                    if (ImGui::MenuItem("DirectX 12"))
                    {
                        KE_CORE_WARN("DirectX 12 is not implemented!");
                    }
                    if (ImGui::MenuItem("OpenGL"))
                    {
                        RendererAPI::SetAPI(RendererAPIType::OpenGL);
                        Application::Get().Restart();
                    }
                    if (ImGui::MenuItem("Vulkan"))
                    {
                        KE_CORE_WARN("Vulkan is not implemented!");
                    }
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Close", "ALT+F4"))
                {
                    Application::Get().Close();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::BeginMenu("Experimental"))
                {
                    ImGui::MenuItem("Secondary Viewport", NULL, &g_EnableSecondaryViewport);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Objects"))
            {
                if (ImGui::MenuItem("Empty Object"))
                {
                    m_Scene->CreateEntity("Empty Object");
                }
                if (ImGui::BeginMenu("2D"))
                {
                    if (ImGui::MenuItem("Sprite"))
                    {
                        Entity entity = m_Scene->CreateEntity("Sprite");
                        QuadData* data = new QuadData();
                        entity.AddComponent<SpriteRendererComponent>(SpriteRendererComponent::PolygonType::Quad, data);
                    }
                    if (ImGui::MenuItem("Circle"))
                    {
                        Entity entity = m_Scene->CreateEntity("Circle");
                        CircleData* data = new CircleData();
                        entity.AddComponent<SpriteRendererComponent>(SpriteRendererComponent::PolygonType::Circle, data);
                    }
                    if (ImGui::MenuItem("Line"))
                    {
                        Entity entity = m_Scene->CreateEntity("Line");
                        LineData* data = new LineData();
                        entity.AddComponent<SpriteRendererComponent>(SpriteRendererComponent::PolygonType::Line, data);
                    }
                    if (ImGui::MenuItem("Rect"))
                    {
                        Entity entity = m_Scene->CreateEntity("Rect");
                        RectData* data = new RectData();
                        entity.AddComponent<SpriteRendererComponent>(SpriteRendererComponent::PolygonType::Rect, data);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    bool EditorLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == Mouse::ButtonLeft)
        {
            if (m_ViewportHovered && !ImGuizmo::IsOver())
            {
                m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
            }
        }
        return false;
    }

    bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
    {
        // Shortcuts
        if (e.GetRepeatCount() > 0)
        {
            return false;
        }

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

        switch (e.GetKeyCode())
        {
            case Key::N:
            {
                if (control)
                {
                    NewScene();
                }
            } break;
            case Key::O:
            {
                if (control)
                {
                    OpenScene();
                }
            } break;
            case Key::S:
            {
                if (control && shift)
                {
                    SaveSceneAs();
                }
            } break;

            case Key::Q: m_GizmoType = -1; break;
            case Key::W: m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; break;
            case Key::E: m_GizmoType = ImGuizmo::OPERATION::ROTATE; break;
            case Key::R: m_GizmoType = ImGuizmo::OPERATION::SCALE; break;
        }
        return false;
    }

}