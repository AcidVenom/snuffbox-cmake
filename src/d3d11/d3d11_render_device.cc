#include "../d3d11/d3d11_render_device.h"
#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_vertex_buffer.h"
#include "../d3d11/d3d11_input_layout.h"
#include "../d3d11/d3d11_shader.h"
#include "../d3d11/d3d11_viewport.h"
#include "../d3d11/d3d11_render_settings.h"

#include "../application/game.h"
#include "../platform/platform_window.h"

#include "../cvar/cvar.h"
#include "../content/content_manager.h"

#include <comdef.h>

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11RenderDevice::D3D11RenderDevice() :
		ready_(false),
		adapter_(nullptr),
		swap_chain_(nullptr),
		device_(nullptr),
		context_(nullptr),
    vertex_buffer_type_(-1)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderDevice* D3D11RenderDevice::Instance()
	{
		static SharedPtr<D3D11RenderDevice> render_device = AllocatedMemory::Instance().Construct<D3D11RenderDevice>();
		return render_device.get();
	}

	//---------------------------------------------------------------------------------------------------------
	std::string D3D11RenderDevice::HRToString(HRESULT hr, std::string context)
	{
		_com_error error(hr);
		return "(" + context + ") " + error.ErrorMessage();
	}

	//---------------------------------------------------------------------------------------------------------
	bool D3D11RenderDevice::Initialise()
	{
		CreateDevice();
    CreateBackBuffer();
    CreateScreenQuad();
    ContentManager::Instance()->Load(ContentTypes::kShader, "base.fx");
    CreateInputLayout();
    CreateBaseViewport();

		SNUFF_LOG_SUCCESS("Succesfully initialised the Direct3D 11 render device");

		ready_ = true;
		return true;
	}

	//---------------------------------------------------------------------------------------------------------
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

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderDevice::CreateBackBuffer()
  {
    back_buffer_ = AllocatedMemory::Instance().Construct<D3D11RenderTarget>("Backbuffer");
    back_buffer_->Create(D3D11RenderTarget::RenderTargets::kBackBuffer, swap_chain_, device_);

    back_buffer_->Set(context_);
  }
  
  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderDevice::CreateScreenQuad()
  {
    std::vector<Vertex> vertices({
      { XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
      { XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
      { XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
      { XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }
    });

    std::vector<int> indices({
      0, 1, 2, 3
    });

    screen_quad_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kScreen);
    screen_quad_->Create(vertices, indices);
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderDevice::CreateInputLayout()
  {
    D3D11Shader* shader = ContentManager::Instance()->Get<D3D11Shader>("base.fx");
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

  //---------------------------------------------------------------------------------------------------------
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

    viewport_->Set();
  }

	//---------------------------------------------------------------------------------------------------------
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

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderDevice::Draw()
	{
		back_buffer_->Clear(context_);
    screen_quad_->Set();
    screen_quad_->Draw();
		swap_chain_->Present(0, 0);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderDevice::ResizeBuffers(int w, int h)
	{
		if (ready_ == false)
		{
			return;
		}

		ready_ = false;

		back_buffer_->Release();

		HRESULT result = swap_chain_->ResizeBuffers(1, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		SNUFF_XASSERT(result == S_OK, HRToString(result, "ResizeBuffers"), "D3D11RenderDevice::ResizeBuffers");

		CreateBackBuffer();
		CreateBaseViewport();

		ready_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RenderDevice::Dispose()
	{
		SNUFF_SAFE_RELEASE(adapter_, "D3D11RenderDevice::Dispose::adapter_");
		SNUFF_SAFE_RELEASE(swap_chain_, "D3D11RenderDevice::Dispose::swap_chain_");
		SNUFF_SAFE_RELEASE(device_, "D3D11RenderDevice::Dispose::device_");
		SNUFF_SAFE_RELEASE(context_, "D3D11RenderDevice::Dispose::context_");

		SNUFF_LOG_INFO("Disposed the Direct3D 11 render device");
	}

	//---------------------------------------------------------------------------------------------------------
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

	//---------------------------------------------------------------------------------------------------------
	IDXGISwapChain* D3D11RenderDevice::swap_chain()
	{
		return swap_chain_;
	}

	//---------------------------------------------------------------------------------------------------------
	ID3D11Device* D3D11RenderDevice::device()
	{
		return device_;
	}

	//---------------------------------------------------------------------------------------------------------
	ID3D11DeviceContext* D3D11RenderDevice::context()
	{
		return context_;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderTarget* D3D11RenderDevice::back_buffer()
	{
		return back_buffer_.get();
	}

  //---------------------------------------------------------------------------------------------------------
  const int& D3D11RenderDevice::vertex_buffer_type() const
  {
    return vertex_buffer_type_;
  }

  //---------------------------------------------------------------------------------------------------------
  void D3D11RenderDevice::set_vertex_buffer_type(int type)
  {
    vertex_buffer_type_ = type;
  }

	//---------------------------------------------------------------------------------------------------------
	D3D11RenderDevice::~D3D11RenderDevice()
	{
	
	}
}