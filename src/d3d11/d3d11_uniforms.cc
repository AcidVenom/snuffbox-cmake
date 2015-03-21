#include "../d3d11/d3d11_uniforms.h"
#include "../d3d11/d3d11_constant_buffer.h"

#include <vector>

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Uniforms::D3D11Uniforms()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Uniforms::SetUniform(const D3D11Uniforms::UniformTypes& type, const std::string& name, const float* value)
	{
		for (Uniform& it : uniforms_)
		{
			if (it.name == name)
			{
				SetValue(type, it.value, value);
				return;
			}
		}

		Uniform uniform;
		uniform.name = name;
		SetValue(type, uniform.value, value);

		uniforms_.push_back(uniform);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Uniforms::SetValue(const D3D11Uniforms::UniformTypes& type, XMFLOAT4& v, const float* value)
	{
		for (int i = 0; i < static_cast<int>(type); ++i)
		{
			switch (i)
			{
			case 0:
				v.x = value[i];
				v.y = 0.0f;
				v.z = 0.0f;
				v.w = 0.0f;
				break;
			case 1:
				v.y = value[i];
				break;
			case 2:
				v.z = value[i];
				break;
			case 3:
				v.w = value[i];
				break;
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Uniforms::Apply()
	{
		CbUniforms cb_uniforms;
		D3D11ConstantBuffer* buffer = D3D11RenderDevice::Instance()->constant_buffer();

		int s = static_cast<int>(uniforms_.size());
		for (int i = 0; i < s; ++i)
		{
			cb_uniforms.Uniforms[i] = uniforms_.at(i).value;
		}

		buffer->Map(cb_uniforms, s);
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Uniforms::~D3D11Uniforms()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Uniforms::RegisterJS(JS_SINGLETON obj)
	{
		JSWrapper::SetObjectValue(obj, "Float", 1);
		JSWrapper::SetObjectValue(obj, "Float2", 2);
		JSWrapper::SetObjectValue(obj, "Float3", 3);
		JSWrapper::SetObjectValue(obj, "Float4", 4);
	}
}