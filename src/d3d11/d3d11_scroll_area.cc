#include "../d3d11/d3d11_scroll_area.h"
#include "../d3d11/elements/d3d11_widget_element.h"
#include "../input/mouse_area.h"
#include "../d3d11/d3d11_render_queue.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11ScrollArea::D3D11ScrollArea() :
    is_focussed_(false),
    size_(0.0f, 0.0f),
    position_(0.0f, 0.0f),
    max_(0.0f, 0.0f)
  {
    Initialise();
  }

  //-------------------------------------------------------------------------------------------
  D3D11ScrollArea::D3D11ScrollArea(JS_ARGS args) :
    is_focussed_(false),
    size_(0.0f, 0.0f),
    position_(0.0f, 0.0f),
    max_(0.0f, 0.0f)
  {
    Initialise();
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::Initialise()
  {
    children_ = AllocatedMemory::Instance().Construct<D3D11RenderQueue>(nullptr);
    frame_ = AllocatedMemory::Instance().Construct<D3D11Widget>();
    root_ = AllocatedMemory::Instance().Construct<D3D11Widget>();
    mouse_area_ = AllocatedMemory::Instance().Construct<MouseArea>();

    mouse_area_->set_parent(frame_.get());
    mouse_area_->set_scroll_area(this);
    frame_->AddMouseArea(mouse_area_.get());

    root_->set_parent(frame_.get());

    Mouse::Instance()->AddMouseArea(mouse_area_.get());
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::SetZ(const float& z)
  {
    frame_->SetZ(z);
  }
  
  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::Clear()
  {
    children_->Clear();
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::SetValues(const float& x, const float& y)
  {
    root_->set_translation(std::min(std::max(x, 0.0f), max_.x), std::min(std::max(y, 0.0f), max_.y), z());
  }

  //-------------------------------------------------------------------------------------------
  XMFLOAT2 D3D11ScrollArea::Values()
  {
    XMFLOAT2 ret;
    XMStoreFloat2(&ret, root_->translation());

    return ret;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::ScrollBy(const float& x, const float& y)
  {
    XMVECTOR t = root_->translation() + XMLoadFloat2(&XMFLOAT2(x, y));
    XMFLOAT2 trans;
    XMStoreFloat2(&trans, t);

    trans.x = std::min(std::max(x, 0.0f), max_.x);
    trans.y = std::max(std::max(y, 0.0f), max_.y);

    root_->set_translation(trans.x, trans.y, z());
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::set_position(const float& x, const float& y)
  {
    position_.x = x;
    position_.y = y;

    frame_->set_translation(x, y, z());
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::set_size(const float& w, const float& h)
  {
    size_.x = w;
    size_.y = h;

    frame_->set_size(w, h, 1);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::set_max(const float& x, const float& y)
  {
    max_.x = x;
    max_.y = y;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::set_focussed(const bool& focus)
  {
    is_focussed_ = focus;
  }

  //-------------------------------------------------------------------------------------------
  const XMFLOAT2& D3D11ScrollArea::position() const
  {
    return position_;
  }

  //-------------------------------------------------------------------------------------------
  const XMFLOAT2& D3D11ScrollArea::size() const
  {
    return size_;
  }

  //-------------------------------------------------------------------------------------------
  const XMFLOAT2& D3D11ScrollArea::max() const
  {
    return max_;
  }

  //-------------------------------------------------------------------------------------------
  float D3D11ScrollArea::z()
  {
    return XMVectorGetZ(frame_->translation());
  }

  //-------------------------------------------------------------------------------------------
  const bool& D3D11ScrollArea::is_foccused() const
  {
    return is_focussed_;
  }

  //-------------------------------------------------------------------------------------------
  D3D11ScrollArea::~D3D11ScrollArea()
  {
    children_->Clear();
    mouse_area_->set_scroll_area(nullptr);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::RegisterJS(JS_CONSTRUCTABLE obj)
  {
    JSFunctionRegister funcs[] = {
      { "setPosition", JSSetPosition },
      { "position", JSPosition },
      { "setSize", JSSetSize },
      { "size", JSSize },
      { "setMax", JSSetMax },
      { "max", JSMax },
      { "focussed", JSFocussed },
      { "setValues", JSSetValues },
      { "values", JSValues },
      { "scrollBy", JSScrollBy },
      { "clear", JSClear },
      { "setZ", JSSetZ },
      { "z", JSZIndex }
    };

    JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSSetPosition(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11ScrollArea* self = wrapper.GetPointer<D3D11ScrollArea>(args.This());

    if (wrapper.Check("NN") == true)
    {
      self->set_position(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSPosition(JS_ARGS args)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSSetSize(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11ScrollArea* self = wrapper.GetPointer<D3D11ScrollArea>(args.This());

    if (wrapper.Check("NN") == true)
    {
      self->set_size(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSSize(JS_ARGS args)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSSetMax(JS_ARGS args)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSMax(JS_ARGS args)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSFocussed(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11ScrollArea* self = wrapper.GetPointer<D3D11ScrollArea>(args.This());

    wrapper.ReturnValue<bool>(self->is_foccused());
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSSetValues(JS_ARGS args)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSValues(JS_ARGS args)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSScrollBy(JS_ARGS args)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSClear(JS_ARGS args)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSSetZ(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11ScrollArea* self = wrapper.GetPointer<D3D11ScrollArea>(args.This());

    if (wrapper.Check("NN") == true)
    {
      self->SetZ(wrapper.GetValue<float>(0, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSZIndex(JS_ARGS args)
  {

  }
}