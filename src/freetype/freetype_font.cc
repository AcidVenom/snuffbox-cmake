#include "../freetype/freetype_font.h"
#include "../freetype/freetype_font_atlas.h"
#include "../freetype/freetype_font_manager.h"
#include "../application/game.h"
#include "../d3d11/d3d11_texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H

#define HRES  64
#define HRESf 64.f
#define DPI   72

namespace snuffbox
{
  //------------------------------------------------------------------------------------------------
  Font::Font() :
    library_(nullptr),
    face_(nullptr)
  {
    
  }

  //------------------------------------------------------------------------------------------------
  Font::~Font()
  {
    if (face_ != nullptr)
    {
      FT_Done_Face(face_);
      face_ = nullptr;
    }

    if (library_ != nullptr)
    {
      FT_Done_FreeType(library_);
      library_ = nullptr;
    }
  }

  //------------------------------------------------------------------------------------------------
  void Font::Load(const std::string& path, const float& size)
  {
    FT_Error error;

    FT_Matrix matrix = {
      static_cast<int>((1.0 / HRES) * 0x10000L),
			static_cast<int>((0.0) * 0x10000L),
			static_cast<int>((0.0) * 0x10000L),
			static_cast<int>((1.0) * 0x10000L),
    };

    error = FT_Init_FreeType(&library_);
    SNUFF_XASSERT(!error, "Error initialising freetype for font '" + path + "'", "Font::Load::" + path);

    error = FT_New_Face(library_, (Game::Instance()->path() + "/" + path).c_str(), 0, &face_);
		SNUFF_XASSERT(!error, "Failed loading face for font '" + path + "'", "Font::Load::" + path);

    error = FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
		SNUFF_XASSERT(!error, "Error selecting charmap for font '" + path + "'", "Font::Load::" + path);

    error = FT_Set_Char_Size(face_, static_cast<int>(size * HRES), 0, DPI*HRES, DPI);
		SNUFF_XASSERT(!error, "Error setting size for font '" + path + "'", "Font::Load::" + path);

    FT_Set_Transform(face_, &matrix, nullptr);

    size_ = size;

    height_ = 0.0f;
    ascender_ = 0.0f;
    descender_ = 0.0f;

		lcd_weights_[0] = 0x10;
		lcd_weights_[1] = 0x40;
		lcd_weights_[2] = 0x70;
		lcd_weights_[3] = 0x40;
		lcd_weights_[4] = 0x10;
    
    FT_Size_Metrics metrics;
    metrics = face_->size->metrics;
    ascender_ = static_cast<float>(metrics.ascender >> 6);
		descender_ = static_cast<float>(metrics.descender >> 6);
		height_ = static_cast<float>(metrics.height >> 6);
    linegap_ = height_ - ascender_ + descender_;

    LoadGlyph(-1);
  }

  //------------------------------------------------------------------------------------------------
  FontGlyph* Font::LoadGlyph(wchar_t charcode)
  {
		FontAtlas* atlas = FontManager::Instance()->atlas();
		for (GlyphMap::iterator it = glyphs_.find(charcode); it != glyphs_.end() && it->second->charcode == charcode; ++it)
    {
      FontGlyph* glyph = it->second.get();
      if (glyph->charcode == (wchar_t)(-1))
      {
        return it->second.get();
      }
    }

    if (charcode == static_cast<wchar_t>(-1))
    {
			int width = atlas->size();
			int height = atlas->size();
			FontAtlasRegion region = atlas->CreateRegion(5, 5);
      SharedPtr<FontGlyph> glyph = AllocatedMemory::Instance().Construct<FontGlyph>();
      static signed char data[4 * 4 * 3] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

      SNUFF_XASSERT(region.x > 0, "Font Texture Atlas is full", "Font::LoadGlyph");

			atlas->FillRegion(region, reinterpret_cast<const unsigned char*>(data), 4 * 3);
      glyph->charcode = static_cast<wchar_t>(-1);
      glyph->tex_coords.left = region.x / static_cast<float>(width);
			glyph->tex_coords.top = region.y / static_cast<float>(height);
			glyph->tex_coords.right = region.width / static_cast<float>(width);
			glyph->tex_coords.bottom = region.height / static_cast<float>(height);

      FontGlyph* ptr = glyph.get();
      glyphs_.emplace(-1, glyph);
      return ptr;
    }

    wchar_t buffer[2] = { charcode, 0 };
    if (LoadGlyphs(buffer) == 0)
    {
      return glyphs_.find(charcode)->second.get();
    }

    return nullptr;
  }

