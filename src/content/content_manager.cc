#include "../content/content_manager.h"
#include "../content/content.h"

#include "../platform/platform_file_watch.h"

#include "../d3d11/d3d11_shader.h"
#include "../d3d11/d3d11_effect.h"
#include "../d3d11/d3d11_blend_state.h"
#include "../d3d11/d3d11_depth_state.h"

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
	void ContentManager::Notify(const ContentManager::Events& evt, const ContentTypes& type, const std::string& path)
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
		if (type != ContentTypes::kScript)
		{
			SharedPtr<Content> content;
			
			if (type == ContentTypes::kShader)
			{
				content = AllocatedMemory::Instance().Construct<D3D11Shader>();
			}
			else if (type == ContentTypes::kEffect)
			{
				content = AllocatedMemory::Instance().Construct<D3D11Effect>();
			}

			content->Load(path);
      loaded_content_.emplace(path, content);
    }

		SNUFF_LOG_INFO("Loaded file '" + path + "'");
		FileWatch::Instance()->Add(path, type);
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Reload(const ContentTypes& type, const std::string& path)
	{
		if (type == ContentTypes::kScript)
		{
			SNUFF_LOG_INFO("Hot reloaded script '" + path + "'");
			JSStateWrapper::Instance()->CompileAndRun(path, true);
		}
		else if (type == ContentTypes::kCustom)
		{
			SNUFF_LOG_INFO("Hot reloaded custom file '" + path + "'");
		}

		loaded_content_.find(path)->second->Load(path);
		SNUFF_LOG_INFO("Hot reloaded file '" + path + "'");
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Unload(const ContentTypes& type, const std::string& path)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Watch(const std::string& path)
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
  ContentTypes ContentManager::StringToType(const std::string& type)
  {
    if (type == "texture")
    {
      return ContentTypes::kTexture;
    }
    else if (type == "shader")
    {
      return ContentTypes::kShader;
    }
		else if (type == "effect")
		{
			return ContentTypes::kEffect;
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