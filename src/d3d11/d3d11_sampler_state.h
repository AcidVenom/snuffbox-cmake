#pragma once

#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11SamplerState
  * @brief Contains information to set the sampler state of the render device
  * @author Dani�l Konings
  */
  class D3D11SamplerState
  {
  public:

    /**
    * @enum snuffbox::D3D11SamplerState::SamplerTypes
    * @brief An enumeration for both sampler types
    * @author Dani�l Konings
    */
    enum SamplerTypes
    {
      kLinear,
			kAnisotropic,
      kPoint
    };

  public:
    /**
		* @brief Construct a sampler state by type
		* @param[in] type (const SamplerTypes&) The type of sampler to construct
		*/
    D3D11SamplerState(const SamplerTypes& type);

    /// Sets this sampler state as the current sampler
    void Set();

    /// Default destructor
    ~D3D11SamplerState();

  private:
    ID3D11SamplerState* sampler_state_; //!< The actual sampler state
  };
}