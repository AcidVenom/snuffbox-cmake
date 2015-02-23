#include "../d3d11/d3d11_sampler_state.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11SamplerState::D3D11SamplerState(D3D11SamplerState::SamplerTypes type) :
    sampler_state_(nullptr)
  {
    D3D11_SAMPLER_DESC desc;

    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.BorderColor[0] = 0.0f;
    desc.BorderColor[1] = 0.0f;
    desc.BorderColor[2] = 0.0f;
    desc.BorderColor[3] = 0.0f;
    desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    desc.MinLOD = 0.0f;
    desc.MaxLOD = D3D11_FLOAT32_MAX;
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = 0;

    switch (type)
    {
    case SamplerTypes::kLinear:
      desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      break;
    case SamplerTypes::kPoint:
      desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
      break;
    }

    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    ID3D11Device* device = render_device->device();
    HRESULT result = device->CreateSamplerState(&desc, &sampler_state_);

    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateSamplerState"), "D3D11SamplerState::D3D11SamplerState");
  }

  //-------------------------------------------------------------------------------------------
  void D3D11SamplerState::Set()
  {
    ID3D11DeviceContext* ctx = D3D11RenderDevice::Instance()->context();
    ID3D11SamplerState* state = nullptr;
    ctx->PSGetSamplers(0, 1, &state);

    if (state != sampler_state_)
    {
      ctx->PSSetSamplers(0, 1, &sampler_state_);
    }
  }

  //-------------------------------------------------------------------------------------------
  D3D11SamplerState::~D3D11SamplerState()
  {
    SNUFF_SAFE_RELEASE(sampler_state_, "D3D11SamplerState::~D3D11SamplerState");
  }
}