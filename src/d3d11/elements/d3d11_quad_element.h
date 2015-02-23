#pragma once

#include "../../d3d11/elements/d3d11_render_element.h"

namespace snuffbox
{
  class D3D11Quad : public D3D11RenderElement, public JSObject
  {
  public:
    D3D11Quad();
    D3D11Quad(JS_ARGS args);

    /// Creates the vertex buffer
    void Create();

    D3D11VertexBuffer* vertex_buffer();

    virtual ~D3D11Quad();

  private:
    SharedPtr<D3D11VertexBuffer> vertex_buffer_;

  public:
    JS_NAME("Quad");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
  };
}