  //------------------------------------------------------------------------------------------------
  int Font::LoadGlyphs(const wchar_t* str)
  {
		FontAtlas* atlas = FontManager::Instance()->atlas();
		int atlas_size = atlas->size();

		wchar_t c;
    const int count = static_cast<int>(std::char_traits<wchar_t>::length(str));

    for (int i = 0; i < count; ++i)
    {
      c = str[i];

      if (glyphs_.find(c) != glyphs_.end())
      {
        continue;
      }

      FT_UInt glyph_index = FT_Get_Char_Index(face_, c);

			FT_Library_SetLcdFilter(library_, FT_LCD_FILTER_LIGHT);
			
			FT_Library_SetLcdFilterWeights(library_, lcd_weights_);
			FT_Int32 flags = FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LCD;

			FT_Error error = FT_Load_Glyph(face_, glyph_index, flags);
      SNUFF_XASSERT(!error, "Unable to load glyph", "Font::LoadGlyphs");

      FT_GlyphSlot slot;
      FT_Bitmap bitmap;
      int top = 0, left = 0;

      slot = face_->glyph;
			bitmap = slot->bitmap;
      top = slot->bitmap_top;
      left = slot->bitmap_left;

      int width = bitmap.width / 3 + 1;
			int height = bitmap.rows + 1;
			FontAtlasRegion region = atlas->CreateRegion(width, height);
      SNUFF_XASSERT(region.x > 0, "Texture font atlas is full", "Font::LoadGlyphs");

      region.width -= 1;
      region.height -= 1;
			atlas->FillRegion(region, bitmap.buffer, bitmap.pitch);

      SharedPtr<FontGlyph> glyph = AllocatedMemory::Instance().Construct<FontGlyph>();
      glyph->charcode = c;
      glyph->width = width - 1;
      glyph->height = height - 1;
      glyph->x_offset = left;
      glyph->y_offset = top;
      glyph->tex_coords.left = region.x / static_cast<float>(atlas_size);
			glyph->tex_coords.top = region.y / static_cast<float>(atlas_size);
			glyph->tex_coords.right = glyph->tex_coords.left + glyph->width / static_cast<float>(atlas_size);
			glyph->tex_coords.bottom = glyph->tex_coords.top + glyph->height / static_cast<float>(atlas_size);

      slot = face_->glyph;
      glyph->x_advance = slot->advance.x / HRESf;
      glyph->y_advance = slot->advance.y / HRESf;

      glyphs_.emplace(c, glyph);
    }

		atlas->CreateTexture();
    return 0;
  }

	//------------------------------------------------------------------------------------------------
	const float& Font::ascender() const
	{
		return ascender_;
	}

	//------------------------------------------------------------------------------------------------
	const float& Font::line_gap() const
	{
		return linegap_;
	}

	//------------------------------------------------------------------------------------------------
	const float& Font::line_height() const
	{
		return height_;
	}

  //------------------------------------------------------------------------------------------------
  FontGlyph* Font::glyph(wchar_t charcode)
  {
		GlyphMap::iterator glyph = glyphs_.find(charcode);

    if (glyph == glyphs_.end())
    {
      wchar_t buffer[1] = { charcode };
      LoadGlyphs(buffer);

      glyph = glyphs_.find(charcode);
    }

    return glyph->second.get();
  }
}