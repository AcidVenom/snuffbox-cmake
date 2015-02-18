#include "../content/content_manager.h"
#include "../content/content.h"

#include "../platform/platform_file_watch.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	ContentManager::ContentManager()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	ContentManager* ContentManager::Instance()
	{
		static SharedPtr<ContentManager> content_manager = AllocatedMemory::Instance().Construct<ContentManager>();
		return content_manager.get();
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Notify(ContentManager::Events evt, ContentTypes type, std::string path)
	{
		switch (evt)
		{
		case Events::kLoad:
			Load(type, path);
			break;

		case Events::kReload:
			Reload(type, path);
			break;

		case Events::kUnload:
			Unload(type, path);
			break;

		default:
			break;
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Load(const ContentTypes& type, const std::string& path)
	{
		if (type == ContentTypes::kScript)
		{
			SNUFF_LOG_INFO("Loaded JavaScript file '" + path + "'");
		}

		FileWatch::Instance()->Add(path, type);
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Reload(const ContentTypes& type, const std::string& path)
	{
		if (type == ContentTypes::kScript)
		{
			SNUFF_LOG_INFO("Hot reloaded JavaScript file '" + path + "'");
			JSStateWrapper::Instance()->CompileAndRun(path, true);
			return;
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Unload(const ContentTypes& type, const std::string& path)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	ContentManager::~ContentManager()
	{

	}
}