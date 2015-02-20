#pragma once

#include "../d3d11/d3d11_render_device.h"

namespace snuffbox
{
  /**
  * @class snuffbox::D3D11Viewport
  * @brief A viewport to draw to a region of the window
  * @author Daniël Konings
  */
  class D3D11Viewport
  {
  public:
    /// Default constructor
    D3D11Viewport();

    /**
    * @brief Creates the viewport
    * @param[in] x (float) The x position of the viewport
    * @param[in] y (float) The y position of the viewport
    * @param[in] w (float) The width of the viewport
    * @param[in] h (float) The height of the viewport
    */
    void Create(float x, float y, float w, float h);

    /// Sets this viewport for rendering
    void Set();
    
    /**
    * @return const float& The x position of the viewport
    */
    const float& x() const;

    /**
    * @return const float& The y position of the viewport
    */
    const float& y() const;

    /**
    * @return const float& The width of the viewport
    */
    const float& width() const;

    /**
    * @return const float& The height of the viewport
    */
    const float& height() const;

    /// Default destructor
    ~D3D11Viewport();

  private:
    float x_; //!< The x position of this viewport
    float y_; //!< The y position of this viewport
    float width_; //!< The width of this viewport
    float height_; //!< The height of this viewport
    D3D11_VIEWPORT viewport_; //!< The actual viewport
    bool valid_; //!< Is this viewport valid?

    /// Adjusts the viewport to the given parameters
    void Adjust();
  };
}