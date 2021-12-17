#include "kepch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Daemon
{

	extern const std::filesystem::path g_AssetPath = "Assets";

	extern float g_Padding = 16.0f;
	extern float g_ThumbnailSize = 96.0f;

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(g_AssetPath)
	{
		m_BackButtonIcon = Texture2D::Create("Assets/Textures/UI/arrowleft.png");

		m_FileIcon = Texture2D::Create("Assets/Textures/UI/save.png");
		m_DirectoryIcon = Texture2D::Create("Assets/Textures/UI/video.png");
	}

	void ContentBrowserPanel::OnGUI()
	{
		ImGui::Begin("Content Browser");

		if (ImGui::BeginChild("##cb_top_bar", { 0, 24 }, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			if (ImGui::ImageButton(m_BackButtonIcon->GetTexture(), { 20, 20 }))
			{
				if (m_CurrentDirectory != g_AssetPath)
				{
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
				}
			}

			ImGui::SameLine();

			std::string currentAssetPath = m_CurrentDirectory.relative_path().string();
			ImGui::Text("%s", currentAssetPath.c_str());

			ImGui::EndChild();
		}

		ImGui::Separator();

		float cellSize = g_ThumbnailSize + g_Padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
		{
			columnCount = 1;
		}
		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			Shared<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton(icon->GetTexture(), { g_ThumbnailSize, g_ThumbnailSize }, { 0, 1 }, { 1,0 });

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t)); // ImGuiCond_Once
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
				{
					m_CurrentDirectory /= path.filename();
				}
			}

			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}

}