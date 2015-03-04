#pragma once

#include "../../d3d11/elements/d3d11_render_element.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11Widget
  * @brief A billboarding quad element
  * @author Daniël Konings
  */
	class D3D11Billboard : public D3D11RenderElement, public JSObject
  {
  public:
    /// Default constructor
		D3D11Billboard();

    /// JavaScript constructor
		D3D11Billboard(JS_ARGS args);

    /// Creates the vertex buffer
    void Create();

    /// @see snuffbox::D3D11RenderElement
    D3D11VertexBuffer* vertex_buffer();

    /// Default destructor
		virtual ~D3D11Billboard();

  private:
    SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of this widget
		XMMATRIX world_matrix_; //!< The world matrix of this widget

  public:
    JS_NAME("Billboard");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
  };
}