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
		else if (type == ContentTypes::kCustom)
		{
			SNUFF_LOG_INFO("Hot reloaded custom file '" + path + "'");
			return;
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Unload(const ContentTypes& type, const std::string& path)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Watch(std::string path)
	{
		bool success = FileWatch::Instance()->Add(path, ContentTypes::kCustom);

		if (success == true)
		{
			SNUFF_LOG_INFO("Added '" + path + "' to the file watch");
			return;
		}
		
		SNUFF_LOG_ERROR("'" + path + "' could not be added to the file watch");
	}

	//---------------------------------------------------------------------------------------------------------
	ContentManager::~ContentManager()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::RegisterJS(JS_SINGLETON obj)
	{
		JSFunctionRegister funcs[] = {
			{ "watch", JSWatch }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::JSWatch(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		if (wrapper.Check("S"))
		{
			ContentManager::Instance()->Watch(wrapper.GetValue<std::string>(0, "undefined"));
			return;
		}
		
		SNUFF_LOG_ERROR("No path specified to watch, file will not be added to file watch");
	}
}