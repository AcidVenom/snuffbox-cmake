#pragma once

#include "../../d3d11/elements/d3d11_render_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"
#include "../../js/js_object.h"

#include <vector>

namespace snuffbox
{
	class Font;
	struct RichTextOperation;

	/**
	* @class snuffbox::D3D11Text
	* @brief A UI widget element as a container or a visual representation
	* @author Daniël Konings
	*/
	class D3D11Text : public D3D11RenderElement, public JSObject
	{
	public:

		/**
		* @struct snuffbox::RichTextMarkup
		* @brief The current markup being used
		* @author Daniël Konings
		*/
		struct RichTextMarkup
		{
			Font* font;
			XMFLOAT4 colour;
		};

		/**
		* @struct snuffbox::TextIcon
		* @brief Holds all information required to render an icon on the right position in the text
		* @author Daniël Konings
		*/
		struct TextIcon
		{
			TextIcon() : icon(nullptr), path(L""), position(0.0f, 0.0f), size(0.0f){}
			SharedPtr<D3D11VertexBuffer> vertex_buffer;
			D3D11Texture* icon;
			std::wstring path;
			XMFLOAT2 position;
			std::vector<Vertex> vertices;
			std::vector<int> indices;
			float size;
			int line;
		};

		/**
		* @enum snuffbox::TextAlignment
		* @brief Text alignment enumeration
		* @author Daniël Konings
		*/
		enum TextAlignment
		{
			kLeft,
			kRight,
			kCenter
		};

		/// Default constructor
		D3D11Text();

		/// JavaScript constructor
		D3D11Text(JS_ARGS);

		/// Creates the text
		void Create();

		/// Clears the shadow of this element
		void ClearShadow();

		/// Prepares the shadow before drawing
		void PrepareShadow();

		/**
		* @brief Resets the text after drawing the shadow
		* @param[in] blend (const XMFLOAT3&) The old blend value
		* @param[in] alpha (const float&) The old alpha value
		*/
		void Reset(const XMFLOAT3& blend, const float& alpha);

		/**
		* @brief Fills the vertex buffer and index buffer from a given string of text
		* @param[in] buffer (const std::wstring&) The text buffer
		*/
		void FillBuffers(const std::wstring& buffer);

		/*
		* @brief Fills the vertex and index buffer of an icon
		* @param[in] icon (snuffbox::TextIcon&) A reference to the text icon structure to fill the buffer with
		*/
		void FillIconBuffer(TextIcon& icon);

		/**
		* @brief Aligns the text using a string to check how to align all buffers
		* @param[in] buffer (std::wstring*) The text buffer to check from
		*/
		void Align(std::wstring* buffer);

		/// Draws the icons
		void DrawIcons();

		/**
		* @return const bool& Does this text element have a shadow set?
		*/
		const bool& shadow_set() const;

		/**
		* @return const std::string& The text of this text element
		*/
		const std::string& text() const;

		/**
		* @return const std::string& The font of this text element
		*/
		const std::string& font() const;

		/**
		* @return const float& The x spacing of this text element
		*/
		const float& spacing_x() const;

		/**
		* @return const float& The y spacing of this text element
		*/
		const float& spacing_y() const;

		/**
		* @return const float& The current font size
		*/
		const float& font_size() const;

		/**
		* @return const float& The total width of the text
		*/
		const float& width() const;

		/**
		* @return const float& The total height of the text
		*/
		const float& height() const;

		/**
		* @brief Sets the text of this text widget
		* @param[in] text (const std::string&) The actual text
		*/
		void set_text(const std::string& text);

		/**
		* @brief Sets the font of this text widget
		* @param[in] path (const std::string&) The font path
		*/
		void set_font(const std::string& path);

		/**
		* @brief Sets the font size of this element
		* @param[in] size (const float&) The font size
		*/
		void set_font_size(const float& size);

		/**
		* @brief Sets the spacing of the font
		* @param[in] x (const float&) The x spacing
		* @param[in] y (const float&) The y spacing
		*/
		void set_spacing(const float& x, const float& y);

		/**
		* @brief Sets the shadow offset of this text element
		* @param[in] x (const float&) The X offset
		* @param[in] y (const float&) The Y offset
		*/
		void set_shadow_offset(const float& x, const float& y);

		/**
		* @brief Sets the shadow colour of this text element
		* @param[in] r (const float&) The red value
		* @param[in] g (const float&) The green value
		* @param[in] b (const float&) The blue value
		* @param[in] a (const float&) The alpha value
		*/
		void set_shadow_colour(const float& r, const float& g, const float& b, const float& a);

		/**
		* @brief Sets the alignment of this text element
		* @param[in] const snuffbox::D3D11Text::TextAlignment& The alignment to set
		*/
		void set_alignment(const TextAlignment& alignment);

		//// @see snuffbox::D3D11RenderElement::vertex_buffer
		D3D11VertexBuffer* vertex_buffer();

		//// @see snuffbox::D3D11RenderElement::world_matrix
		const XMMATRIX& world_matrix();

		//// @see snuffbox::D3D11RenderElement::material
		D3D11Material* material();

		/// Default destructor
		virtual ~D3D11Text();

	private:
		SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of this element

		std::string               text_; //!< The string of this text
		Font*                     current_font_; //!< The font this text uses
		XMFLOAT4									current_colour_; //!< The current font colour
		float                     spacing_y_; //!< The vertical spacing
		float                     spacing_x_; //!< The horizontal spacing
		float                     font_size_; //!< The current font size
		std::string               font_; //!< The current font
		TextAlignment             alignment_; //!< The text alignment of this text object
		float                     width_; //!< The width of this element
		float                     height_; //!< The height of this element
		bool											shadow_set_; //!< Does this font have a shadow set?
		XMFLOAT4									shadow_colour_; //!< The shadow colour of this text element
		XMFLOAT2									shadow_offset_; //!< The shadow offset of this text element
		XMFLOAT2									pen_; //!< The pen position

		std::vector<TextIcon>			icon_buffer_; //!< All icons that need to be rendered
		int												line_; //!< The current line

		std::vector<Vertex>				vertices_; //!< The vertices of this element
		std::vector<int>					indices_; //!< The indices of this element

		XMMATRIX									world_matrix_; //!< The world matrix of the text
		SharedPtr<D3D11Material>  material_; //!< The material used for text

		float											highest_; //!< The biggest height found to adjust to newline
	public:
		JS_NAME("Text");
		static void RegisterJS(JS_CONSTRUCTABLE obj);

		static void JSSetText(JS_ARGS args);
		static void JSSetFont(JS_ARGS args);
		static void JSSetSpacing(JS_ARGS args);
		static void JSSetFontSize(JS_ARGS args);
		static void JSText(JS_ARGS args);
		static void JSFontSize(JS_ARGS args);
		static void JSFont(JS_ARGS args);
		static void JSSpacing(JS_ARGS args);
		static void JSMetrics(JS_ARGS args);
		static void JSSetAlignment(JS_ARGS args);
		static void JSSetShadowOffset(JS_ARGS args);
		static void JSSetShadowColour(JS_ARGS args);
		static void JSClearShadow(JS_ARGS args);
	};
}