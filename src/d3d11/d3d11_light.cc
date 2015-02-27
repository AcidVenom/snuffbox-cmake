#include "../d3d11/d3d11_light.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	D3D11Light::D3D11Light(D3D11Light::LightTypes type)
	{
		attributes_.type = type;
	}

	//-------------------------------------------------------------------------------------------
	D3D11Light::D3D11Light(JS_ARGS args)
	{

	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::TranslateBy(const float& x, const float& y, const float& z)
	{
		XMFLOAT4& t = attributes_.translation;
		t.x += x;
		t.y += y;
		t.z += z;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::RotateBy(const float& x, const float& y, const float& z)
	{
		XMFLOAT4& t = attributes_.rotation;
		t.x += x;
		t.y += y;
		t.z += z;
	}

	//-------------------------------------------------------------------------------------------
	const D3D11Light::Attributes& D3D11Light::attributes() const
	{
		return attributes_;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::set_translation(const float& x, const float& y, const float& z)
	{
		XMFLOAT4& t = attributes_.translation;
		t.x = x;
		t.y = y;
		t.z = z;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::set_rotation(const float& x, const float& y, const float& z)
	{
		XMFLOAT4& r = attributes_.rotation;
		r.x = x;
		r.y = y;
		r.z = z;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::set_colour(const float& r, const float& g, const float& b, const float& a)
	{
		XMFLOAT4& c = attributes_.colour;
		c.x = r;
		c.y = g;
		c.z = b;
		c.w = a;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::set_spot_angle(const float& angle)
	{
		attributes_.spot_angle = angle;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::set_constant_attenuation(const float& attenuation)
	{
		attributes_.constant_attenuation = attenuation;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::set_linear_attenuation(const float& attenuation)
	{
		attributes_.linear_attenuation = attenuation;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::set_quadratic_attenuation(const float& attenuation)
	{
		attributes_.quad_attenuation = attenuation;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::set_activated(const bool& activated)
	{
		attributes_.activated = activated;
	}

	//-------------------------------------------------------------------------------------------
	D3D11Light::~D3D11Light()
	{

	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::RegisterJS(JS_CONSTRUCTABLE obj)
	{
		JSFunctionRegister funcs[] = {
			{ "setTranslation", JSSetTranslation },
			{ "translateBy", JSTranslateBy },
			{ "translation", JSTranslation },
			{ "setRotation", JSSetRotation },
			{ "rotateBy", JSRotateBy },
			{ "rotation", JSRotation },
			{ "setColour", JSSetColour },
			{ "colour", JSColour },
			{ "setSpotAngle", JSSetSpotAngle },
			{ "spotAngle", JSSpotAngle },
			{ "setConstantAttenuation", JSSetConstantAttenuation },
			{ "constantAttenuation", JSConstantAttenuation },
			{ "setLinearAttenuation", JSSetLinearAttenuation },
			{ "linearAttenuation", JSLinearAttenuation },
			{ "setQuadraticAttenuation", JSSetQuadraticAttenuation },
			{ "quadraticAttenuation", JSQuadraticAttenuation },
			{ "setActivated", JSSetActivated },
			{ "activated", JSActivated }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSSetTranslation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->set_translation(
				wrapper.GetValue<float>(0, 0.0f),
				wrapper.GetValue<float>(1, 0.0f),
				wrapper.GetValue<float>(2, 0.0f)
				);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSTranslateBy(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->TranslateBy(
				wrapper.GetValue<float>(0, 0.0f),
				wrapper.GetValue<float>(1, 0.0f),
				wrapper.GetValue<float>(2, 0.0f)
				);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSTranslation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());
		
		const Attributes& a = self->attributes();

		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

		const XMFLOAT4& p = a.translation;
		JSWrapper::SetObjectValue(obj, "x", p.x);
		JSWrapper::SetObjectValue(obj, "y", p.y);
		JSWrapper::SetObjectValue(obj, "z", p.z);

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSSetRotation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->set_rotation(
				wrapper.GetValue<float>(0, 0.0f),
				wrapper.GetValue<float>(1, 0.0f),
				wrapper.GetValue<float>(2, 0.0f)
				);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSRotateBy(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->RotateBy(
				wrapper.GetValue<float>(0, 0.0f),
				wrapper.GetValue<float>(1, 0.0f),
				wrapper.GetValue<float>(2, 0.0f)
				);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSRotation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		const Attributes& a = self->attributes();

		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

		const XMFLOAT4& p = a.rotation;
		JSWrapper::SetObjectValue(obj, "x", p.x);
		JSWrapper::SetObjectValue(obj, "y", p.y);
		JSWrapper::SetObjectValue(obj, "z", p.z);

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSSetColour(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->set_colour(
				wrapper.GetValue<float>(0, 0.0f),
				wrapper.GetValue<float>(1, 0.0f),
				wrapper.GetValue<float>(2, 0.0f),
				wrapper.GetValue<float>(3, 1.0f)
				);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSColour(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		const Attributes& a = self->attributes();

		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

		const XMFLOAT4& p = a.colour;
		JSWrapper::SetObjectValue(obj, "r", p.x);
		JSWrapper::SetObjectValue(obj, "g", p.y);
		JSWrapper::SetObjectValue(obj, "b", p.z);
		JSWrapper::SetObjectValue(obj, "a", p.w);

		wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSSetSpotAngle(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if(wrapper.Check("N") == true)
		{
			self->set_spot_angle(wrapper.GetValue<float>(0, 0.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSSpotAngle(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		wrapper.ReturnValue<float>(self->attributes().spot_angle);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSSetConstantAttenuation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("N") == true)
		{
			self->set_constant_attenuation(wrapper.GetValue<float>(0, 0.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSConstantAttenuation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		wrapper.ReturnValue<float>(self->attributes().constant_attenuation);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSSetLinearAttenuation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("N") == true)
		{
			self->set_linear_attenuation(wrapper.GetValue<float>(0, 0.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSLinearAttenuation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		wrapper.ReturnValue<float>(self->attributes().linear_attenuation);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSSetQuadraticAttenuation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("N") == true)
		{
			self->set_quadratic_attenuation(wrapper.GetValue<float>(0, 0.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSQuadraticAttenuation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		wrapper.ReturnValue<float>(self->attributes().quad_attenuation);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSSetActivated(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("B") == true)
		{
			self->set_activated(wrapper.GetValue<bool>(0, true));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSActivated(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		wrapper.ReturnValue<bool>(self->attributes().activated);
	}
}