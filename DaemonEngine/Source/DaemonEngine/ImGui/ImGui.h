#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace Daemon::UI
{

	static char* s_IDBuffer = new char[16];
	static uint32_t s_Counter = 0;

	static void GenerateID()
	{
		s_IDBuffer[0] = '#';
		s_IDBuffer[1] = '#';
		memset(s_IDBuffer + 2, 0, 14);
		sprintf_s(s_IDBuffer + 2, 14, "%o", s_Counter++);
	}

	static bool Property(const char* label, float& value, float delta = 0.1f)
	{
		bool modified = false;

		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		GenerateID();
		if (ImGui::DragFloat(s_IDBuffer, &value, delta))
		{
			modified = true;
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		return modified;
	}

	static bool Property(const char* label, glm::vec2& vector, float delta = 0.1f)
	{
		bool modified = false;

		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		GenerateID();
		if (ImGui::DragFloat2(s_IDBuffer, glm::value_ptr(vector), delta))
		{
			modified = true;
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		return modified;
	}

	static bool Property(const char* label, glm::vec3& vector, float delta = 0.1f)
	{
		bool modified = false;

		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		GenerateID();
		if (ImGui::DragFloat3(s_IDBuffer, glm::value_ptr(vector), delta))
		{
			modified = true;
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		return modified;
	}

	static bool PropertyColor(const char* label, glm::vec4& color)
	{
		bool modified = false;

		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		GenerateID();
		if (ImGui::ColorEdit4(s_IDBuffer, glm::value_ptr(color)))
		{
			modified = true;
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		return modified;
	}

	static bool PropertyDropdown(const char* label, const char** options, int32_t optionCount, int32_t* selected)
	{
		const char* current = options[*selected];
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		bool changed = false;

		const std::string id = "##" + std::string(label);
		if (ImGui::BeginCombo(id.c_str(), current))
		{
			for (int i = 0; i < optionCount; i++)
			{
				const bool is_selected = (current == options[i]);
				if (ImGui::Selectable(options[i], is_selected))
				{
					current = options[i];
					*selected = i;
					changed = true;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		return changed;
	}

	static void BeginPropertyGrid()
	{
		ImGui::Columns(2);
	}

	static void EndPropertyGrid()
	{
		ImGui::Columns(1);
		s_Counter = 0;
	}

}