#include "../../d3d11/elements/d3d11_text_element.h"
#include "../../freetype/freetype_font_manager.h"
#include "../../freetype/freetype_font_atlas.h"
#include "../../freetype/freetype_font.h"
#include "../../freetype/freetype_rich_text.h"
#include "../../d3d11/d3d11_vertex_buffer.h"
#include "../../d3d11/d3d11_texture.h"
#include "../../d3d11/d3d11_material.h"
#include "../../d3d11/d3d11_effect.h"
#include "../../content/content_manager.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define HRES  64
#define HRESf 64.f
#define DPI   72

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	D3D11Text::D3D11Text() :
		current_font_(nullptr),
		spacing_y_(0.0f),
		spacing_x_(0.0f),
		alignment_(TextAlignment::kLeft),
		shadow_set_(false),
		shadow_offset_(0.0f, 0.0f),
		shadow_colour_(0.0f, 0.0f, 0.0f, 1.0f),
		current_colour_(1.0f, 1.0f, 1.0f, 1.0f),
		font_("fonts/arial.ttf"),
		font_size_(16.0f),
    text_("")
	{
		Create();
	}

	//-------------------------------------------------------------------------------------------
	D3D11Text::D3D11Text(JS_ARGS args) :
		D3D11RenderElement(args),
		current_font_(nullptr),
		spacing_y_(0.0f),
		spacing_x_(0.0f),
		alignment_(TextAlignment::kLeft),
		shadow_set_(false),
		shadow_offset_(0.0f, 0.0f),
		shadow_colour_(0.0f, 0.0f, 0.0f, 1.0f),
		current_colour_(1.0f, 1.0f, 1.0f, 1.0f),
		font_("fonts/arial.ttf"),
		font_size_(16.0f),
    text_("")
	{
		Create();
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::Create()
	{
		set_layer_type(D3D11RenderElement::LayerType::kUI);
		current_font_ = FontManager::Instance()->default_font();

		pen_.x = 0.0f;
		pen_.y = 0.0f;

		material_ = AllocatedMemory::Instance().Construct<D3D11Material>();

		material_->Validate();
		material_->set_effect(D3D11RenderDevice::Instance()->default_effect());
		set_technique("Text");

    material_group().material = material_.get();
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::FillBuffers(const std::wstring& buffer)
	{
		float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;

		float tx, ty, tw, th;
		int index_offset = static_cast<int>(vertices_.size()) / 4;

		RichTextMarkup markup;
		markup.font = current_font_;
		markup.colour = current_colour_;

		float new_height = markup.font->line_height();

		if (new_height > highest_)
		{
			highest_ = new_height;
		}

		for (int i = 0; i < buffer.size(); ++i)
		{
			wchar_t ch = buffer.at(i);

			if (ch == L'\n')
			{
				if (highest_ == 0.0f)
				{
					highest_ = markup.font->line_height();
				}
				pen_.y += (markup.font->line_gap() + highest_);
				pen_.x = 0.0f;
				++line_;
				highest_ = 0.0f;
				continue;
			}

			FontGlyph* glyph = markup.font->glyph(ch);

			w = static_cast<float>(glyph->width);
			h = static_cast<float>(glyph->height);
			x = std::roundf(pen_.x + glyph->x_offset);
			y = std::roundf(pen_.y - glyph->y_offset + current_font_->ascender());

			tx = glyph->tex_coords.left;
			th = glyph->tex_coords.top;
			tw = glyph->tex_coords.right;
			ty = glyph->tex_coords.bottom;

			Vertex verts[] = {
				{ XMFLOAT4(x, y, 0.0f, 1.0f), markup.colour, XMFLOAT2(tx, th), XMFLOAT3(0.0f, 0.0f, 1.0f) },
				{ XMFLOAT4(x, y + h, 0.0f, 1.0f), markup.colour, XMFLOAT2(tx, ty), XMFLOAT3(0.0f, 0.0f, 1.0f) },
				{ XMFLOAT4(x + w, y, 0.0f, 1.0f), markup.colour, XMFLOAT2(tw, th), XMFLOAT3(0.0f, 1.0f, 0.0f) },
				{ XMFLOAT4(x + w, y + h, 0.0f, 1.0f), markup.colour, XMFLOAT2(tw, ty), XMFLOAT3(0.0f, 1.0f, 0.0f) }
			};

			for (int j = 0; j < 4; ++j)
			{
				verts[j].position.z -= index_offset;
				vertices_.push_back(verts[j]);
				indices_.push_back(index_offset * 4 + j);

				if (j == 3)
				{
					indices_.push_back(index_offset * 4 + j);
					indices_.push_back((index_offset + 1) * 4);
				}
			}

			pen_.x += (glyph->x_advance) + spacing_x_;
			++index_offset;
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::FillIconBuffer(TextIcon& icon)
	{
		float size = icon.size * 2.0f;
		float x = icon.position.x;
		float y = icon.position.y;

		Vertex verts[] = {
			{ XMFLOAT4(x, y, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT4(x, y + size, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT4(x + size, y, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT4(x + size, y + size, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }
		};

		for (int j = 0; j < 4; ++j)
		{
			icon.vertices.push_back(verts[j]);
		}

    icon.indices = std::vector<int>({ 0, 1, 2, 3, 2, 1 });
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::DrawIcons()
	{
		material_group().material->effect()->Apply("UI", 0);
		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		for (D3D11Text::TextIcon& it : icon_buffer_)
		{
			if (it.icon == nullptr)
			{
				continue;
			}
			it.vertex_buffer->Set();
			it.icon->Set(1);
			it.vertex_buffer->Draw();
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::set_text(const std::string& text)
	{
		highest_ = 0.0f;
		line_ = 0;

		vertices_.clear();
		indices_.clear();
		icon_buffer_.clear();

		text_ = text;

		if (text.size() == 0)
		{
			return;
		}

		const int buffsize = MultiByteToWideChar(CP_UTF8, NULL, text.c_str(), -1, NULL, NULL);

		wchar_t* widestr = new wchar_t[buffsize];

		MultiByteToWideChar(CP_UTF8, NULL, text.c_str(), -1, widestr, buffsize);

		SNUFF_XASSERT(current_font_ != nullptr, "Text widget has no font set!", "D3D11Text::set_text::" + text);

		std::wstring buffer(widestr);
		MarkupOperations operations = RichTextParser::Parse(&buffer);

		Font* default_font = current_font_;
		float default_size = font_size_;

		bool is_bold = false;
		bool is_italic = false;
		bool was_nested = false;
		std::string font_string = font_;
		std::wstring parsed = L"";

		FontManager* font_manager = FontManager::Instance();

		for (int i = 0; i < operations.size(); ++i)
		{
			const Markup& it = operations.at(i);

			if (i + 1 < operations.size())
			{
				if (operations.at(i + 1).icon == true)
				{
					was_nested = true;
				}
			}

			if (it.is_default)
			{
				parsed += it.text;
				FillBuffers(it.text);

				if (was_nested == false)
				{
					current_font_ = default_font;
					current_colour_ = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					is_bold = false;
					is_italic = false;
					font_size_ = default_size;
					font_string = font_;
				}
				else
				{
					was_nested = false;
				}
			}
			else if (it.do_colour)
			{
				current_colour_ = XMFLOAT4(it.colour[0], it.colour[1], it.colour[2], 1.0f);
			}
			else if (it.bold)
			{
				is_bold = true;
				if (is_italic == false)
				{
					current_font_ = font_manager->GetFont(font_ + 'b', font_size_);
					font_string = font_ + 'b';
				}
				else
				{
					current_font_ = font_manager->GetFont(font_ + 'z', font_size_);
					font_string = font_ + 'z';
				}
			}
			else if (it.italic)
			{
				is_italic = true;
				if (is_bold == false)
				{
					current_font_ = font_manager->GetFont(font_ + 'i', font_size_);
					font_string = font_ + 'i';
				}
				else
				{
					current_font_ = font_manager->GetFont(font_ + 'z', font_size_);
					font_string = font_ + 'z';
				}
			}
			else if (it.do_size)
			{
				font_size_ = it.size;
				current_font_ = font_manager->GetFont(font_string, font_size_);
			}
			else if (it.new_font)
			{
				font_string = it.font;

				if (is_italic)
				{
					font_string = it.font + 'i';
				}
				else if (is_bold)
				{
					font_string = it.font + 'b';
				}

				if (is_italic && is_bold)
				{
					font_string = it.font + 'z';
				}

				current_font_ = font_manager->GetFont(font_string, font_size_);
			}
			else if (it.icon)
			{
				TextIcon icon;

				icon.position = XMFLOAT2(pen_.x, std::roundf(pen_.y + current_font_->glyph(L'A')->y_offset - current_font_->ascender()));

				float size = static_cast<float>(current_font_->glyph(L'A')->height);
				pen_.x += size + spacing_x_;
				icon.size = size;

				int s = WideCharToMultiByte(CP_UTF8, 0, it.icon_path.c_str(), -1, NULL, 0, 0, 0);

				char* multistr = new char[s];
				WideCharToMultiByte(CP_UTF8, 0, it.icon_path.c_str(), -1, multistr, s, 0, 0);

				icon.icon = ContentManager::Instance()->Get<D3D11Texture>(std::string(multistr));
				icon.line = line_;
				delete[] multistr;

				icon_buffer_.push_back(icon);
			}
		}

		current_font_ = default_font;
		current_colour_ = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		is_bold = false;
		is_italic = false;
		font_size_ = default_size;
		font_string = font_;

		Align(&parsed);

		vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);
		vertex_buffer_->Create(vertices_, indices_, false);
    vertex_buffer_->set_topology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		for (TextIcon& it : icon_buffer_)
		{
			it.vertex_buffer = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);
			it.vertex_buffer->Create(it.vertices, it.indices, false);
		}

		pen_.x = 0.0f;
		pen_.y = 0.0f;

		delete[] widestr;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::set_font(const std::string& path)
	{
		font_ = path;
		current_font_ = FontManager::Instance()->GetFont(font_, font_size_);
		set_text(text_);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::set_spacing(const float& x, const float& y)
	{
		spacing_x_ = x;
		spacing_y_ = y;

		set_text(text_);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::set_font_size(const float& size)
	{
		font_size_ = size;
		current_font_ = FontManager::Instance()->GetFont(font_, font_size_);
		set_text(text_);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::set_shadow_colour(const float& r, const float& g, const float& b, const float& a)
	{
		shadow_set_ = true;
		shadow_colour_.x = r;
		shadow_colour_.y = g;
		shadow_colour_.z = b;
		shadow_colour_.w = a;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::set_shadow_offset(const float& x, const float& y)
	{
		shadow_set_ = true;
		shadow_offset_.x = x;
		shadow_offset_.y = y;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::set_alignment(const D3D11Text::TextAlignment& alignment)
	{
		alignment_ = alignment;
		set_text(text_);
	}

	//-------------------------------------------------------------------------------------------
	const bool& D3D11Text::shadow_set() const
	{
		return shadow_set_;
	}

	//-------------------------------------------------------------------------------------------
	const std::string& D3D11Text::text() const
	{
		return text_;
	}

	//-------------------------------------------------------------------------------------------
	const std::string& D3D11Text::font() const
	{
		return font_;
	}

	//-------------------------------------------------------------------------------------------
	const float& D3D11Text::spacing_x() const
	{
		return spacing_x_;
	}

	//-------------------------------------------------------------------------------------------
	const float& D3D11Text::spacing_y() const
	{
		return spacing_y_;
	}

	//-------------------------------------------------------------------------------------------
	const float& D3D11Text::font_size() const
	{
		return font_size_;
	}

	//-------------------------------------------------------------------------------------------
	const float& D3D11Text::width() const
	{
		return width_;
	}

	//-------------------------------------------------------------------------------------------
	const float& D3D11Text::height() const
	{
		return height_;
	}

	//-------------------------------------------------------------------------------------------
	D3D11VertexBuffer* D3D11Text::vertex_buffer()
	{
		return vertex_buffer_ == nullptr ? nullptr : vertex_buffer_.get();
	}

	//-------------------------------------------------------------------------------------------
	const XMMATRIX& D3D11Text::world_matrix()
	{
		CalculateWorldMatrix(&world_matrix_, true);

		return world_matrix_;
	}

	//-------------------------------------------------------------------------------------------
  D3D11RenderElement::MaterialGroup& D3D11Text::material_group()
	{
		material_group_.material = material_.get();
		material_group_.material->set_textures(FontManager::Instance()->atlas()->texture());
		return material_group_;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::Align(std::wstring* buffer)
	{
		float highest_x;
		float highest_y;
		bool set = false;

		for (Vertex& it : vertices_)
		{
			if (set == false)
			{
				highest_x = it.position.x;
				highest_y = it.position.y;
				set = true;
				continue;
			}

			if (it.position.x > highest_x)
			{
				highest_x = it.position.x;
			}

			if (it.position.y > highest_y)
			{
				highest_y = it.position.y;
			}
		}

		float xx = 0.0f;
		float yy = 0.0f;
		for (TextIcon& it : icon_buffer_)
		{
			FillIconBuffer(it);
			xx = it.position.x + it.size * 2.0f;
			yy = it.position.y + it.size * 2.0f;

			if (xx > highest_x)
			{
				highest_x = xx;
			}

			if (yy > highest_y)
			{
				highest_y = yy;
			}
		}

		width_ = highest_x;
		height_ = std::abs(highest_y);

		if (buffer->size() == 1)
		{
			return;
		}

		std::vector<Vertex>& verts = vertices_;
		line_ = 0;

		if (alignment_ != TextAlignment::kLeft)
		{
			int offset = 0;
			int index = -1;
			wchar_t ch;

			for (int i = 0; i < buffer->size(); ++i)
			{
				ch = buffer->at(i);

				if (ch != L'\n')
				{
					index += 4;
				}

				if (ch == L'\n' || (i == buffer->size() - 1 && buffer->size() != 1))
				{
					float x = verts.at(index).position.x;

					for (TextIcon& it : icon_buffer_)
					{
						if (it.line == line_)
						{
							float xx = it.position.x + it.size * 2.0f;
							if (xx > x)
							{
								x = xx;
							}
						}
					}
					
					if (x < highest_x)
					{
						int delta = static_cast<int>(highest_x - x);

						if (alignment_ == TextAlignment::kCenter)
						{
							delta = static_cast<int>(delta / 2);
						}
						for (int j = offset; j <= index; ++j)
						{
							verts.at(j).position.x += delta;
						}

						for (TextIcon& it : icon_buffer_)
						{
							if (it.line == line_)
							{
								for (Vertex& vert : it.vertices)
								{
									vert.position.x += delta;
								}
							}
						}
					}

					offset = index + 1;
					++line_;
				}
			}
		}

		for (Vertex& it : verts)
		{
			if (alignment_ != TextAlignment::kLeft)
			{
				it.position.x -= alignment_ == TextAlignment::kRight ? width_ : width_ / 2;
			}
		}

		for (TextIcon& it : icon_buffer_)
		{
			for (Vertex& vert : it.vertices)
			{
				if (alignment_ != TextAlignment::kLeft)
				{
					vert.position.x -= alignment_ == TextAlignment::kRight ? width_ : width_ / 2;
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::ClearShadow()
	{
		shadow_set_ = false;
		shadow_colour_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		shadow_offset_ = XMFLOAT2(0.0f, 0.0f);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::PrepareShadow()
	{
		TranslateBy(shadow_offset_.x, shadow_offset_.y, 0.0f);
		set_blend(shadow_colour_.x, shadow_colour_.y, shadow_colour_.z);
		set_alpha(shadow_colour_.w * alpha());
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::Reset(const XMFLOAT3& blend, const float& alpha)
	{
		TranslateBy(-shadow_offset_.x, -shadow_offset_.y, 0.0f);
		set_blend(blend.x, blend.y, blend.z);
		set_alpha(alpha);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::RegisterJS(JS_CONSTRUCTABLE obj)
	{
		D3D11RenderElement::Register(obj);

		v8::Handle<v8::Object> enumerator = JSWrapper::CreateObject();

		JSWrapper::SetObjectValue(enumerator, "Left", 0);
		JSWrapper::SetObjectValue(enumerator, "Right", 1);
		JSWrapper::SetObjectValue(enumerator, "Center", 2);

		JSStateWrapper::Instance()->RegisterGlobal("TextAlignment", enumerator);

		JSFunctionRegister funcs[] = {
			{ "setText", JSSetText },
			{ "text", JSText },
			{ "font", JSFont },
			{ "setFont", JSSetFont },
			{ "setFontSize", JSSetFontSize },
			{ "fontSize", JSFontSize },
			{ "setSpacing", JSSetSpacing },
			{ "spacing", JSSpacing },
			{ "metrics", JSMetrics },
			{ "setAlignment", JSSetAlignment },
			{ "setShadowOffset", JSSetShadowOffset },
			{ "setShadowColour", JSSetShadowColour },
			{ "clearShadow", JSClearShadow }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSSetText(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		if (wrapper.Check("S") == true)
		{
			self->set_text(wrapper.GetValue<std::string>(0, ""));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSSetFont(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		if (wrapper.Check("S") == true)
		{
			self->set_font(wrapper.GetValue<std::string>(0, ""));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSSetFontSize(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		if (wrapper.Check("N") == true)
		{
			self->set_font_size(wrapper.GetValue<float>(0, 16.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSSetSpacing(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		if (wrapper.Check("NN") == true)
		{
			self->set_spacing(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSText(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		wrapper.ReturnValue<std::string>(self->text());
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSFont(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		wrapper.ReturnValue<std::string>(self->font());
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSFontSize(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		wrapper.ReturnValue<float>(self->font_size());
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSSpacing(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());
		
		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();
		JSWrapper::SetObjectValue(obj, "x", self->spacing_x());
		JSWrapper::SetObjectValue(obj, "y", self->spacing_y());

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSMetrics(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();
		JSWrapper::SetObjectValue(obj, "w", self->width());
		JSWrapper::SetObjectValue(obj, "h", self->height());

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSSetAlignment(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		self->set_alignment(static_cast<D3D11Text::TextAlignment>(wrapper.GetValue<int>(0, 0)));
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSSetShadowOffset(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		if (wrapper.Check("NN") == true)
		{
			self->set_shadow_offset(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSSetShadowColour(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->set_shadow_colour(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f), wrapper.GetValue<float>(3, 1.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Text::JSClearShadow(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Text* self = wrapper.GetPointer<D3D11Text>(args.This());

		self->ClearShadow();
	}

	//-------------------------------------------------------------------------------------------
	D3D11Text::~D3D11Text()
	{

	}
}