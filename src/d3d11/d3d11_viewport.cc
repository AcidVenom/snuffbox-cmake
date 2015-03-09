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
	void D3D11Viewport::Create(const float& x, const float& y, const float& w, const float& h)
  {
    x_ = x;
    y_ = y;
    width_ = w;
    height_ = h;

    Adjust();
  }

	//---------------------------------------------------------------------------------------------------------
	void D3D11Viewport::SetToAspectRatio(const float& width, const float& height, const float& r_width, const float& r_height)
	{
		float target_aspect = r_width / r_height;
		float current_aspect = width / height;

		float w = width;
		float h = height;

		if (target_aspect > current_aspect) {
			w = width;
			h = w / target_aspect;
		}
		else 
		{
			h = height;
			w = h * target_aspect;
		}

		x_ = width / 2 - w / 2;
		y_ = height / 2 - h / 2;

		width_ = w;
		height_ = h;

		Adjust();
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
	const float& D3D11Viewport::x() const
	{
		return viewport_.TopLeftX;
	}

	//---------------------------------------------------------------------------------------------------------
	const float& D3D11Viewport::y() const
	{
		return viewport_.TopLeftY;
	}

	//---------------------------------------------------------------------------------------------------------
	const float& D3D11Viewport::width() const
	{
		return viewport_.Width;
	}

	//---------------------------------------------------------------------------------------------------------
	const float& D3D11Viewport::height() const
	{
		return viewport_.Height;
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

		valid_ = true;
  }
}