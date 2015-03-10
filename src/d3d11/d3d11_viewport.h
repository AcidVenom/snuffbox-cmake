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
    * @param[in] x (const float&) The x position of the viewport
    * @param[in] y (const float&) The y position of the viewport
    * @param[in] w (const float&) The width of the viewport
    * @param[in] h (const float&) The height of the viewport
    */
		void Create(const float& x, const float& y, const float& w, const float& h);
	
		/**
		* @brief Makes sure this viewport maintains a certain aspect ratio
		* @param[in] width (const float&) The width of the underlying window
		* @param[in] height (const float&) The height of the underlying window
		* @param[in] r_width (const float&) The width of the underlying renderer
		* @param[in] r_height (const float&) The height of the underlying renderer
		*/
		void SetToAspectRatio(const float& width, const float& height, const float& r_width, const float& r_height);

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

    /**
    * @return const bool& Is this viewport valid?
    */
    const bool& valid() const;

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