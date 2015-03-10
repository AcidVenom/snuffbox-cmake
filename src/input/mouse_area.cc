#include "../input/mouse_area.h"
#include "../input/mouse.h"

#include "../d3d11/elements/d3d11_widget_element.h"
#include "../d3d11/d3d11_render_settings.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  MouseArea::MouseArea() :
    metrics_({1.0f, 1.0f, 0.0f, 0.0f }),
    parent_(nullptr)
  {

  }

  //-------------------------------------------------------------------------------------------
  MouseArea::MouseArea(JS_ARGS args) :
    metrics_({1.0f, 1.0f, 0.0f, 0.0f }),
    parent_(nullptr)
  {
    JSWrapper wrapper(args);

    if (wrapper.Check("O") == true)
    {
      parent_ = wrapper.GetPointer<D3D11Widget>(0);
      parent_->AddMouseArea(this);
      Mouse::Instance()->AddMouseArea(this);
    }
  }

  //-------------------------------------------------------------------------------------------
  void MouseArea::Check(Mouse* mouse)
  {
    if (parent_ == nullptr)
    {
      return;
    }

    const XMFLOAT2& res = D3D11RenderSettings::Instance()->resolution();

    XMMATRIX proj = XMMatrixOrthographicRH(res.x, res.y, 0.0f, 1.0f);
    XMMATRIX wp = parent_->world_matrix() * proj;

    Mouse::float2 screen = mouse->Position(Mouse::MousePosition::kScreen);

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

    SNUFF_LOG_INFO(std::to_string(InBounds(p1, p2, p3)));
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
  void MouseArea::set_parent(D3D11Widget* parent)
  {
    parent_ = parent;
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

  }
}