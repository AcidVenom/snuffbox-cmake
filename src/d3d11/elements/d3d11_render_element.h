#pragma once

#include "../../js/js_object.h"
#include "../../d3d11/d3d11_render_device.h"

namespace snuffbox
{
  class D3D11VertexBuffer;

  /**
  * @class snuffbox::D3D11RenderElement
  * @brief Contains information for the render device to use in drawing different elements
  * @author Daniël Konings
  */
  class D3D11RenderElement
  {
  public:
    /// Default constructor
    D3D11RenderElement();

    /// Default destructor
    ~D3D11RenderElement();

    /**
    * @brief Spawns this element on a given render target by name
    * @param[in] target (const std::string&) The name of the target to spawn the element on
    */
    void Spawn(const std::string& target);

    /**
    * @return snuffbox::D3D11VertexBuffer* The vertex buffer associated with this render element
    */
    virtual D3D11VertexBuffer* vertex_buffer() = 0;

  public:
    static void Register(JS_CONSTRUCTABLE obj);
    static void JSSpawn(JS_ARGS args);
  };
}