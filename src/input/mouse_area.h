#pragma once

#include "../js/js_object.h"
#include "../input/mouse.h"
#include "../js/js_callback.h"

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

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
    * @param[in] callback (const bool&) Should callback be invoked?
    * @return bool Was the mouse in bounds of this mouse area?
    */
    bool Check(Mouse* mouse, const bool& callback);

    /**
    * @brief Checks if a point is within the given boundaries
    * @param[in] p (const XMVECTOR&) The point to check for
    * @param[in] p1 (const XMVECTOR&) The upper left point of the rectangle
    * @param[in] p2 (const XMVECTOR&) The upper right point of the rectangle
    */
    bool InBounds(const XMVECTOR& p, const XMVECTOR& p1, const XMVECTOR& p2);

    /// When the mouse area was entered
    void OnEnter();

    /// When the mouse area was left after being entered
    void OnLeave();

    /// When the mouse area is being held down on
    void OnDown(const Mouse::MouseButton& button);

    /// When the mouse area is being pressed
    void OnPressed(const Mouse::MouseButton& button);

    /// When the mouse area is being released
    void OnReleased(const Mouse::MouseButton& button);

    /**
    * @brief Sets the on enter callback
    * @param[in] on_enter (const v8::Handle<v8::Value>&) The on enter callback
    */
    void SetOnEnter(const v8::Handle<v8::Value>& on_pressed);

    /**
    * @brief Sets the on leave callback
    * @param[in] on_leave (const v8::Handle<v8::Value>&) The on leave callback
    */
    void SetOnLeave(const v8::Handle<v8::Value>& on_pressed);

    /**
    * @brief Sets the on down callback
    * @param[in] on_down (const v8::Handle<v8::Value>&) The on down callback
    */
    void SetOnDown(const v8::Handle<v8::Value>& on_pressed);

    /**
    * @brief Sets the on pressed callback
    * @param[in] on_pressed (const v8::Handle<v8::Value>&) The on pressed callback
    */
    void SetOnPressed(const v8::Handle<v8::Value>& on_pressed);

    /**
    * @brief Sets the on released callback
    * @param[in] on_released (const v8::Handle<v8::Value>&) The on released callback
    */
    void SetOnReleased(const v8::Handle<v8::Value>& on_pressed);

    /// Resets all was pressed values
    void ResetWasPressed();

    /**
    * @brief The Z value of this mouse area
    */
    float GetZ();

    /**
    * @return const snuffbox::MouseArea::Metrics& The metrics of this mouse area
    */
    const Metrics& metrics() const;

    /**
    * @brief Sets the parent of this mouse area
    * @param[in] parent (snuffbox::D3D11Widget*) The parent to set
    */
    void set_parent(D3D11Widget* parent);

    /**
    * @brief Sets the scaling of this mouse area local to the widget parent
    * @param[in] sx (const float&) The x scaling
    * @param[in] sy (const float&) The y scaling
    */
    void set_scale(const float& sx, const float& sy);

    /**
    * @brief Sets the offset of this mouse area local to the widget parent
    * @param[in] ox (const float&) The x offsetting
    * @param[in] oy (const float&) The y offsetting
    */
    void set_offset(const float& ox, const float& oy);

    /// Default destructor
    virtual ~MouseArea();

  private:
    Metrics metrics_; //!< The metrics of this mouse area
    D3D11Widget* parent_; //!< The parent of this mouse area
    bool entered_; //!< Was this mouse area entered?
    bool was_pressed_[3]; //!< Was this mouse area pressed?
    JSCallback<> on_enter_; //!< The on enter callback
    JSCallback<> on_leave_; //!< The on leave callback
    JSCallback<int> on_down_; //!< The on down callback
    JSCallback<int> on_pressed_; //!< The on pressed callback
    JSCallback<int> on_released_; //!< The on released callback

  public:
    JS_NAME("MouseArea");
    static void RegisterJS(JS_CONSTRUCTABLE obj);
    static void JSSetScale(JS_ARGS args);
    static void JSScale(JS_ARGS args);
    static void JSSetOffset(JS_ARGS args);
    static void JSOffset(JS_ARGS args);
    static void JSSetOnEnter(JS_ARGS args);
    static void JSSetOnLeave(JS_ARGS args);
    static void JSSetOnDown(JS_ARGS args);
    static void JSSetOnPressed(JS_ARGS args);
    static void JSSetOnReleased(JS_ARGS args);
  };
}