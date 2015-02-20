#include "../content/content_manager.h"
#include "../content/content.h"

#include "../platform/platform_file_watch.h"

#include "../d3d11/d3d11_shader.h"

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
    else if (type == ContentTypes::kShader)
    {
      SharedPtr<Content> shader = AllocatedMemory::Instance().Construct<D3D11Shader>();
      static_cast<D3D11Shader*>(shader.get())->Load(path);

      loaded_content_.emplace(path, shader);
      SNUFF_LOG_INFO("Loaded shader file '" + path + "'");
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
    else if (type == ContentTypes::kShader)
    {
      static_cast<D3D11Shader*>(loaded_content_.find(path)->second.get())->Load(path);
      SNUFF_LOG_INFO("Hot reloaded shader file '" + path + "'");
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
  Content* ContentManager::Get(std::string path)
  {
    std::map<std::string, SharedPtr<Content>>::iterator it = loaded_content_.find(path);

    if (it != loaded_content_.end())
    {
      return it->second.get();
    }

    SNUFF_LOG_ERROR("Could not find content '" + path + "', are you sure it's been loaded correctly?");
    return nullptr;
  }

  //---------------------------------------------------------------------------------------------------------
  ContentTypes ContentManager::StringToType(std::string type)
  {
    if (type == "texture")
    {
      return ContentTypes::kTexture;
    }
    else if (type == "shader")
    {
      return ContentTypes::kShader;
    }
    else if (type == "material")
    {
      return ContentTypes::kMaterial;
    }
    else if (type == "model")
    {
      return ContentTypes::kModel;
    }
    else if (type == "sound")
    {
      return ContentTypes::kSound;
    }
    else
    {
      SNUFF_LOG_WARNING("Tried to load unknown content type '" + type + "'");
      return ContentTypes::kUnknown;
    }
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