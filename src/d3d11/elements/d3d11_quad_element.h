#pragma once

#include "../../d3d11/elements/d3d11_render_element.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11Quad
  * @brief A quad element of 4 vertices
  * @author Daniël Konings
  */
  class D3D11Quad : public D3D11RenderElement, public JSObject
  {
  public:
    /// Default constructor
    D3D11Quad();

    /// JavaScript constructor
    D3D11Quad(JS_ARGS args);

    /// Creates the vertex buffer
    void Create();

    /// @see snuffbox::D3D11RenderElement
    D3D11VertexBuffer* vertex_buffer();

    /// Default destructor
    virtual ~D3D11Quad();

  private:
    SharedPtr<D3D11VertexBuffer> vertex_buffer_; //!< The vertex buffer of this quad

  public:
    JS_NAME("Quad");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
  };
}