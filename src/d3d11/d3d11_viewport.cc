#include "../d3d11/d3d11_viewport.h"

namespace snuffbox
{
  //---------------------------------------------------------------------------------------------------------
  D3D11Viewport::D3D11Viewport() :
    x_(0.0f),
    y_(0.0f),
    width_(0.0f),
    height_(0.0f),
    valid_(false)
  {
    Adjust();
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11Viewport::Create(float x, float y, float w, float h)
  {
    x_ = x;
    y_ = y;
    width_ = w;
    height_ = h;

    Adjust();

    valid_ = true;
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11Viewport::Set()
  {
    if (valid_ == false)
    {
      SNUFF_LOG_WARNING("Tried to set an invalid viewport, rendering will fail");
      return;
    }

    D3D11RenderDevice::Instance()->context()->RSSetViewports(1, &viewport_);
  }

  //---------------------------------------------------------------------------------------------------------
  D3D11Viewport::~D3D11Viewport()
  {

  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11Viewport::Adjust()
  {
    viewport_.TopLeftX = x_;
    viewport_.TopLeftY = y_;
    viewport_.Width = width_;
    viewport_.Height = height_;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
  }
}