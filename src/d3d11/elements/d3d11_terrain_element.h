#pragma once

#include "../../d3d11/elements/d3d11_render_element.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11Terrain
  * @brief A terrain element made up out of quads
  * @author Daniël Konings
  */
  class D3D11Terrain : public D3D11RenderElement, public JSObject
  {
  public:
    /// Default constructor
		D3D11Terrain();

    /// JavaScript constructor
		D3D11Terrain(JS_ARGS args);

    /**
		* @brief Creates the vertex buffer by dimensions
		* @param[in] w (const int&) The width of the terrain
		* @param[in] h (const int&) The height of the terrain
		*/
		void Create(const int& w, const int& h);

    /// @see snuffbox::D3D11RenderElement
    D3D11VertexBuffer* vertex_buffer();

		/**
		* @return const int& The width of the current terrain
		*/
		const int& width() const;

		/**
		* @return const int& The height of the current terrain
		*/
		const int& height() const;

    /// Default destructor
		virtual ~D3D11Terrain();

  private:
    SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of this quad
		int width_; //!< The width of the terrain
		int height_; //!< The height of the terrain

  public:
    JS_NAME("Terrain");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
		static void JSCreate(JS_ARGS args);
		static void JSWidth(JS_ARGS args);
		static void JSHeight(JS_ARGS args);
  };
}