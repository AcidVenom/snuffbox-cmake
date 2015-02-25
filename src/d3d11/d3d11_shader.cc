#include "../d3d11/d3d11_shader.h"
#include "../content/content_manager.h"
#include "../application/game.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11Shader::D3D11Shader() :
    Content(ContentTypes::kShader),
    vs_buffer_(nullptr),
    ps_buffer_(nullptr),
    vs_(nullptr),
    ps_(nullptr),
    valid_(false)
  {
  
  }

  //-------------------------------------------------------------------------------------------
	void D3D11Shader::Load(const std::string& path)
  {
    if (vs_buffer_ != nullptr)
    {
      SNUFF_SAFE_RELEASE(vs_buffer_, "D3D11Shader::Load::vs_buffer_");
      SNUFF_SAFE_RELEASE(ps_buffer_, "D3D11Shader::Load::ps_buffer_");
    }

    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    std::string full_path = Game::Instance()->path() + "/" + path;

    ID3D10Blob* errors = nullptr;
    HRESULT result = S_OK;

    result = D3DX10CompileFromFileA(full_path.c_str(), 0, 0, "VS", "vs_4_0", D3D10_SHADER_PACK_MATRIX_ROW_MAJOR, 0, 0, &vs_buffer_, &errors, 0);
    if (errors != nullptr)
    {
      SNUFF_LOG_ERROR(static_cast<const char*>(errors->GetBufferPointer()));
      errors->Release();
      return;
    }

    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "D3DX10CompileFromFileA::VS"), "D3D11Shader::Load::" + path);

    result = D3DX10CompileFromFileA(full_path.c_str(), 0, 0, "PS", "ps_4_0", D3D10_SHADER_PACK_MATRIX_ROW_MAJOR, 0, 0, &ps_buffer_, &errors, 0);
    if (errors != nullptr)
    {
      SNUFF_LOG_ERROR(static_cast<const char*>(errors->GetBufferPointer()));
      errors->Release();
      return;
    }

    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "D3DX10CompileFromFileA::PS"), "D3D11Shader::Load::" + path);

    ID3D11Device* device = render_device->device();

    if (valid_ == true)
    {
      SNUFF_SAFE_RELEASE(vs_, "D3D11Shader::Load::vs_");
      SNUFF_SAFE_RELEASE(ps_, "D3D11Shader::Load::ps_");
    }

    result = device->CreateVertexShader(vs_buffer_->GetBufferPointer(), vs_buffer_->GetBufferSize(), NULL, &vs_);
    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateVertexShader"), "D3D11Shader::Load::" + path);

    result = device->CreatePixelShader(ps_buffer_->GetBufferPointer(), ps_buffer_->GetBufferSize(), NULL, &ps_);
    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreatePixelShader"), "D3D11Shader::Load::" + path);

    valid_ = true;
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Shader::Set()
  {
    SNUFF_XASSERT(valid_ == true, "Tried to set invalid shaders", "D3D11Shader::Set");

    ID3D11DeviceContext* ctx = D3D11RenderDevice::Instance()->context();

    ctx->VSSetShader(vs_, NULL, 0);
    ctx->PSSetShader(ps_, NULL, 0);
  }

  //-------------------------------------------------------------------------------------------
  ID3D10Blob* D3D11Shader::vs_buffer()
  {
    return vs_buffer_;
  }

  //-------------------------------------------------------------------------------------------
  ID3D10Blob* D3D11Shader::ps_buffer()
  {
    return ps_buffer_;
  }

  //-------------------------------------------------------------------------------------------
  D3D11Shader::~D3D11Shader()
  {
    if (valid_ == false)
    {
      return;
    }

    SNUFF_SAFE_RELEASE(vs_buffer_, "D3D11Shader::~D3D11Shader::vs_buffer_");
    SNUFF_SAFE_RELEASE(ps_buffer_, "D3D11Shader::~D3D11Shader::ps_buffer_");
    SNUFF_SAFE_RELEASE(vs_, "D3D11Shader::~D3D11Shader::vs_");
    SNUFF_SAFE_RELEASE(ps_, "D3D11Shader::~D3D11Shader::ps_");
  }
}