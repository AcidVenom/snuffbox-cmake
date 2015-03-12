#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_render_queue.h"
#include "../d3d11/d3d11_effect.h"
#include "../d3d11/d3d11_uniforms.h"
#include "../d3d11/d3d11_viewport.h"
#include "../d3d11/d3d11_render_settings.h"

#include "../content/content_manager.h"

#include "../application/logging.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11RenderTarget::D3D11RenderTarget(std::string name) :
		valid_(false),
		name_(name),
		buffer_(nullptr),
		view_(nullptr),
		resource_(nullptr),
		post_processing_(nullptr),
		technique_("Default")
	{
    
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderTarget::D3D11RenderTarget(JS_ARGS args) :
		valid_(false),
		buffer_(nullptr),
		view_(nullptr),
		resource_(nullptr),
		post_processing_(nullptr),
		technique_("Default")
	{
		JSWrapper wrapper(args);
		
		SNUFF_XASSERT(wrapper.Check("S"), "There was an attempt to create a render target without a name", "RenderTarget::RenderTarget");
		name_ = wrapper.GetValue<std::string>(0, "undefined");

		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		Create(RenderTargets::kRenderTarget, render_device->swap_chain(), render_device->device());

		render_device->AddRenderTarget(this);
    viewport_ = AllocatedMemory::Instance().Construct<D3D11Viewport>();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::Create(D3D11RenderTarget::RenderTargets type, IDXGISwapChain* swap_chain, ID3D11Device* device)
	{
		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		HRESULT result = S_OK;

		if (buffer_ != nullptr)
		{
			SNUFF_SAFE_RELEASE(buffer_, "D3D11RenderTarget::Create::buffer_");
		}

		if (view_ != nullptr)
		{
			SNUFF_SAFE_RELEASE(view_, "D3D11RenderTarget::Create::view_");
		}

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
			post_processing_ = D3D11RenderDevice::Instance()->default_post_processing();

			if (resource_ != nullptr)
			{
				SNUFF_SAFE_RELEASE(resource_, "D3D11RenderTarget::Create::resource_");
			}
			else
			{
				queue_ = AllocatedMemory::Instance().Construct<D3D11RenderQueue>();
			}

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
		}
		else
		{
			return;
		}
		
		uniforms_ = AllocatedMemory::Instance().Construct<D3D11Uniforms>();

    if (mrts_.size() == 0)
    {
      mrts_.push_back(this);
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
      for (unsigned int i = 0; i < mrts_.size(); ++i)
      {
        context->ClearRenderTargetView(mrts_.at(i)->view(), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
      }
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

    ID3D11RenderTargetView** views = new ID3D11RenderTargetView*[mrts_.size()];

    for (unsigned int i = 0; i < mrts_.size(); ++i)
    {
      views[i] = mrts_.at(i)->view();
    }

    context->OMSetRenderTargets(static_cast<UINT>(mrts_.size()), views, depth_stencil);

    delete[] views;
	}

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderTarget::Draw(ID3D11DeviceContext* context)
  {
    queue_->Draw(context);
  }

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::Clear()
	{
		queue_->Clear();
	}

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderTarget::SetViewport(const float& x, const float& y, const float& w, const float& h)
  {
    const XMFLOAT2& res = D3D11RenderSettings::Instance()->resolution();
    viewport_->Create(x, res.y / 2.0f - y, w, h);
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderTarget::AddMultiTarget(D3D11RenderTarget* multi)
  {
    if (multi == nullptr)
    {
      SNUFF_LOG_ERROR("Attempted to add an invalid multi-target to the render target with name '" + name_ + "'");
      return;
    }
    mrts_.push_back(multi);
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderTarget::SetResources(ID3D11DeviceContext* context, ID3D11ShaderResourceView* depth_stencil_resource)
  {
    ID3D11ShaderResourceView** resources = new ID3D11ShaderResourceView*[mrts_.size() + 1];

    for (unsigned int i = 0; i < mrts_.size(); ++i)
    {
      resources[i] = mrts_.at(i)->resource();
    }

    resources[mrts_.size()] = depth_stencil_resource;

    context->PSSetShaderResources(0, static_cast<UINT>(mrts_.size() + 1), resources);

    delete[] resources;
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
  ID3D11RenderTargetView* D3D11RenderTarget::view()
  {
    return view_;
  }

  //---------------------------------------------------------------------------------------------------------
  D3D11RenderQueue* D3D11RenderTarget::queue()
  {
    return queue_.get();
  }

	//---------------------------------------------------------------------------------------------------------
	D3D11Effect* D3D11RenderTarget::post_processing()
	{
		return post_processing_;
	}

	//---------------------------------------------------------------------------------------------------------
	const std::string& D3D11RenderTarget::technique()
	{
		return technique_;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Uniforms* D3D11RenderTarget::uniforms()
	{
		return uniforms_.get();
	}

  //---------------------------------------------------------------------------------------------------------
  D3D11Viewport* D3D11RenderTarget::viewport()
  {
    return viewport_.get();
  }

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::set_post_processing(const std::string& path)
	{
		post_processing_ = ContentManager::Instance()->Get<D3D11Effect>(path);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::set_post_processing(D3D11Effect* effect)
	{
		post_processing_ = effect;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::set_technique(const std::string& technique)
	{
		technique_ = technique;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderTarget::~D3D11RenderTarget()
	{
		Release();
    D3D11RenderDevice::Instance()->RemoveTarget(this);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::RegisterJS(JS_CONSTRUCTABLE obj)
	{
		JSFunctionRegister funcs[] = {
			{ "clear", JSClear },
			{ "setPostProcessing", JSSetPostProcessing },
			{ "setTechnique", JSSetTechnique },
			{ "setUniform", JSSetUniform },
      { "setViewport", JSSetViewport },
      { "addMultiTarget", JSAddMultiTarget }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::JSClear(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderTarget* self = wrapper.GetPointer<D3D11RenderTarget>(args.This());

		self->Clear();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::JSSetPostProcessing(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderTarget* self = wrapper.GetPointer<D3D11RenderTarget>(args.This());

		if (wrapper.Check("S"))
		{
			self->set_post_processing(wrapper.GetValue<std::string>(0, "undefined"));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::JSSetTechnique(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderTarget* self = wrapper.GetPointer<D3D11RenderTarget>(args.This());

		if (wrapper.Check("S"))
		{
			self->set_technique(wrapper.GetValue<std::string>(0, "undefined"));
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderTarget::JSSetUniform(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderTarget* self = wrapper.GetPointer<D3D11RenderTarget>(args.This());

		if (wrapper.Check("NSN") == true)
		{
			float buffer[4] = {
				wrapper.GetValue<float>(2, 0.0f),
				wrapper.GetValue<float>(3, 0.0f),
				wrapper.GetValue<float>(4, 0.0f),
				wrapper.GetValue<float>(5, 0.0f),
			};

			self->uniforms()->SetUniform(static_cast<D3D11Uniforms::UniformTypes>(
				wrapper.GetValue<int>(0, 1)),
				wrapper.GetValue<std::string>(1, "undefined"),
				buffer);
		}
	}

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderTarget::JSSetViewport(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderTarget* self = wrapper.GetPointer<D3D11RenderTarget>(args.This());

    if (wrapper.Check("NNNN") == true)
    {
      self->SetViewport(
        wrapper.GetValue<float>(0, 0.0f), 
        wrapper.GetValue<float>(1, 0.0f), 
        wrapper.GetValue<float>(2, 640.0f), 
        wrapper.GetValue<float>(3, 480.0f));
    }
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderTarget::JSAddMultiTarget(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderTarget* self = wrapper.GetPointer<D3D11RenderTarget>(args.This());

    if (wrapper.Check("O") == true)
    {
      self->AddMultiTarget(wrapper.GetPointer<D3D11RenderTarget>(0));
    }
  }
}