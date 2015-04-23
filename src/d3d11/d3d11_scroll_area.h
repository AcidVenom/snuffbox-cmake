#pragma once

#include "../js/js_object.h"
#include "../d3d11/d3d11_render_device.h"

#undef max
#undef min

namespace snuffbox
{
  class D3D11Widget;
  class D3D11RenderQueue;
  class MouseArea;

  /**
  * @class snuffbox::D3D11ScrollArea
  * @brief A scroll area to be used with widget/text elements
  * @author Daniël Konings
  */
  class D3D11ScrollArea : public JSObject
  {
  public:
    /// Default constructor
    D3D11ScrollArea();

    /// JavaScript constructor
    D3D11ScrollArea(JS_ARGS args);

    /// Initialises the scroll area
    void Initialise();

    /**
    * @brief Sets the z-index of this scroll area
    * @param[in] z (const float&) The new z-index
    */
    void SetZ(const float& z);
    
    /// Clears this scroll area of elements
    void Clear();

    /**
    * @return XMFLOAT2 The current scroll values
    */
    XMFLOAT2 Values();

    /**
    * @brief Sets the current scroll values on both X and Y
    * @param[in] x (const float&) The new x value
    * @param[in] y (const float&) The new y value
    */
    void SetValues(const float& x, const float& y);

    /**
    * @brief Scrolls by the specified value
    * @param[in] x (const float&) The x value to scroll by horizontally
    * @param[in] x (const float&) The y value to scroll by vertically
    */
    void ScrollBy(const float& x, const float& y);

    /**
    * @brief Sets the size of this scroll area, adjusting the scissor rect
    * @param[in] w (const float&) The width of the scroll area
    * @param[in] h (const float&) The height of the scroll area
    */
    void set_size(const float& w, const float& h);

    /**
    * @brief Sets the position of this scroll area, adjusting the scissor rect
    * @param[in] x (const float&) The x position of the scroll area
    * @param[in] y (const float&) The y position of the scroll area
    */
    void set_position(const float& x, const float& y);

    /**
    * @brief Sets the maximum values (in pixels) this scroll area can scroll to
    * @param[in] x (const float&) The new x value
    * @param[in] y (const float&) The new y value
    */
    void set_max(const float& x, const float& y);

    /**
    * @brief Sets the focus of this scroll area
    * @param[in] focus (const bool&) The boolean value
    */
    void set_focussed(const bool& focus);

    /**
    * @return const XMFLOAT2& The size of this scroll area, representing the scissor rect
    */
    const XMFLOAT2& size() const;

    /**
    * @return const XMFLOAT2& The position of this scroll area, also the start position of the scissor rect
    */
    const XMFLOAT2& position() const;

    /**
    * @return const float& The z-index of this scroll area
    */
    float z();
    
    /**
    * @return const XMFLOAT2& The maximum values (in pixels) The scroll area can scroll to
    */
    const XMFLOAT2& max() const;

    /**
    * @return const bool& Is this scroll area focussed or not? 
    */
    const bool& is_foccused() const;

    /// Default destructor
    virtual ~D3D11ScrollArea();

  private:
    SharedPtr<D3D11Widget> frame_; //!< The frame widget of this scroll area that handles the input and positioning/sizing
    SharedPtr<D3D11Widget> root_; //!< The root widget that is used for actual scrolling
    SharedPtr<MouseArea> mouse_area_; //!< The mouse area that belongs to this scroll area
    SharedPtr<D3D11RenderQueue> children_; //!< The children of this scroll area
    D3D11RenderTarget* target_; //!< The target this scroll area is spawned on
    XMFLOAT2 size_; //!< The size of the scroll area;
    XMFLOAT2 position_; //!< Sets the position of this scroll area
    XMFLOAT2 max_; //!< The maximum value of this scroll area
    bool is_focussed_; //!< Is this scroll area focussed?

  public:
    JS_NAME("ScrollArea");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
    static void JSSetPosition(JS_ARGS args);
    static void JSPosition(JS_ARGS args);
    static void JSSetSize(JS_ARGS args);
    static void JSSize(JS_ARGS args);
    static void JSSetMax(JS_ARGS args);
    static void JSMax(JS_ARGS args);
    static void JSFocussed(JS_ARGS args);
    static void JSSetValues(JS_ARGS args);
    static void JSValues(JS_ARGS args);
    static void JSScrollBy(JS_ARGS args);
    static void JSClear(JS_ARGS args);
    static void JSSetZ(JS_ARGS args);
    static void JSZIndex(JS_ARGS args);
  };
}