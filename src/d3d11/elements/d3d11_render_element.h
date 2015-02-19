#pragma once

#include "../../js/js_object.h"

namespace snuffbox
{
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

  private:
    static void Register(JS_CONSTRUCTABLE obj);
  };
}