#include "../d3d11/d3d11_texture.h"
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
		texture_(nullptr)
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

		underlying->Release();

		valid_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Texture::Create(const int& width, const int& height, const DXGI_FORMAT& format, const void* buffer, const unsigned short& stride)
	{
		width_ = width;
		height_ = height;
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
	void D3D11Texture::Create(ID3D11ShaderResourceView* texture)
	{
		texture_ = texture;
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
			textures[slot] = this;
			render_device->context()->PSSetShaderResources(slot, 1, &texture_);
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
	D3D11Texture::~D3D11Texture()
	{
		if (valid_ == false)
		{
			return;
		}

		SNUFF_SAFE_RELEASE(texture_, "D3D11Texture::~D3D11Texture::texture_");
	}
}