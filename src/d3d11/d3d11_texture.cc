#include "../d3d11/d3d11_texture.h"
#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_vertex_buffer.h"
#include "../d3d11/d3d11_shader.h"
#include "../d3d11/d3d11_viewport.h"

#include "../content/content_manager.h"
#include "../application/game.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Texture::D3D11Texture() :
		Content(ContentTypes::kTexture),
		width_(0),
		height_(0),
		valid_(false),
		texture_(nullptr),
    release_(true)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Texture::Load(const std::string& path)
	{
		if (texture_ != nullptr)
		{
			SNUFF_SAFE_RELEASE(texture_, "D3D11Texture::Load::" + path);
		}

		std::string full_path = Game::Instance()->path() + "/" + path;

		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		ID3D11Device* device = render_device->device();

		HRESULT result = S_OK;
		result = D3DX11CreateShaderResourceViewFromFileA(
			device,
			full_path.c_str(),
			NULL,
			NULL,
			&texture_,
			NULL
			);

		SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "D3DX11CreateShaderResourceViewFromFileA"), "D3D11Texture::Load::" + path);

		ID3D11Resource* res;
		texture_->GetResource(&res);

		ID3D11Texture2D* underlying = static_cast<ID3D11Texture2D*>(res);

		D3D11_TEXTURE2D_DESC desc;

		underlying->GetDesc(&desc);

		width_ = desc.Width;
		height_ = desc.Height;
		format_ = desc.Format;

		underlying->Release();

		valid_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Texture::Create(const int& width, const int& height, const DXGI_FORMAT& format, const void* buffer, const unsigned short& stride)
	{
		width_ = width;
		height_ = height;
		format_ = format;
		ID3D11Texture2D* texture = nullptr;

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = static_cast<UINT>(width);
		desc.Height = static_cast<UINT>(height);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data;

		data.pSysMem = buffer;
		data.SysMemPitch = stride;
		data.SysMemSlicePitch = stride;

		HRESULT hr = S_OK;

		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		ID3D11Device* device = render_device->device();

		hr = device->CreateTexture2D(&desc, &data, &texture);
		SNUFF_XASSERT(hr == S_OK, render_device->HRToString(hr, "CreateTexture2D"), "D3D11Texture::Create");

		hr = device->CreateShaderResourceView(texture, NULL, &texture_);
		SNUFF_XASSERT(hr == S_OK, render_device->HRToString(hr, "CreateShaderResourceView"), "D3D11Texture::Create");

		SNUFF_SAFE_RELEASE(texture, "D3D11Texture::Create::texture");

		valid_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Texture::Create(ID3D11ShaderResourceView* texture, const bool& release)
	{
		texture_ = texture;
		valid_ = true;
    release_ = release;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Texture::CreateCubeMap(const D3D11Texture::Cube& cube)
	{
    if (cube.front == nullptr || cube.back == nullptr || cube.left == nullptr || cube.right == nullptr || cube.top == nullptr || cube.bottom == nullptr)
    {
      SNUFF_LOG_WARNING("Attempted to create an invalid cube map");
      return;
    }

		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		ID3D11Device* device = render_device->device();
		ID3D11DeviceContext* ctx = render_device->context();

		D3D11VertexBuffer vb(D3D11VertexBuffer::VertexBufferType::kOther);
		std::vector<int> indices({
      0, 1, 2, 3, 2, 1
		});

		vb.Create({
			{ XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
			{ XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
			{ XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
			{ XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }
		},
		indices);

		D3D11Texture* front = cube.front;
		const int& w = front->width();
		const int& h = front->height();
		const DXGI_FORMAT& format = front->format();
		
		D3D11Viewport viewport;
		viewport.Create(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h));
		viewport.Set();

		D3D11_TEXTURE2D_DESC d;
		ZeroMemory(&d, sizeof(D3D11_TEXTURE2D_DESC));

		d.Width = static_cast<UINT>(w);
		d.Height = static_cast<UINT>(h);
		d.MipLevels = 0;
		d.ArraySize = 6;
		d.Format = format;
		d.SampleDesc.Count = 1;
		d.SampleDesc.Quality = 0;
		d.Usage = D3D11_USAGE_DEFAULT;
		d.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		d.CPUAccessFlags = 0;
		d.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		ID3D11Texture2D* texture;

		HRESULT hr = device->CreateTexture2D(&d, nullptr, &texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(texture, &desc, &texture_);
		texture->Release();

		vb.Set();

		D3D11Shader* pp = ContentManager::Instance()->Get<D3D11Shader>("shaders/cube_map.fx");
		pp->Set();

		for (unsigned int i = 0; i < 6; ++i)
		{
			D3D11_RENDER_TARGET_VIEW_DESC view_desc;
			ZeroMemory(&view_desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

			view_desc.Format = format;
			view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			view_desc.Texture2DArray.ArraySize = 1;
			view_desc.Texture2DArray.FirstArraySlice = i;

			ID3D11RenderTargetView* target;
			hr = device->CreateRenderTargetView(texture, &view_desc, &target);
      SNUFF_XASSERT(hr == S_OK, render_device->HRToString(hr, "CreateRenderTargetView"), "D3D11Texture::CreateCubeMap");

			ctx->OMSetRenderTargets(1, &target, nullptr);

			switch (i)
			{
			case 0:
				cube.right->Set(0);
				break;
			case 1:
				cube.left->Set(0);
				break;
			case 2:
				cube.bottom->Set(0);
				break;
			case 3:
				cube.top->Set(0);
				break;
			case 4:
				cube.back->Set(0);
				break;
			case 5:
				cube.front->Set(0);
				break;
			}

			vb.Draw();
			target->Release();
		}

		valid_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Texture::Set(const int& slot)
	{
		if (valid_ == false)
		{
			return;
		}

		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		D3D11Texture** textures = render_device->set_textures();

		if (textures[slot] != this)
		{
			render_device->context()->PSSetShaderResources(slot, 1, &texture_);
			textures[slot] = this;
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Texture::SetMultipleTextures(const int& start, const int& num, const std::vector<D3D11Texture*>& textures)
	{
    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    D3D11Texture** old = render_device->set_textures();

    ID3D11ShaderResourceView* resources[8];
    D3D11Texture* current = nullptr;

    bool set = false;
    for (unsigned int i = 0; i < textures.size(); ++i)
    {
      current = textures.at(i);
      resources[i] = current->texture();
  
      if (old[start + i] == nullptr || old[start + i] != textures.at(i))
      {
        set = true;
      }

      old[start + i] = current;
    }

		resources[start + textures.size()] = render_device->depth_stencil_resource();

    if (set == true)
    {
      render_device->context()->PSSetShaderResources(start, num, resources);
    }
	}

	//---------------------------------------------------------------------------------------------------------
	const int& D3D11Texture::width() const
	{
		return width_;
	}

	//---------------------------------------------------------------------------------------------------------
	const int& D3D11Texture::height() const
	{
		return height_;
	}

	//---------------------------------------------------------------------------------------------------------
	ID3D11ShaderResourceView* D3D11Texture::texture()
	{
		return texture_;
	}

	//---------------------------------------------------------------------------------------------------------
	const DXGI_FORMAT& D3D11Texture::format() const
	{
		return format_;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Texture::~D3D11Texture()
	{
		if (valid_ == false || release_ == false)
		{
			return;
		}

		SNUFF_SAFE_RELEASE(texture_, "D3D11Texture::~D3D11Texture::texture_");
	}
}