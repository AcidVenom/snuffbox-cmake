#pragma once

#include "../d3d11/d3d11_render_device.h"

#include <vector>

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11InputLayout
  * @brief The vertex input layout for the render device to use
  * @author Daniël Konings
  */
  class D3D11InputLayout
  {
  public:
    /// Default constructor
    D3D11InputLayout();

    /**
    * @brief Creates the input layout
    * @param[in] layout (std::vector<D3D11_INPUT_ELEMENT_DESC>) The list of input elements
    * @param[in] vs_buffer (ID3D10Blob*) The associated vertex shader blob
    */
    void Create(std::vector<D3D11_INPUT_ELEMENT_DESC> layout, ID3D10Blob* vs_buffer);

    /// Sets the input layout
    void Set();

    /// Default destructor
    ~D3D11InputLayout();

  private:
    ID3D11InputLayout* input_layout_; //!< The actual input layout
    bool valid_; //!< Is this input layout valid?
  };
}