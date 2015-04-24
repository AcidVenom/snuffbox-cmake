#include "../d3d11/d3d11_scroll_area.h"
#include "../d3d11/elements/d3d11_widget_element.h"
#include "../input/mouse_area.h"
#include "../d3d11/d3d11_render_queue.h"
#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_render_settings.h"
#include "../d3d11/d3d11_viewport.h"

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
    JSWrapper wrapper(args);

    if (wrapper.Check("S") == true)
    {
      target_ = D3D11RenderDevice::Instance()->GetTarget(wrapper.GetValue<std::string>(0, "undefined"));
    }

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

    if (target_ != nullptr)
    {
      target_->AddScrollArea(this);
    }
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
  void D3D11ScrollArea::Draw(ID3D11DeviceContext* context)
  {
    D3D11RenderDevice* device = D3D11RenderDevice::Instance();

    const D3D11Viewport* vp = device->viewport_render_target();
    const XMFLOAT2& res = D3D11RenderSettings::Instance()->resolution();

    float ratio_w = vp->width() / res.x;
    float ratio_h = vp->height() / res.y;

    D3D11_RECT rect;
    float px = vp->x() + vp->width() / 2.0f + position_.x * ratio_w;
    float py = vp->height() / 2.0f - position_.y * ratio_h;

    rect.left = static_cast<LONG>(px);
    rect.right = static_cast<LONG>(px + size_.x * ratio_w);
    rect.top = static_cast<LONG>(py - size_.y * ratio_h);
    rect.bottom = static_cast<LONG>(py);

    device->context()->RSSetScissorRects(1, &rect);
    children_->Draw(context);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::AddChild(D3D11Widget* child)
  {
    if (child == nullptr)
    {
      return;
    }

    children_->Add(child);

    D3D11RenderElement* top = child;
    D3D11RenderElement* parent = child->parent();
    while (parent != nullptr)
    {
      children_->Add(parent);
      parent = parent->parent();
      
      if (parent != nullptr)
      {
        top = parent;
      }
    }

    top->set_parent(root_.get());

    bool found = false;
    for (unsigned int i = 0; i < top_level_.size(); ++i)
    {
      if (top_level_.at(i) == top)
      {
        found = true;
        break;
      }
    }

    if (found == false)
    {
      top_level_.push_back(top);
    }

    RecursiveAdd(child);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::RecursiveAdd(D3D11RenderElement* head)
  {
    const std::vector<D3D11RenderElement*>& children = head->children();
    D3D11RenderElement* found_child;
    for (unsigned int i = 0; i < children.size(); ++i)
    {
      found_child = children.at(i);

      if (found_child != nullptr)
      {
        children_->Add(found_child);
        RecursiveAdd(found_child);
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::RemoveChild(D3D11Widget* child)
  {
    if (child == nullptr)
    {
      return;
    }

    children_->Remove(child);

    D3D11RenderElement* parent = child->parent();
    while (parent != nullptr)
    {
      children_->Remove(parent);
      parent = parent->parent();
    }

    for (unsigned int i = 0; i < top_level_.size(); ++i)
    {
      if (top_level_.at(i) == child)
      {
        child->set_parent(nullptr);
        top_level_.erase(top_level_.begin() + i);
        break;
      }
    }

    RecursiveRemove(child);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::RecursiveRemove(D3D11RenderElement* head)
  {
    const std::vector<D3D11RenderElement*>& children = head->children();
    D3D11RenderElement* found_child;
    for (unsigned int i = 0; i < children.size(); ++i)
    {
      found_child = children.at(i);

      if (found_child != nullptr)
      {
        children_->Remove(found_child);
        RecursiveRemove(found_child);
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::SetValues(const float& x, const float& y)
  {
    root_->set_translation(std::max(std::min(-x, 0.0f), -max_.x), std::max(std::min(-y, 0.0f), -max_.y), z());
  }

  //-------------------------------------------------------------------------------------------
  XMFLOAT2 D3D11ScrollArea::Values()
  {
    XMFLOAT2 ret;
    XMStoreFloat2(&ret, root_->translation() * -1);

    return ret;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::ScrollBy(const float& x, const float& y)
  {

    XMVECTOR t = root_->translation() - XMLoadFloat2(&XMFLOAT2(x, y));
    XMFLOAT2 trans;
    XMStoreFloat2(&trans, t);

    trans.x = std::max(std::min(trans.x, 0.0f), -max_.x);
    trans.y = std::max(std::min(trans.y, 0.0f), -max_.y);

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
  void D3D11ScrollArea::set_target(D3D11RenderTarget* target)
  {
    target_ = target;
  }

  //-------------------------------------------------------------------------------------------
  D3D11ScrollArea::~D3D11ScrollArea()
  {
    for (unsigned int i = 0; i < top_level_.size(); ++i)
    {
      top_level_.at(i)->set_parent(nullptr);
    }

    children_->Clear();
    mouse_area_->set_scroll_area(nullptr);

    if (target_ != nullptr)
    {
      target_->RemoveScrollArea(this);
    }
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
      { "z", JSZIndex },
      { "addChild", JSAddChild },
      { "removeChild", JSRemoveChild }
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
    JSWrapper wrapper(args);
    D3D11ScrollArea* self = wrapper.GetPointer<D3D11ScrollArea>(args.This());

    if (wrapper.Check("NN") == true)
    {
      self->set_max(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f));
    }
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
    JSWrapper wrapper(args);
    D3D11ScrollArea* self = wrapper.GetPointer<D3D11ScrollArea>(args.This());

    if (wrapper.Check("NN") == true)
    {
      self->ScrollBy(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f));
    }
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

    if (wrapper.Check("N") == true)
    {
      self->SetZ(wrapper.GetValue<float>(0, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSZIndex(JS_ARGS args)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSAddChild(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11ScrollArea* self = wrapper.GetPointer<D3D11ScrollArea>(args.This());

    if (wrapper.Check("O") == true)
    {
      self->AddChild(wrapper.GetPointer<D3D11Widget>(0));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11ScrollArea::JSRemoveChild(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11ScrollArea* self = wrapper.GetPointer<D3D11ScrollArea>(args.This());

    if (wrapper.Check("O") == true)
    {
      self->RemoveChild(wrapper.GetPointer<D3D11Widget>(0));
    }
  }
}