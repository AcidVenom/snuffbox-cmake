#include "../input/mouse_area.h"

#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_viewport.h"

#include "../d3d11/elements/d3d11_widget_element.h"
#include "../d3d11/d3d11_render_settings.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  MouseArea::MouseArea() :
    metrics_({1.0f, 1.0f, 0.0f, 0.0f }),
    parent_(nullptr),
    entered_(false)
  {
    ResetWasPressed();
  }

  //-------------------------------------------------------------------------------------------
  MouseArea::MouseArea(JS_ARGS args) :
    metrics_({1.0f, 1.0f, 0.0f, 0.0f }),
    parent_(nullptr),
    entered_(false)
  {
    JSWrapper wrapper(args);

    if (wrapper.Check("O") == true)
    {
      parent_ = wrapper.GetPointer<D3D11Widget>(0);
      parent_->AddMouseArea(this);
      Mouse::Instance()->AddMouseArea(this);
    }

    ResetWasPressed();
  }

  //-------------------------------------------------------------------------------------------
  bool MouseArea::Check(Mouse* mouse, const bool& callback)
  {
    if (parent_ == nullptr)
    {
      return false;
    }

    const XMFLOAT2& res = D3D11RenderSettings::Instance()->resolution();
    D3D11Viewport* vp = nullptr;

    if (parent_->target() != nullptr && parent_->target()->viewport() != nullptr && parent_->target()->viewport()->valid() == true)
    {
      vp = parent_->target()->viewport();
    }
    else
    {
      vp = D3D11RenderDevice::Instance()->viewport_render_target();
    }

    XMMATRIX proj = XMMatrixOrthographicRH(res.x, res.y, 0.0f, 1.0f);
    XMMATRIX wp = parent_->world_matrix() * proj;

    Mouse::float2 screen = mouse->Position(Mouse::MousePosition::kScreen);

    if (screen.x > 1.0f || screen.x < -1.0f || screen.y > 1.0f || screen.y < -1.0f)
    {
      if (entered_ == true)
      {
				OnLeave();
        entered_ = false;
      }
      return false;
    }

    XMVECTOR deter;
    XMVECTOR p1 = XMVector2Transform(XMVectorSet(screen.x, screen.y, 0.0f, 1.0f), XMMatrixInverse(&deter, wp));
    Rect rect = {
      0.0f, 0.0f,
      metrics_.sx, metrics_.sy
    };

    rect.x1 += metrics_.ox;
    rect.x2 += metrics_.ox;

    rect.y1 += metrics_.oy;
    rect.y2 += metrics_.oy;
    
    XMVECTOR p2 = XMVectorSet(rect.x1, rect.y1, 0.0f, 1.0f);
    XMVECTOR p3 = XMVectorSet(rect.x2, rect.y2, 0.0f, 1.0f);

    bool in_bounds = InBounds(p1, p2, p3);

    if (in_bounds == true)
    {
      if (entered_ == false)
      {
				if (callback == true)
				{
					OnEnter();
				}

				entered_ = true;
      }

      Mouse::MouseButton button;
      for (unsigned int i = 0; i < 3; ++i)
      {
        button = static_cast<Mouse::MouseButton>(i);
        if (mouse->IsDown(button) || mouse->IsDoubleClicked(button) == true)
        {
          if (callback == true)
          {
            OnDown(button);
          }
        }

        if (mouse->IsPressed(button) || mouse->IsDoubleClicked(button) == true)
        {
          was_pressed_[button] = true;

          if (callback == true)
          {
            OnPressed(button);
          }
        }

        if (mouse->IsReleased(button) == true)
        {
          if (was_pressed_[button] == true)
          {
            if (callback == true)
            {
              OnReleased(button);
            }
            was_pressed_[button] = false;
          }
        }
      }
    }
    
		if (in_bounds == false || callback == false)
    {
      if (entered_ == true)
      {
        OnLeave();
        entered_ = false;
      }

      Mouse::MouseButton button;
      for (unsigned int i = 0; i < 3; ++i)
      {
        button = static_cast<Mouse::MouseButton>(i);

        if (mouse->IsDown(button) == true)
        {
          was_pressed_[button] = false;
        }
      }
    }

    return in_bounds;
  }

  //-------------------------------------------------------------------------------------------
  bool MouseArea::InBounds(const XMVECTOR& p, const XMVECTOR& p1, const XMVECTOR& p2)
  {
    XMFLOAT2 mouse;
    XMFLOAT2 upper;
    XMFLOAT2 lower;

    XMStoreFloat2(&mouse, p);
    XMStoreFloat2(&upper, p1);
    XMStoreFloat2(&lower, p2);

    if (mouse.x >= upper.x && mouse.y >= upper.y && mouse.x <= lower.x && mouse.y <= lower.y)
    {
      return true;
    }

    return false;
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::OnEnter()
  {
    on_enter_.Call();
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::OnLeave()
  {
    on_leave_.Call();
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::OnDown(const Mouse::MouseButton& button)
  {
    on_down_.Call(static_cast<int>(button));
  }


  //-------------------------------------------------------------------------------------------
  void MouseArea::OnPressed(const Mouse::MouseButton& button)
  {
    on_pressed_.Call(static_cast<int>(button));
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::OnReleased(const Mouse::MouseButton& button)
  {
    on_released_.Call(static_cast<int>(button));
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::SetOnEnter(const v8::Handle<v8::Value>& on_enter)
  {
    on_enter_.Set(on_enter);
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::SetOnLeave(const v8::Handle<v8::Value>& on_leave)
  {
    on_leave_.Set(on_leave);
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::SetOnDown(const v8::Handle<v8::Value>& on_down)
  {
    on_down_.Set(on_down);
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::SetOnPressed(const v8::Handle<v8::Value>& on_pressed)
  {
    on_pressed_.Set(on_pressed);
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::SetOnReleased(const v8::Handle<v8::Value>& on_released)
  {
    on_released_.Set(on_released);
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::ResetWasPressed()
  {
    for (unsigned int i = 0; i < 3; ++i)
    {
      was_pressed_[i] = false;
    }
  }

  //-------------------------------------------------------------------------------------------
  float MouseArea::GetZ()
  {
    if (parent_ == nullptr)
    {
      return 0.0f;
    }

    return XMVectorGetZ(parent_->translation());
  }

  //-------------------------------------------------------------------------------------------
  const MouseArea::Metrics& MouseArea::metrics() const
  {
    return metrics_;
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::set_parent(D3D11Widget* parent)
  {
    parent_ = parent;
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::set_scale(const float& x, const float& y)
  {
    metrics_.sx = x;
    metrics_.sy = y;
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::set_offset(const float& x, const float& y)
  {
    metrics_.ox = x;
    metrics_.oy = y;
  }

  //-------------------------------------------------------------------------------------------
  MouseArea::~MouseArea()
  {
    if (parent_ != nullptr)
    {
      parent_->RemoveMouseArea(this);
      Mouse::Instance()->RemoveMouseArea(this);
    }
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::RegisterJS(JS_CONSTRUCTABLE obj)
  {
    JSFunctionRegister funcs[] = {
      { "setScale", JSSetScale },
      { "scale", JSScale },
      { "setOffset", JSSetOffset },
      { "offset", JSOffset },
      { "setOnEnter", JSSetOnEnter },
      { "setOnLeave", JSSetOnLeave },
      { "setOnDown", JSSetOnDown },
      { "setOnPressed", JSSetOnPressed },
      { "setOnReleased", JSSetOnReleased }
    };

    JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::JSSetScale(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    MouseArea* self = wrapper.GetPointer<MouseArea>(args.This());

    if (wrapper.Check("NN") == true)
    {
      self->set_scale(wrapper.GetValue<float>(0, 1.0f), wrapper.GetValue<float>(1, 1.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::JSScale(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    MouseArea* self = wrapper.GetPointer<MouseArea>(args.This());

    const Metrics& metrics = self->metrics();

    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();
    JSWrapper::SetObjectValue(obj, "x", metrics.sx);
    JSWrapper::SetObjectValue(obj, "y", metrics.sy);

    wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::JSSetOffset(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    MouseArea* self = wrapper.GetPointer<MouseArea>(args.This());

    if (wrapper.Check("NN") == true)
    {
      self->set_offset(wrapper.GetValue<float>(0, 1.0f), wrapper.GetValue<float>(1, 1.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::JSOffset(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    MouseArea* self = wrapper.GetPointer<MouseArea>(args.This());

    const Metrics& metrics = self->metrics();

    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();
    JSWrapper::SetObjectValue(obj, "x", metrics.ox);
    JSWrapper::SetObjectValue(obj, "y", metrics.oy);

    wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::JSSetOnEnter(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    MouseArea* self = wrapper.GetPointer<MouseArea>(args.This());

    if (wrapper.Check("F") == true)
    {
      self->SetOnEnter(args[0]);
    }
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::JSSetOnLeave(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    MouseArea* self = wrapper.GetPointer<MouseArea>(args.This());

    if (wrapper.Check("F") == true)
    {
      self->SetOnLeave(args[0]);
    }
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::JSSetOnDown(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    MouseArea* self = wrapper.GetPointer<MouseArea>(args.This());

    if (wrapper.Check("F") == true)
    {
      self->SetOnDown(args[0]);
    }
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::JSSetOnPressed(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    MouseArea* self = wrapper.GetPointer<MouseArea>(args.This());

    if (wrapper.Check("F") == true)
    {
      self->SetOnPressed(args[0]);
    }
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::JSSetOnReleased(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    MouseArea* self = wrapper.GetPointer<MouseArea>(args.This());

    if (wrapper.Check("F") == true)
    {
      self->SetOnReleased(args[0]);
    }
  }
}