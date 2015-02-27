#include "../d3d11/d3d11_constant_buffer.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11ConstantBuffer::D3D11ConstantBuffer() :
		valid_(false),
		global_buffer_(nullptr),
		per_object_buffer_(nullptr)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ConstantBuffer::Create()
	{
		HRESULT result = S_OK;
		CbGlobal cb_global;
		CbPerObject cb_per_object;

		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(CbGlobal) * 4;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &cb_global;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		ID3D11Device* device = render_device->device();

		result = device->CreateBuffer(&desc, &data, &global_buffer_);
		SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateBuffer"), "D3D11ConstantBuffer::Create::global_buffer_");

		desc.ByteWidth = sizeof(CbPerObject) * 4;
		data.pSysMem = &cb_per_object;

		result = device->CreateBuffer(&desc, &data, &per_object_buffer_);
		SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateBuffer"), "D3D11ConstantBuffer::Create::per_object_buffer_");
		valid_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ConstantBuffer::Map(const CbGlobal& cb)
	{
		CbGlobal* mapped = nullptr;

		D3D11_MAPPED_SUBRESOURCE data;
		ID3D11DeviceContext* ctx = D3D11RenderDevice::Instance()->context();

		ctx->Map(global_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		mapped = static_cast<CbGlobal*>(data.pData);
		mapped->Time = cb.Time;
		mapped->View = cb.View;
		mapped->Projection = cb.Projection;
		ctx->Unmap(global_buffer_, 0);

		mapped_ = global_buffer_;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ConstantBuffer::Map(const CbPerObject& cb)
	{
		CbPerObject* mapped = nullptr;

		D3D11_MAPPED_SUBRESOURCE data;
		ID3D11DeviceContext* ctx = D3D11RenderDevice::Instance()->context();

		ctx->Map(per_object_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		mapped = static_cast<CbPerObject*>(data.pData);
		mapped->World = cb.World;
		mapped->InvWorld = cb.InvWorld;
		mapped->Alpha = cb.Alpha;
		mapped->Blend = cb.Blend;
		mapped->AnimationCoords = cb.AnimationCoords;
		mapped->Attributes = cb.Attributes;
		ctx->Unmap(per_object_buffer_, 0);

		mapped_ = per_object_buffer_;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ConstantBuffer::Set(const int& index)
	{
		ID3D11DeviceContext* ctx = D3D11RenderDevice::Instance()->context();

		ctx->VSSetConstantBuffers(index, 1, &mapped_);
		ctx->PSSetConstantBuffers(index, 1, &mapped_);
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ConstantBuffer::~D3D11ConstantBuffer()
	{
		if (valid_ == false)
		{
			return;
		}

		SNUFF_SAFE_RELEASE(global_buffer_, "D3D11ConstantBuffer::~D3D11ConstantBuffer::global_buffer_");
		SNUFF_SAFE_RELEASE(per_object_buffer_, "D3D11ConstantBuffer::~D3D11ConstantBuffer::per_object_buffer_");
	}
}