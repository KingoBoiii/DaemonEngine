#pragma once
#include "DaemonEngine.h"
#include <filesystem>

namespace Daemon
{

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnGUI();
	private:
		Shared<Texture2D> m_BackButtonIcon;
		Shared<Texture2D> m_DirectoryIcon;
		Shared<Texture2D> m_FileIcon;

		std::filesystem::path m_CurrentDirectory;
	};

}
