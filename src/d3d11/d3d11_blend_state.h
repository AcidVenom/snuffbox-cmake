#pragma once

#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11BlendState
  * @brief Used to set blend states per-object
  * @author Daniël Konings
  */
  class D3D11BlendState
  {
  public:
    /// Default constructor
    D3D11BlendState();

    /**
    * @brief Creates the actual blend state
    * @param[in] desc D3D11_BLEND_DESC The description of the blend state
    */
    void Create(D3D11_BLEND_DESC desc);

    /**
    * @brief Creates the blend state from a JSON
    * @param[in] json_string (std::string) The JSON string
    */
    void CreateFromJson(std::string json_string);

    /// Sets this blend state
    void Set();

    /// Default destructor
    ~D3D11BlendState();

    /**
    * @brief Converts a string to a blend enumerator
    * @param[in] str (std::string) The string to convert
    * @return D3D11_BLEND The converted value
    */
    static D3D11_BLEND StringToBlend(std::string str);

    /**
    * @brief Converts a string to a blend operation enumerator
    * @param[in] str (std::string) The string to convert
    * @return D3D11_BLEND_OP The converted value
    */
    static D3D11_BLEND_OP StringToBlendOp(std::string str);

  private:
    bool valid_; //!< Is this blend state valid?
    ID3D11BlendState* blend_state_; //!< The actual blend state
  };
}