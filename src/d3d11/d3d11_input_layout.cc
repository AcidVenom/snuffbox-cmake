#include "../d3d11/d3d11_input_layout.h"

namespace snuffbox
{
  //---------------------------------------------------------------------------------------------------------
  D3D11InputLayout::D3D11InputLayout() :
    input_layout_(nullptr),
    valid_(false)
  {

  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11InputLayout::Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3D10Blob* vs_buffer)
  {
    HRESULT result = S_OK;

    SNUFF_XASSERT(vs_buffer != nullptr, "You have to properly load the shader initially, fix the shader and restart", "D3D11InputLayout::Create");

    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    ID3D11Device* device = render_device->device();
    ID3D11DeviceContext* ctx = render_device->context();

    result = device->CreateInputLayout(&layout[0], static_cast<UINT>(layout.size()), vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), &input_layout_);
    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateInputLayout"), "D3D11InputLayout::Create");
    
    valid_ = true;
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11InputLayout::Set()
  {
    SNUFF_XASSERT(valid_ == true, "Tried to set an invalid input layout", "D3D11InputLayout::Set");

    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    ID3D11DeviceContext* ctx = render_device->context();
    ctx->IASetInputLayout(input_layout_);
  }

  //---------------------------------------------------------------------------------------------------------
  D3D11InputLayout::~D3D11InputLayout()
  {
    if (valid_ == false)
    {
      return;
    }

    SNUFF_SAFE_RELEASE(input_layout_, "D3D11InputLayout::~D3D11InputLayout::input_layout_");
  }
}