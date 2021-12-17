workspace "DaemonEngine"
	architecture "x86_64"
	startproject "Sandbox"

	configurations { 
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/DaemonEngine/Vendor/glfw/include";
IncludeDir["glad"] = "%{wks.location}/DaemonEngine/Vendor/glad/include";
IncludeDir["glm"] = "%{wks.location}/DaemonEngine/Vendor/glm";
IncludeDir["spdlog"] = "%{wks.location}/DaemonEngine/vendor/spdlog/include";
IncludeDir["imgui"] = "%{wks.location}/DaemonEngine/vendor/imgui";
IncludeDir["stb_image"] = "%{wks.location}/DaemonEngine/vendor/stb_image";
IncludeDir["freetype"] = "%{wks.location}/DaemonEngine/vendor/freetype/include";
IncludeDir["ftgl"] = "%{wks.location}/DaemonEngine/vendor/freetype/freetype-gl";
IncludeDir["box2d"] = "%{wks.location}/DaemonEngine/vendor/box2d/include";
IncludeDir["entt"] = "%{wks.location}/DaemonEngine/vendor/entt/include";
IncludeDir["yaml_cpp"] = "%{wks.location}/DaemonEngine/vendor/yaml-cpp/include";
IncludeDir["imguizmo"] = "%{wks.location}/DaemonEngine/vendor/ImGuizmo";

LibraryDir = {}
Library = {}

group "Dependencies"
	include "DaemonEngine/Vendor/glfw"
	include "DaemonEngine/Vendor/glad"
	include "DaemonEngine/vendor/imgui"
	include "DaemonEngine/vendor/freetype"
	include "DaemonEngine/vendor/box2d"
	include "DaemonEngine/vendor/yaml-cpp"
group ""

include "DaemonEngine"
include "DaemonEditor"
include "Sandbox"
