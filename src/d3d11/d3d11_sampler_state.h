#pragma once

#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11SamplerState
  * @brief Contains information to set the sampler state of the render device
  * @author Daniël Konings
  */
  class D3D11SamplerState
  {
  public:

    /**
    * @enum snuffbox::D3D11SamplerState::SamplerTypes
    * @brief An enumeration for both sampler types
    * @author Daniël Konings
    */
    enum SamplerTypes
    {
      kLinear,
      kPoint
    };

  public:
    /// Default constructor
    D3D11SamplerState(SamplerTypes type);

    /// Sets this sampler state as the current sampler
    void Set();

    /// Default destructor
    ~D3D11SamplerState();

  private:
    ID3D11SamplerState* sampler_state_; //!< The actual sampler state
  };
}