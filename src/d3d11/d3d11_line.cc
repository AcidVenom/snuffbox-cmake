#include "../d3d11/d3d11_line.h"
#include "../d3d11/d3d11_material.h"
#include "../d3d11/d3d11_constant_buffer.h"
#include "../d3d11/d3d11_effect.h"
#include "../d3d11/d3d11_render_settings.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Line::D3D11Line()
	{
		material_ = AllocatedMemory::Instance().Construct<D3D11Material>();
		
		D3D11Material::Attributes& attributes = material_->attributes();
		attributes.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		attributes.emissive = 1.0f;
		attributes.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		attributes.specular_power = 1.0f;
		attributes.specular_intensity = 0.0f;

		material_->Validate();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Line::DrawLine(
		const float& x1, const float& y1, const float& z1,
		const float& r1, const float& g1, const float& b1,
		const float& x2, const float& y2, const float& z2,
		const float& r2, const float& g2, const float& b2)
	{
		const bool& invert = D3D11RenderSettings::Instance()->invert_y();
		float yy1 = invert == false ? y1 : y1 * -1;
		float yy2 = invert == false ? y2 : y2 * -1;

		vertices_.push_back(
			{ XMFLOAT4(x1, -yy1, z1, 1.0f), XMFLOAT4(r1, g1, b1, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }
			);

		vertices_.push_back(
			{ XMFLOAT4(x2, -yy2, z2, 1.0f), XMFLOAT4(r2, g2, b2, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }
			);

		indices_.push_back(static_cast<int>(vertices_.size()) - 2);
		indices_.push_back(static_cast<int>(vertices_.size()) - 1);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Line::Draw()
	{
		if (vertices_.size() > 0)
		{
			D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();

			D3D11ConstantBuffer* constant_buffer = render_device->constant_buffer();
			constant_buffer->Map({
				XMMatrixIdentity(),
				XMMatrixIdentity(),
				XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
				XMFLOAT3(1.0f, 1.0f, 1.0f),
				1.0f,
				material_->attributes()
			});

			vertex_buffer_ = AllocatedMemory::Instance().Construct<D3D11VertexBuffer>(D3D11VertexBuffer::VertexBufferType::kOther);
			vertex_buffer_->Create(vertices_, indices_, false);
			vertex_buffer_->set_topology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

      material_->Apply();
			render_device->default_effect()->Apply("Diffuse", 0);
			vertex_buffer_->Set();
			vertex_buffer_->Draw();

			vertices_.clear();
			indices_.clear();
		}
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Line::~D3D11Line()
	{

	}
}