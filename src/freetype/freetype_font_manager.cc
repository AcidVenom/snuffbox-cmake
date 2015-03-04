#include "../freetype/freetype_font_manager.h"
#include "../freetype/freetype_font_atlas.h"

#define BASE_WCHAR L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,-=+\\/()*&^@#!_><?%~:;\"'_áàèé`ëäüùúäïìíîâêûãñöóòôõ"

namespace snuffbox
{
  //------------------------------------------------------------------------------------------------
  FontManager::FontManager()
  {
		
  }

	//------------------------------------------------------------------------------------------------
	FontManager* FontManager::Instance()
	{
		static SharedPtr<FontManager> font_manager = AllocatedMemory::Instance().Construct<FontManager>();
		return font_manager.get();
	}

	//------------------------------------------------------------------------------------------------
	void FontManager::Initialise()
	{
		atlas_ = AllocatedMemory::Instance().Construct<FontAtlas>(2048, 3);
		default_font_ = GetFont("fonts/arial.ttf", 16);

		SNUFF_LOG_SUCCESS("Succesfully initialised the FreeType font manager");
	}

  //------------------------------------------------------------------------------------------------
  Font* FontManager::GetFont(const std::string& path, const float& size)
  {
		FontMap::iterator font_exists = map_.find(path);
    
		if (font_exists != map_.end())
    {
			FontSizeMap& size_map = font_exists->second;
			FontSizeMap::iterator size_exists = size_map.find(size);

			if (size_exists != size_map.end())
      {
				return size_exists->second.get();
      }
      else
      {
        SharedPtr<Font> font = AllocatedMemory::Instance().Construct<Font>();

        font->Load(path, size);
        font->LoadGlyphs(BASE_WCHAR);

        Font* f = font.get();

        size_map.emplace(size, SharedPtr<Font>(std::move(font)));

        SNUFF_LOG_INFO("Loading font '" + path + "' with size '" + std::to_string(size) + "'");

        return f;
      }
    }
    else
    {
      SharedPtr<Font> font = AllocatedMemory::Instance().Construct<Font>();

      font->Load(path, size);
      font->LoadGlyphs(BASE_WCHAR);

      FontSizeMap map;

      Font* f = font.get();
      map.emplace(size, SharedPtr<Font>(std::move(font)));

      map_.emplace(path, map);
      SNUFF_LOG_INFO("Loading font '" + path + "' with size '" + std::to_string(size) + "'");

      return f;
    }
  }

	//------------------------------------------------------------------------------------------------
	FontAtlas* FontManager::atlas()
	{
		return atlas_.get();
	}

  //------------------------------------------------------------------------------------------------
  Font* FontManager::default_font()
  {
    return default_font_;
  }

  //------------------------------------------------------------------------------------------------
  FontManager::~FontManager()
  {

  }
}