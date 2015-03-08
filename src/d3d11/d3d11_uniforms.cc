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
		Uniforms::iterator it = uniforms_.find(name);

		if (it != uniforms_.end())
		{
			XMFLOAT4& v = it->second;

			SetValue(type, v, value);
			return;
		}

		XMFLOAT4 uniform;
		SetValue(type, uniform, value);

		uniforms_.emplace(name, uniform);
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
		std::vector<XMFLOAT4> uniforms;
		for (Uniforms::iterator it = uniforms_.begin(); it != uniforms_.end(); ++it)
		{
			uniforms.push_back(it->second);
		}

		CbUniforms cb_uniforms;
		D3D11ConstantBuffer* buffer = D3D11RenderDevice::Instance()->uniforms_buffer();

		int s = static_cast<int>(uniforms.size());
		for (int i = 0; i < s; ++i)
		{
			cb_uniforms.Uniforms[i] = uniforms[i];
		}

		buffer->Map(cb_uniforms, s);
		buffer->Set(3);
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