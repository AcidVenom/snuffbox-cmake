#pragma once

#include "../js/js_object.h"
#include <Windows.h>
#include <xnamath.h>

namespace snuffbox
{
  class D3D11Widget;
  class Mouse;

  /**
  * @class snuffbox::MouseArea
  * @brief A mouse area for use with the UI
  * @author Daniël Konings
  */
  class MouseArea : public JSObject
  {
  public:

    /**
    * @struct snuffbox::MouseArea::Metrics
    * @brief Contains metrics information of a mouse area
    * @author Daniël Konings
    */
    struct Metrics
    {
      float sx, sy;
      float ox, oy;
    };

    /**
    * @struct snuffbox::MouseArea::Rect
    * @brief Contains information for when the mouse area is transformed into world space
    * @author Daniël Konings
    */
    struct Rect
    {
      float x1, y1;
      float x2, y2;
    };

  public:

    /// Default constructor
    MouseArea();

    /// JavaScript constructor
    MouseArea(JS_ARGS args);

    /**
    * @brief Checks for interaction with this mouse area
    * @param[in] mouse (snuffbox::Mouse*) A pointer to the mouse currently in use
    */
    void Check(Mouse* mouse);

    /**
    * @brief Checks if a point is within the given boundaries
    * @param[in] p (const XMVECTOR&) The point to check for
    * @param[in] p1 (const XMVECTOR&) The upper left point of the rectangle
    * @param[in] p2 (const XMVECTOR&) The upper right point of the rectangle
    */
    bool InBounds(const XMVECTOR& p, const XMVECTOR& p1, const XMVECTOR& p2);

    /**
    * @brief Sets the parent of this mouse area
    * @param[in] parent (snuffbox::D3D11Widget*) The parent to set
    */
    void set_parent(D3D11Widget* parent);

    /// Default destructor
    virtual ~MouseArea();

  private:
    Metrics metrics_; //!< The metrics of this mouse area
    D3D11Widget* parent_; //!< The parent of this mouse area

  public:
    JS_NAME("MouseArea");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
    
  };
}