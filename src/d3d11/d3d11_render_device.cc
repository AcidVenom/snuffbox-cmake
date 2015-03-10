#include "../d3d11/d3d11_render_device.h"
#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_vertex_buffer.h"
#include "../d3d11/d3d11_input_layout.h"
#include "../d3d11/d3d11_shader.h"
#include "../d3d11/d3d11_viewport.h"
#include "../d3d11/d3d11_render_settings.h"
#include "../d3d11/d3d11_sampler_state.h"
#include "../d3d11/d3d11_constant_buffer.h"
#include "../d3d11/d3d11_camera.h"
#include "../d3d11/d3d11_blend_state.h"
#include "../d3d11/d3d11_depth_state.h"
#include "../d3d11/d3d11_effect.h"
#include "../d3d11/d3d11_lighting.h"
#include "../d3d11/d3d11_texture.h"
#include "../d3d11/d3d11_material.h"
#include "../d3d11/d3d11_line.h"
#include "../d3d11/d3d11_uniforms.h"

#include "../application/game.h"
#include "../platform/platform_window.h"

#include "../cvar/cvar.h"
#include "../content/content_manager.h"
#include "../io/io_manager.h"

#include "../d3d11/shaders/d3d11_base_shader.h"
#include "../d3d11/shaders/d3d11_post_processing_shader.h"
#include "../d3d11/shaders/d3d11_ui_shader.h"
#include "../d3d11/shaders/d3d11_text_shader.h"

#include <comdef.h>

#undef max

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
	D3D11RenderDevice::D3D11RenderDevice() :
		ready_(false),
		adapter_(nullptr),
		swap_chain_(nullptr),
		device_(nullptr),
		context_(nullptr),
    vertex_buffer_type_(-1),
		camera_(nullptr),
		depth_stencil_view_(nullptr),
		current_shader_(nullptr),
		lighting_(nullptr),
		current_target_(nullptr)
	{

	}

  //-------------------------------------------------------------------------------------------
	D3D11RenderDevice* D3D11RenderDevice::Instance()
	{
		static SharedPtr<D3D11RenderDevice> render_device = AllocatedMemory::Instance().Construct<D3D11RenderDevice>();
		return render_device.get();
	}

  //-------------------------------------------------------------------------------------------
	std::string D3D11RenderDevice::HRToString(const HRESULT& hr, const std::string& context)
	{
		_com_error error(hr);
		return "(" + context + ") " + error.ErrorMessage();
	}

  //-------------------------------------------------------------------------------------------
  bool D3D11RenderDevice::Initialise()
  {
    CreateDevice();
    CreateBackBuffer();
    CreateScreenQuad();
    LoadBaseShaders();
    CreateInputLayout();
    CreateBaseViewport();

    sampler_linear_ = AllocatedMemory::Instance().Construct<D3D11SamplerState>(D3D11SamplerState::SamplerTypes::kLinear);
		sampler_anisotropic_ = AllocatedMemory::Instance().Construct<D3D11SamplerState>(D3D11SamplerState::SamplerTypes::kAnisotropic);
		sampler_point_ = AllocatedMemory::Instance().Construct<D3D11SamplerState>(D3D11SamplerState::SamplerTypes::kPoint);

		set_textures_[0] = nullptr;
		set_textures_[1] = nullptr;
		set_textures_[2] = nullptr;

    sampler_linear_->Set();

    default_blend_state_ = AllocatedMemory::Instance().Construct<D3D11BlendState>();

		default_blend_state_->CreateFromJson(std::string("{}"));
    default_blend_state_->Set();

		default_depth_state_ = AllocatedMemory::Instance().Construct<D3D11DepthState>();

		default_depth_state_->CreateFromJson(std::string("{}"));
		default_depth_state_->Set();

    default_texture_ = AllocatedMemory::Instance().Construct<D3D11Texture>();
    default_texture_->Create(1, 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), sizeof(D3DXCOLOR));
    default_texture_->Validate();

    CreateDefaultMaterial();
		CreateDepthStencilView();

		lighting_ = D3D11Lighting::Instance();
		line_ = D3D11Line::Instance();
		constant_buffer_ = D3D11ConstantBuffer::Instance();
		constant_buffer_->Create();

		SNUFF_LOG_SUCCESS("Succesfully initialised the Direct3D 11 render device");

		ready_ = true;
		return true;
	}

  //-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::CreateDevice()
	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&swap_chain_, sizeof(IDXGISwapChain));
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

		Window* window = Game::Instance()->window();
		desc.BufferCount = 1;
		desc.BufferDesc.Width = window->width();
		desc.BufferDesc.Height = window->height();
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = reinterpret_cast<HWND>(window->handle());
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = TRUE;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		HRESULT result = S_OK;

		D3D_FEATURE_LEVEL feature_levels_requested = D3D_FEATURE_LEVEL_10_0;
		D3D_FEATURE_LEVEL feature_levels_supported;

		UINT device_flags = 0;

