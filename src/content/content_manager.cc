#include "../content/content_manager.h"
#include "../content/content.h"
#include "../content/content_box.h"

#include "../platform/platform_file_watch.h"

#include "../d3d11/d3d11_shader.h"
#include "../d3d11/d3d11_effect.h"
#include "../d3d11/d3d11_blend_state.h"
#include "../d3d11/d3d11_depth_state.h"
#include "../d3d11/d3d11_texture.h"
#include "../d3d11/d3d11_material.h"
#include "../d3d11/elements/particles/d3d11_particle_effect.h"
#include "../fbx/fbx_model.h"
#include "../animation/anim.h"
#include "../fmod/fmod_sound.h"

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
		SNUFF_LOG_INFO("Loading file '" + path + "'");
		if (type != ContentTypes::kScript)
		{
			std::map<std::string, SharedPtr<Content>>::iterator it = loaded_content_.find(path);

			if (it != loaded_content_.end())
			{
				SNUFF_LOG_WARNING("The file '" + path + "' was already loaded");
				return;
			}

			SharedPtr<Content> content;
			
			if (type == ContentTypes::kShader)
			{
				content = AllocatedMemory::Instance().Construct<D3D11Shader>();
			}
			else if (type == ContentTypes::kEffect)
			{
				content = AllocatedMemory::Instance().Construct<D3D11Effect>();
			}
			else if (type == ContentTypes::kTexture)
			{
				content = AllocatedMemory::Instance().Construct<D3D11Texture>();
			}
			else if (type == ContentTypes::kMaterial)
			{
				content = AllocatedMemory::Instance().Construct<D3D11Material>();
			}
			else if (type == ContentTypes::kModel)
			{
				content = AllocatedMemory::Instance().Construct<FBXModel>();
			}
      else if (type == ContentTypes::kBox)
      {
        content = AllocatedMemory::Instance().Construct<Box>();
      }
			else if (type == ContentTypes::kAnim)
			{
				content = AllocatedMemory::Instance().Construct<Anim>();
			}
      else if (type == ContentTypes::kSound)
      {
        content = AllocatedMemory::Instance().Construct<Sound>();
      }
			else if (type == ContentTypes::kParticleEffect)
			{
				content = AllocatedMemory::Instance().Construct<D3D11ParticleEffect>();
			}
			else
			{
				SNUFF_LOG_WARNING("No content loader was specified for the content type of '" + path + "'");
				return;
			}
			content->Load(path);
			content->Validate();
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
			return;
		}
    else if (type == ContentTypes::kBox)
    {
      SNUFF_LOG_INFO("Edited box file '" + path + "', reload this box to load its contents");
      return;
    }
		else if (type == ContentTypes::kCustom)
		{
			SNUFF_LOG_INFO("Hot reloaded custom file '" + path + "'");
      return;
		}

		loaded_content_.find(path)->second->Load(path);
		SNUFF_LOG_INFO("Hot reloaded file '" + path + "'");
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::Unload(const ContentTypes& type, const std::string& path)
	{
    if (type == ContentTypes::kBox)
    {
      SNUFF_LOG_INFO("Unloading box '" + path + "'");
      loaded_content_.erase(path);
      FileWatch::Instance()->Remove(path);
      SNUFF_LOG_INFO("Unloaded box '" + path + "'");
      return;
    }

		SNUFF_LOG_INFO("Unloading file '" + path + "'");
		std::map<std::string, SharedPtr<Content>>::iterator it = loaded_content_.find(path);
		if (it != loaded_content_.end())
		{
			if (it->second->type() != type)
			{
				SNUFF_LOG_ERROR("The specified file type does not match the type of the loaded file '" + path + "'");
				return;
			}
			it->second->Invalidate();
			to_unload_.push(path);
			SNUFF_LOG_INFO("Unloaded file '" + path + "'");
			return;
		}

		SNUFF_LOG_ERROR("Attempted to unload a file that hasn't been loaded '" + path + "'");
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
	void ContentManager::UnloadAll()
	{
    while (to_unload_.empty() == false)
		{
      const std::string& top = to_unload_.front();

			loaded_content_.erase(loaded_content_.find(top));
			FileWatch::Instance()->Remove(top);

      to_unload_.pop();
		}
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
    else if (type == "box")
    {
      return ContentTypes::kBox;
    }
		else if (type == "anim")
		{
			return ContentTypes::kAnim;
		}
		else if (type == "particle")
		{
			return ContentTypes::kParticleEffect;
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
			{ "load", JSLoad },
			{ "unload", JSUnload },
			{ "watch", JSWatch }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::JSLoad(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("SS"))
		{
			ContentManager::Instance()->Notify(ContentManager::Events::kLoad,
				ContentManager::StringToType(wrapper.GetValue<std::string>(0, "undefined")),
				wrapper.GetValue<std::string>(1, "undefined")
				);
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void ContentManager::JSUnload(JS_ARGS args)
	{
		JSWrapper wrapper(args);

		if (wrapper.Check("SS"))
		{
			ContentManager::Instance()->Notify(ContentManager::Events::kUnload,
				ContentManager::StringToType(wrapper.GetValue<std::string>(0, "undefined")),
				wrapper.GetValue<std::string>(1, "undefined"));
		}
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