#include "../d3d11/d3d11_lighting.h"
#include "../d3d11/d3d11_light.h"
#include "../d3d11/d3d11_constant_buffer.h"
#include "../d3d11/d3d11_camera.h"
#include "../d3d11/d3d11_vertex_buffer.h"
#include "../d3d11/d3d11_blend_state.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Lighting::D3D11Lighting() :
		ambient_colour_(1.0f, 1.0f, 1.0f, 1.0f)
	{
		additive_ = AllocatedMemory::Instance().Construct<D3D11BlendState>();
		additive_->CreateFromJson(std::string("{\
			\"SrcBlend\" : \"One\",\
			\"DestBlend\" : \"One\"\
		}"));
		SNUFF_LOG_SUCCESS("Succesfully initialised the Direct3D 11 lighting system");
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Lighting* D3D11Lighting::Instance()
	{
		static SharedPtr<D3D11Lighting> lighting = AllocatedMemory::Instance().Construct<D3D11Lighting>();
		return lighting.get();
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::Add(D3D11Light* light)
	{
		for (D3D11Light* it : lights_)
		{
			if (it == light)
			{
				return;
			}
		}

		lights_.push_back(light);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::Remove(D3D11Light* light)
	{
		if (lights_.size() == 0)
		{
			return;
		}
		for (D3D11Light* it : lights_)
		{
			if (it == light)
			{
				it = nullptr;
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::Update(D3D11ConstantBuffer* buffer, D3D11VertexBuffer* vbo)
	{
		additive_->Set();
		D3D11Light* light = nullptr;
		for (unsigned int i = 0; i < lights_.size(); ++i)
		{
			light = lights_.at(i);

			if (light->attributes().activated == false)
			{
				continue;
			}

			const D3D11Light::Attributes& a = light->attributes();
			buffer->Map({ a });
			vbo->Draw();
		}
	}

	//---------------------------------------------------------------------------------------------------------
	const std::vector<D3D11Light*>& D3D11Lighting::lights() const
	{
		return lights_;
	}

	//---------------------------------------------------------------------------------------------------------
	const XMFLOAT4& D3D11Lighting::ambient_colour() const
	{
		return ambient_colour_;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::set_ambient_colour(const float& r, const float& g, const float& b)
	{
		ambient_colour_.x = r;
		ambient_colour_.y = g;
		ambient_colour_.z = b;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Lighting::~D3D11Lighting()
	{

	}
	
	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::RegisterJS(JS_SINGLETON obj)
	{
		JSFunctionRegister funcs[] = {
			{ "setAmbientColour", JSSetAmbientColour }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::JSSetAmbientColour(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		
		if (wrapper.Check("NNN"))
		{
			D3D11Lighting::Instance()->set_ambient_colour(
				wrapper.GetValue<float>(0, 1.0f),
				wrapper.GetValue<float>(1, 1.0f),
				wrapper.GetValue<float>(2, 1.0f)
				);
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Lighting::JSAmbientColour(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		const XMFLOAT4 c = D3D11Lighting::Instance()->ambient_colour();

		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

		JSWrapper::SetObjectValue(obj, "r", c.x);
		JSWrapper::SetObjectValue(obj, "g", c.y);
		JSWrapper::SetObjectValue(obj, "b", c.z);

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}
}