#ifdef _DEBUG
		device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		FindAdapter();

		if (FAILED(result = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			device_flags,
			&feature_levels_requested,
			1,
			D3D11_SDK_VERSION,
			&desc,
			&swap_chain_,
			&device_,
			&feature_levels_supported,
			&context_
			)))
		{
			SNUFF_ASSERT(HRToString(result, "D3D11CreateDeviceAndSwapChain"), "D3D11DisplayDevice::CreateDevice");
		}

    context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::CreateBackBuffer()
  {
    back_buffer_ = AllocatedMemory::Instance().Construct<D3D11RenderTarget>("Backbuffer");
    back_buffer_->Create(D3D11RenderTarget::RenderTargets::kBackBuffer, swap_chain_, device_);

    back_buffer_->Set(context_);
  }
  
  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::CreateScreenQuad()
  {
		std::vector<int> indices = {
			0, 1, 2, 3
		};
    screen_quad_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kScreen);
		screen_quad_->Create({
			{ XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }
		}, 
		indices);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::LoadBaseShaders()
  {
		IOManager* io = IOManager::Instance();

		if (io->DirectoryExists("shaders") == false)
		{
			io->CreateDir("shaders");
		}

		bool exists = IOManager::Instance()->Exists("shaders/base.fx");
		if (exists == false)
		{
			IOManager::Instance()->Write("shaders/base.fx", D3D11_BASE_SHADER);
		}

		exists = IOManager::Instance()->Exists("shaders/post_processing.fx");
		if (exists == false)
		{
			IOManager::Instance()->Write("shaders/post_processing.fx", D3D11_POST_PROCESSING_SHADER);
		}

		exists = IOManager::Instance()->Exists("shaders/ui.fx");
		if (exists == false)
		{
			IOManager::Instance()->Write("shaders/ui.fx", D3D11_UI_SHADER);
		}

		exists = IOManager::Instance()->Exists("shaders/text.fx");
		if (exists == false)
		{
			IOManager::Instance()->Write("shaders/text.fx", D3D11_TEXT_SHADER);
		}

    ContentManager* content_manager = ContentManager::Instance();
    content_manager->Notify(ContentManager::Events::kLoad, ContentTypes::kShader, "shaders/base.fx");
    content_manager->Notify(ContentManager::Events::kLoad, ContentTypes::kShader, "shaders/post_processing.fx");
    content_manager->Notify(ContentManager::Events::kLoad, ContentTypes::kShader, "shaders/ui.fx");
    content_manager->Notify(ContentManager::Events::kLoad, ContentTypes::kShader, "shaders/text.fx");
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::CreateInputLayout()
  {
    D3D11Shader* shader = ContentManager::Instance()->Get<D3D11Shader>("shaders/base.fx");
    if (shader == nullptr)
    {
      return;
    }

    input_layout_ = AllocatedMemory::Instance().Construct<D3D11InputLayout>();
    input_layout_->Create({
      { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    }, shader->vs_buffer());

    input_layout_->Set();
    shader->Set();
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::CreateBaseViewport()
  {
    viewport_ = AllocatedMemory::Instance().Construct<D3D11Viewport>();
    
		const XMFLOAT2& resolution = D3D11RenderSettings::Instance()->resolution();
		Window* window = Game::Instance()->window();
		viewport_->SetToAspectRatio(
			static_cast<float>(window->width()), 
			static_cast<float>(window->height()), 
			resolution.x, 
			resolution.y);

    viewport_render_target_ = AllocatedMemory::Instance().Construct<D3D11Viewport>();
    viewport_render_target_->Create(
      0,
      0,
      static_cast<float>(window->width()),
      static_cast<float>(window->height())
      );
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::CreateDefaultMaterial()
  {
    default_effect_ = AllocatedMemory::Instance().Construct<D3D11Effect>();

    D3D11Effect::Technique technique;
    D3D11Effect::Pass pass;

    ContentManager* content_manager = ContentManager::Instance();
    pass.sampling = D3D11SamplerState::SamplerTypes::kLinear;
    pass.shader = content_manager->Get<D3D11Shader>("shaders/base.fx");

    technique.name = "Default";
    technique.passes = { pass };

    default_effect_->AddTechnique(technique);

    pass.shader = content_manager->Get<D3D11Shader>("shaders/ui.fx");

    technique.name = "UI";
    technique.passes = { pass };

    default_effect_->AddTechnique(technique);

    pass.sampling = D3D11SamplerState::SamplerTypes::kPoint;
    pass.shader = content_manager->Get<D3D11Shader>("shaders/text.fx");
		pass.blend_state = AllocatedMemory::Instance().Construct<D3D11BlendState>();

		pass.blend_state->CreateFromJson(std::string("{\
			\"SrcBlend\" : \"SrcAlpha\",\
			\"DestBlend\" : \"InvSrcAlpha\",\
			\"SrcBlendAlpha\" : \"InvDestAlpha\",\
			\"DestBlendAlpha\" : \"One\"\
	}"));

    technique.name = "Text";
    technique.passes = { pass };

    default_effect_->AddTechnique(technique);

    default_effect_->Validate();

    default_material_ = AllocatedMemory::Instance().Construct<D3D11Material>();
    default_material_->set_textures(default_texture_.get());
    default_material_->set_effect(default_effect_.get());

    D3D11Material::Attributes& attributes = default_material_->attributes();
    attributes.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    attributes.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    attributes.emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    attributes.normal_scale = 0.0f;
    attributes.reflectivity = 0.0f;
    attributes.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    attributes.specular_intensity = 0.0f;

    default_material_->Validate();

		default_post_processing_ = AllocatedMemory::Instance().Construct<D3D11Effect>();
		
		pass.sampling = D3D11SamplerState::SamplerTypes::kLinear;
		pass.shader = content_manager->Get<D3D11Shader>("shaders/post_processing.fx");

		technique.name = "Default";
		technique.passes = { pass };

		default_post_processing_->AddTechnique(technique);
		default_post_processing_->Validate();
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::CreateDepthStencilView()
	{
		HRESULT result = S_OK;

		D3D11_TEXTURE2D_DESC tex_desc;
		DXGI_SWAP_CHAIN_DESC sc_desc;
		swap_chain_->GetDesc(&sc_desc);

		tex_desc.Width = sc_desc.BufferDesc.Width;
		tex_desc.Height = sc_desc.BufferDesc.Height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		result = device_->CreateTexture2D(&tex_desc, NULL, &depth_stencil_buffer_);
		SNUFF_XASSERT(result == S_OK, HRToString(result, "CreateTexture2D"), "D3D11RenderDevice::CreateDepthStencilView");
		result = device_->CreateDepthStencilView(depth_stencil_buffer_, NULL, &depth_stencil_view_);
		SNUFF_XASSERT(result == S_OK, HRToString(result, "CreateDepthStencilView"), "D3D11RenderDevice::CreateDepthStencilView");
	}

  //-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::FindAdapter()
	{
		IDXGIFactory* factory = NULL;

		HRESULT hr = S_OK;

		if (FAILED(hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
		{
			SNUFF_ASSERT(HRToString(hr, "CreateDXGIFactory"), "D3D11RenderDevice::FindAdapter");
		}

		DXGI_ADAPTER_DESC desc;

		hr = factory->EnumAdapters(0, &adapter_);
		SNUFF_XASSERT(hr == S_OK, HRToString(hr, "EnumAdapters"), "D3D11RenderDevice::FindAdapter");
		
		hr = adapter_->GetDesc(&desc);
		SNUFF_XASSERT(hr == S_OK, HRToString(hr, "GetDesc"), "D3D11RenderDevice::FindAdapter");

		int vram = static_cast<int>(desc.DedicatedVideoMemory / 1024 / 1024);
		int shared_memory = static_cast<int>(desc.SharedSystemMemory / 1024 / 1024);
		WCHAR* adapter_name = desc.Description;

		_bstr_t name = adapter_name;

		std::string result = "\n\nDefault display adapter: ";

		result += "\n\tVideo card name: " + name + "\n";
		result += "\tVirtual memory: " + std::to_string(vram) + "\n";
		result += "\tShared system memory: " + std::to_string(shared_memory);

		result += "\n";

		if (factory)
		{
			factory->Release();
		}

		SNUFF_LOG_INFO(result);
	}

  //-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::Draw()
	{
    if (input_layout_ == nullptr || camera_ == nullptr)
    {
      return;
    }

		lighting_->Update(constant_buffer_);
		
		back_buffer_->Clear(context_);
		context_->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    for (std::map<std::string, D3D11RenderTarget*>::iterator it = render_targets_.begin(); it != render_targets_.end(); ++it)
    {
			MapGlobalBuffer();
      DrawRenderTarget(it->second);
    }

    ID3D11ShaderResourceView *const null_resource[1] = { NULL };
    context_->PSSetShaderResources(0, 1, null_resource);

		swap_chain_->Present(D3D11RenderSettings::Instance()->vsync(), 0);

		camera_ = nullptr;
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::DrawRenderTarget(D3D11RenderTarget* target)
  {
		current_target_ = target;

    viewport_render_target_->Set();
		D3D11Shader* shader = ContentManager::Instance()->Get<D3D11Shader>("shaders/base.fx");
		shader->Set();
    target->Clear(context_);
		default_depth_state_->Set();
    target->Set(context_, camera_->type() == D3D11Camera::CameraTypes::kPerspective ? depth_stencil_view_ : nullptr);
    target->Draw(context_);

		MapGlobalBuffer();
		line_->Draw();
		
		sampler_linear_->Set();
    back_buffer_->Set(context_);

    viewport_->Set();
    screen_quad_->Set();

    ID3D11ShaderResourceView* resource = target->resource();
    context_->PSSetShaderResources(0, 1, &resource);
    default_blend_state_->Set();
		context_->OMSetDepthStencilState(NULL, 1);

		//target->uniforms()->Apply();
		D3D11Effect* effect = target->post_processing();

		if (effect != nullptr && effect->is_valid())
		{
			const std::string& technique = target->technique();
			unsigned int n_pass = effect->NumPasses(technique);

			if (n_pass > 0)
			{
				for (unsigned int i = 0; i < n_pass; ++i)
				{
					effect->Apply(technique, i);
					screen_quad_->Draw();
				}
			}
			else
			{
				screen_quad_->Draw();
			}
		}
		else
		{
			target->set_post_processing(nullptr);
			screen_quad_->Draw();
		}

		set_textures_[0] = nullptr;
		set_textures_[1] = nullptr;
		set_textures_[2] = nullptr;

		current_shader_ = nullptr;
		current_target_ = nullptr;
  }

  //-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::ResizeBuffers(const int& w, const int& h)
	{
		if (ready_ == false)
		{
			return;
		}

		ready_ = false;

		back_buffer_->Release();
		depth_stencil_view_->Release();
		depth_stencil_buffer_->Release();

		HRESULT result = swap_chain_->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		SNUFF_XASSERT(result == S_OK, HRToString(result, "ResizeBuffers"), "D3D11RenderDevice::ResizeBuffers");

		CreateBackBuffer();
		CreateDepthStencilView();

		for (std::map<std::string, D3D11RenderTarget*>::iterator it = render_targets_.begin(); it != render_targets_.end(); ++it)
		{
			it->second->Create(D3D11RenderTarget::RenderTargets::kRenderTarget, swap_chain_, device_);
		}

    CreateBaseViewport();

		ready_ = true;
	}

  //-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::Dispose()
	{
    swap_chain_->SetFullscreenState(FALSE, NULL);

		SNUFF_SAFE_RELEASE(adapter_, "D3D11RenderDevice::Dispose::adapter_");
		SNUFF_SAFE_RELEASE(swap_chain_, "D3D11RenderDevice::Dispose::swap_chain_");
		SNUFF_SAFE_RELEASE(device_, "D3D11RenderDevice::Dispose::device_");
		SNUFF_SAFE_RELEASE(context_, "D3D11RenderDevice::Dispose::context_");
		SNUFF_SAFE_RELEASE(depth_stencil_view_, "D3D11RenderDevice::Dispose::depth_stencil_view_");
		SNUFF_SAFE_RELEASE(depth_stencil_buffer_, "D3D11RenderDevice::Dispose::depth_stencil_buffer_");

		SNUFF_LOG_INFO("Disposed the Direct3D 11 render device");
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::AddRenderTarget(D3D11RenderTarget* target)
	{
		const std::string& name = target->name();
		std::map<std::string, D3D11RenderTarget*>::iterator it = render_targets_.find(name);

		if (it != render_targets_.end())
		{
			SNUFF_LOG_WARNING("Attempted to add a render target with an already existing name '" + name + "'");
			return;
		}

		render_targets_.emplace(name, target);

		SNUFF_LOG_INFO("Added render target '" + name + "'");
	}

  //-------------------------------------------------------------------------------------------
  D3D11RenderTarget* D3D11RenderDevice::GetTarget(const std::string& name)
  {
    std::map<std::string, D3D11RenderTarget*>::iterator it = render_targets_.find(name);

    if (it != render_targets_.end())
    {
      return it->second;
    }

    SNUFF_LOG_WARNING("Attempted to retrieve a non-existant render target '" + name + "'");
    return nullptr;
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::MapGlobalBuffer()
	{
		constant_buffer_->Map({
			static_cast<float>(Game::Instance()->time()),
			camera_->view(),
			camera_->projection(),
			camera_->translation()
		});
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::MapUIBuffer()
	{
		const XMFLOAT2& res = D3D11RenderSettings::Instance()->resolution();

    XMMATRIX projection = XMMatrixOrthographicRH(res.x, res.y, -9999999.0f, 9999999.0f);

		constant_buffer_->Map({
			static_cast<float>(Game::Instance()->time()),
			XMMatrixIdentity(),
			projection,
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)
		});
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::SetSampler(const int& type)
	{
		D3D11SamplerState::SamplerTypes t = static_cast<D3D11SamplerState::SamplerTypes>(type);

		switch (t)
		{
		case D3D11SamplerState::SamplerTypes::kLinear:
			sampler_linear_->Set();
			break;
		case D3D11SamplerState::SamplerTypes::kAnisotropic:
			sampler_anisotropic_->Set();
			break;
		case D3D11SamplerState::SamplerTypes::kPoint:
			sampler_point_->Set();
			break;
		}
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::SetFullscreen(const bool& fullscreen)
  {
    swap_chain_->SetFullscreenState(fullscreen, NULL);
  }

  //-------------------------------------------------------------------------------------------
	IDXGISwapChain* D3D11RenderDevice::swap_chain()
	{
		return swap_chain_;
	}

  //-------------------------------------------------------------------------------------------
	ID3D11Device* D3D11RenderDevice::device()
	{
		return device_;
	}

  //-------------------------------------------------------------------------------------------
	ID3D11DeviceContext* D3D11RenderDevice::context()
	{
		return context_;
	}

  //-------------------------------------------------------------------------------------------
	D3D11RenderTarget* D3D11RenderDevice::back_buffer()
	{
		return back_buffer_.get();
	}

  //-------------------------------------------------------------------------------------------
  const int& D3D11RenderDevice::vertex_buffer_type() const
  {
    return vertex_buffer_type_;
  }

	//-------------------------------------------------------------------------------------------
	D3D11ConstantBuffer* D3D11RenderDevice::constant_buffer()
	{
		return constant_buffer_;
	}

	//-------------------------------------------------------------------------------------------
	D3D11Camera* D3D11RenderDevice::camera()
	{
		return camera_;
	}

	//-------------------------------------------------------------------------------------------
	D3D11Texture** D3D11RenderDevice::set_textures()
	{
		return set_textures_;
	}

	//-------------------------------------------------------------------------------------------
	D3D11Shader* D3D11RenderDevice::current_shader()
	{
		return current_shader_;
	}

	//-------------------------------------------------------------------------------------------
	D3D11RenderTarget* D3D11RenderDevice::current_target()
	{
		return current_target_;
	}

  //-------------------------------------------------------------------------------------------
  D3D11Texture* D3D11RenderDevice::default_texture()
  {
    return default_texture_.get();
  }

  //-------------------------------------------------------------------------------------------
  D3D11Effect* D3D11RenderDevice::default_effect()
  {
    return default_effect_.get();
  }

	//-------------------------------------------------------------------------------------------
	D3D11Effect* D3D11RenderDevice::default_post_processing()
	{
		return default_post_processing_.get();
	}

  //-------------------------------------------------------------------------------------------
  D3D11Material* D3D11RenderDevice::default_material()
  {
    return default_material_.get();
  }

  //-------------------------------------------------------------------------------------------
  D3D11BlendState* D3D11RenderDevice::default_blend_state()
  {
    return default_blend_state_.get();
  }

  //-------------------------------------------------------------------------------------------
  D3D11DepthState* D3D11RenderDevice::default_depth_state()
  {
    return default_depth_state_.get();
  }

	//-------------------------------------------------------------------------------------------
	D3D11Viewport* D3D11RenderDevice::viewport_render_target()
	{
		return viewport_.get();
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderDevice::set_vertex_buffer_type(const int& type)
  {
    vertex_buffer_type_ = type;
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::set_camera(D3D11Camera* camera)
	{
		camera_ = camera;
	}
	
	//-------------------------------------------------------------------------------------------
	void D3D11RenderDevice::set_current_shader(D3D11Shader* shader)
	{
		current_shader_ = shader;
	}

  //-------------------------------------------------------------------------------------------
	D3D11RenderDevice::~D3D11RenderDevice()
	{
	
	}
}