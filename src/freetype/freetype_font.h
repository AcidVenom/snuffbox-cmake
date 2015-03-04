#pragma once

#include <map>
#include "../memory/shared_ptr.h"

struct FT_LibraryRec_;
struct FT_FaceRec_;

namespace snuffbox
{
  class FontAtlas;
  struct FontGlyph;

	typedef std::map<wchar_t, SharedPtr<FontGlyph>> GlyphMap;

  /**
  * @class snuffbox::Font
  * @brief A wrapper class for a freetype font, use this to draw to a texture atlas
  * @author Daniël Konings
  */
  class Font
  {
  public:
    /// Default constructor
    Font();

    /// Default destructor
    ~Font();

    /**
    * @brief Loads a font from a relative path and stores its information in this class
    * @param[in] path (const std::string&) The path to the font file
    * @param[in] size (const float&) The font size to load
    */
    void Load(const std::string& path, const float& size);

    /**
    * @brief Loads a glyph from a given charcode
    * @param[in] charcode (const wchar_t&) The character code to load as a glyph
    * @return (snuffbox::FontGlyph*) The new or old glyph
    */
    FontGlyph* LoadGlyph(const wchar_t& charcode);

    /**
    * @brief Loads an entire string of glyphs
    * @param[in] str (const wchar_t*) The string
    * @return (int) Failed (> 0) Succes (0)
    */
		int LoadGlyphs(const wchar_t* str);

    /**
		* @brief Retrieves the glyph of a given character code
		* @param[in] charcode (const wchar_t&) The character code to retrieve the glyph of
    * @return (snuffbox::FontGlyph*) The glyph for the specific charcode
    */
		FontGlyph* glyph(const wchar_t& charcode);

		/**
		* @return const float& The ascender of this font
		*/
		const float& ascender() const;

		/**
		* @return const float& The line gap of this font
		*/
		const float& line_gap() const;

		/**
		* @return const float& The line height of this font
		*/
		const float& line_height() const;

  private:
		GlyphMap										glyphs_; //!< The loaded font glyphs
    FT_LibraryRec_*             library_; //!< The freetype library
    FT_FaceRec_*                face_; //!< The freetype face
    float                       size_; //!< The size of this font
    unsigned char               lcd_weights_[5]; //!< The LCD weights for LCD filtering
    float                       height_; //!< The height of this font
    float                       linegap_; //!< The line gap of this font
    float                       ascender_; //!< The ascender of this font
    float                       descender_; //!< The descender of this font
  };


  /**
  * @struct snuffbox::FontTexCoords
  * @brief Texture coordinates for a font glyph
  * @author Daniël Konings
  */
  struct FontTexCoords
  {
    FontTexCoords() : left(0.0f), right(0.0f), top(0.0f), bottom(0.0f){}
    float left, right, top, bottom;
  };

  /**
  * @struct snuffbox::FontGlyph
  * @brief Stores glyph information from freetype
  * @author Daniël Konings
  */
  struct FontGlyph
  {
    wchar_t charcode; //!< The character code of this glyph
    int width; //!< Pixel width
    int height; //!< Pixel height
    int x_offset; //!< Left bearing
    int y_offset; //!< Top bearing
    float x_advance; //!< Pen X advance distance
    float y_advance; //!< Pen Y advance distance
    FontTexCoords tex_coords; //!< Texture coordinates of this glyph in the atlas
  };
}