#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_render_queue.h"

#include "../application/logging.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11RenderTarget::D3D11RenderTarget(std::string name) :
		valid_(false),
		name_(name),
		buffer_(nullptr),
		view_(nullptr),
		resource_(nullptr)
	{
    
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderTarget::D3D11RenderTarget(JS_ARGS args) :
		valid_(false),
		buffer_(nullptr),
		view_(nullptr),
		resource_(nullptr)
	{
		JSWrapper wrapper(args);
		
		SNUFF_XASSERT(wrapper.Check("S"), "There was an attempt to create a render target without a name", "RenderTarget::RenderTarget");
		name_ = wrapper.GetValue<std::string>(0, "undefined");

		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		Create(RenderTargets::kRenderTarget, render_device->swap_chain(), render_device->device());

		render_device->AddRenderTarget(this);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::Create(D3D11RenderTarget::RenderTargets type, IDXGISwapChain* swap_chain, ID3D11Device* device)
	{
		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		HRESULT result = S_OK;

		if (type == RenderTargets::kBackBuffer)
		{
			name_ = "Backbuffer";
			result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&buffer_));

			SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "GetBuffer"), "D3D11RenderTarget::Create");

			result = device->CreateRenderTargetView(buffer_, NULL, &view_);

			SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateRenderTargetView"), "D3D11RenderTarget::Create");
		}
		else if (type == RenderTargets::kRenderTarget)
		{
			HRESULT result = S_OK;

			D3D11_TEXTURE2D_DESC desc;
			bool valid = render_device->back_buffer()->GetDescription(&desc);

			SNUFF_XASSERT(valid == true, "Tried to create a render target without a corresponding back buffer!", "D3D11RenderTarget::Create");

			desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

			result = device->CreateTexture2D(&desc, NULL, &buffer_);

			SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateTexture2D"), "D3D11RenderTarget::Create");

			D3D11_SHADER_RESOURCE_VIEW_DESC view_desc;

			view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			view_desc.Texture2D.MipLevels = 1;
			view_desc.Texture2D.MostDetailedMip = 0;

			result = device->CreateShaderResourceView(buffer_, &view_desc, &resource_);

			SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateShaderResourceView"), "D3D11RenderTarget::Create");

			result = device->CreateRenderTargetView(buffer_, NULL, &view_);

			SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateRenderTargetView"), "D3D11RenderTarget::Create");

      queue_ = AllocatedMemory::Instance().Construct<D3D11RenderQueue>();
		}
		else
		{
			return;
		}
	
		type_ = type;
		valid_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	bool D3D11RenderTarget::GetDescription(D3D11_RENDER_TARGET_VIEW_DESC* desc)
	{
		if (valid_ == false)
		{
			return false;
		}

		view_->GetDesc(desc);

		return true;
	}

	//---------------------------------------------------------------------------------------------------------
	bool D3D11RenderTarget::GetDescription(D3D11_TEXTURE2D_DESC* desc)
	{
		if (valid_ == false)
		{
			return false;
		}

		buffer_->GetDesc(desc);

		return true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::Clear(ID3D11DeviceContext* context)
	{
		if (valid_ == false)
		{
			SNUFF_LOG_WARNING("Render target with name '" + name_ + "' is invalid");
			return;
		}

		if (type_ == RenderTargets::kBackBuffer)
		{
			context->ClearRenderTargetView(view_, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		}
		else if (type_ == RenderTargets::kRenderTarget)
		{
			context->ClearRenderTargetView(view_, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::Set(ID3D11DeviceContext* context, ID3D11DepthStencilView* depth_stencil)
	{
		if (valid_ == false)
		{
			SNUFF_LOG_WARNING("Render target with name '" + name_ + "' is invalid, rendering will fail");
			return;
		}

    context->OMSetRenderTargets(1, &view_, depth_stencil);
	}

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderTarget::Draw(ID3D11DeviceContext* context)
  {
    queue_->Draw(context);
  }

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::Release()
	{
		if (valid_ == false)
		{
			return;
		}
		SNUFF_SAFE_RELEASE(buffer_, "D3D11RenderTarget::~D3D11RenderTarget::buffer_");
		SNUFF_SAFE_RELEASE(view_, "D3D11RenderTarget::~D3D11RenderTarget::view_");

		if (type_ == RenderTargets::kRenderTarget)
		{
			SNUFF_SAFE_RELEASE(resource_, "D3D11RenderTarget::~D3D11RenderTarget::resource_");
		}

		valid_ = false;
	}

	//---------------------------------------------------------------------------------------------------------
	const std::string& D3D11RenderTarget::name() const
	{
		return name_;
	}

  //---------------------------------------------------------------------------------------------------------
  ID3D11ShaderResourceView* D3D11RenderTarget::resource()
  {
    return resource_;
  }

  //---------------------------------------------------------------------------------------------------------
  D3D11RenderQueue* D3D11RenderTarget::queue()
  {
    return queue_.get();
  }

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderTarget::~D3D11RenderTarget()
	{
		Release();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::RegisterJS(JS_CONSTRUCTABLE obj)
	{

	}
}