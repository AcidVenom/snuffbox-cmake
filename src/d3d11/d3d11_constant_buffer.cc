#include "../d3d11/d3d11_constant_buffer.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11ConstantBuffer::D3D11ConstantBuffer() :
		valid_(false),
		global_buffer_(nullptr),
		per_object_buffer_(nullptr),
		uniforms_buffer_(nullptr)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ConstantBuffer* D3D11ConstantBuffer::Instance()
	{
		static SharedPtr<D3D11ConstantBuffer> constant_buffer = AllocatedMemory::Instance().Construct<D3D11ConstantBuffer>();
		return constant_buffer.get();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ConstantBuffer::Create()
	{
		HRESULT result = S_OK;
		CbGlobal cb_global;
		CbPerObject cb_per_object;
		CbUniforms cb_uniforms;

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
		
		desc.ByteWidth = sizeof(XMFLOAT4) * 1024;
		data.pSysMem = &cb_uniforms;

		result = device->CreateBuffer(&desc, &data, &uniforms_buffer_);
		SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateBuffer"), "D3D11ConstantBuffer::Create::uniforms_buffer_");
		
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
		mapped->EyePosition = cb.EyePosition;
		mapped->InvViewProjection = cb.InvViewProjection;
		ctx->Unmap(global_buffer_, 0);
		Set();
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
		mapped->AnimationCoords = cb.AnimationCoords;
		mapped->Blend = cb.Blend;
		mapped->Alpha = cb.Alpha;
		mapped->Attributes = cb.Attributes;
		ctx->Unmap(per_object_buffer_, 0);
		Set();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ConstantBuffer::Map(const CbUniforms& cb, const int& num_uniforms)
	{
		CbUniforms* mapped = nullptr;

		D3D11_MAPPED_SUBRESOURCE data;
		ID3D11DeviceContext* ctx = D3D11RenderDevice::Instance()->context();

		ctx->Map(uniforms_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		mapped = static_cast<CbUniforms*>(data.pData);
		for (int i = 0; i < num_uniforms; ++i)
		{
			mapped->Uniforms[i] = cb.Uniforms[i];
		}
		ctx->Unmap(uniforms_buffer_, 0);
		Set();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ConstantBuffer::Set()
	{
		ID3D11DeviceContext* ctx = D3D11RenderDevice::Instance()->context();

		ID3D11Buffer* buffers[] = {
			global_buffer_,
			per_object_buffer_,
			uniforms_buffer_
		};

		ctx->VSSetConstantBuffers(0, 3, buffers);
		ctx->PSSetConstantBuffers(0, 3, buffers);
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
		SNUFF_SAFE_RELEASE(uniforms_buffer_, "D3D11ConstantBuffer::~D3D11ConstantBuffer::uniforms_buffer_");
	}
}