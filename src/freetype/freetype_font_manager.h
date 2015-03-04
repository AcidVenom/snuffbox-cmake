#pragma once

#include <string>
#include <map>

#include "../freetype/freetype_font.h"
#include "../freetype/freetype_font_atlas.h"
#include "../memory/shared_ptr.h"

namespace snuffbox
{
  class Font;
	typedef std::map <float, SharedPtr<Font>> FontSizeMap;
  typedef std::map<std::string, std::map<float, SharedPtr<Font>>> FontMap;

	/**
	* @class snuffbox::FontManager
	* @brief Manages all fonts loaded by FreeType
	* @author Daniël Konings
	*/
  class FontManager
  {
  public:
		/// Default constructor
    FontManager();

		/**
		* @brief Retrieves the singleton instance for this class
		* @return FontManager* The pointer to the singleton
		*/
		static FontManager* Instance();

		/// Initialises the FreeType font manager
		void Initialise();

		/**
		* @brief Requests a font to retrieve, if it doesn't exist; create it
		* @param[in] path (const std::string&) The relative path
		* @param[in] size (const float&) The requested font size
		*/
		Font* GetFont(const std::string& path, const float& size);

		/**
		* @brief Requests the default font from the font manager
		* @return (snuffbox::Font*) The pointer to the default font
		*/
		Font* default_font();

		/**
		* @return (snuffbox::FontAtlas*) The pointer to the font atlas used
		*/
		FontAtlas* atlas();

		/// Default destructor
    ~FontManager();

  private:
    FontMap map_; //!< Contains all font name <-> font size mappings
    Font* default_font_; //!< The default font
		SharedPtr<FontAtlas> atlas_; //!< The atlas to hold all fonts
  };
}