#include "../d3d11/d3d11_light.h"
#include "../d3d11/d3d11_lighting.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	D3D11Light::D3D11Light(D3D11Light::LightTypes type)
	{
		Default();
		attributes_.type = type;
	}

	//-------------------------------------------------------------------------------------------
	D3D11Light::D3D11Light(JS_ARGS args)
	{
		Default();
		JSWrapper wrapper(args);
		wrapper.Check("N");

		attributes_.type = wrapper.GetValue<int>(0, 0);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::Default()
	{
		attributes_.translation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		attributes_.direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
		attributes_.colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		attributes_.constant_attenuation = 1.0f;
		attributes_.linear_attenuation = 1.0f;
		attributes_.quad_attenuation = 1.0f;
		attributes_.spot_angle = XM_PI / 4;
		attributes_.type = 0;
		attributes_.activated = true;

		D3D11Lighting::Instance()->Add(this);
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
	void D3D11Light::set_direction(const float& x, const float& y, const float& z)
	{
		XMFLOAT4& d = attributes_.direction;
		d.x = x;
		d.y = y;
		d.z = z;
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
		D3D11Lighting::Instance()->Remove(this);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSEnumerate()
	{
		JSStateWrapper* wrapper = JSStateWrapper::Instance();
		v8::Handle <v8::Object> obj = JSWrapper::CreateObject();

		wrapper->RegisterToObject(obj, "Point", JSWrapper::CastValue<int>(0));
		wrapper->RegisterToObject(obj, "Directional", JSWrapper::CastValue<int>(1));
		wrapper->RegisterToObject(obj, "Spot", JSWrapper::CastValue<int>(2));

		wrapper->RegisterGlobal("LightType", obj);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::RegisterJS(JS_CONSTRUCTABLE obj)
	{
		D3D11Light::JSEnumerate();
		JSFunctionRegister funcs[] = {
			{ "setTranslation", JSSetTranslation },
			{ "translateBy", JSTranslateBy },
			{ "translation", JSTranslation },
			{ "setDirection", JSSetDirection },
			{ "direction", JSDirection },
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
	void D3D11Light::JSSetDirection(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		if (wrapper.Check("NNN") == true)
		{
			self->set_direction(
				wrapper.GetValue<float>(0, 0.0f),
				wrapper.GetValue<float>(1, 0.0f),
				wrapper.GetValue<float>(2, 0.0f)
				);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Light::JSDirection(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Light* self = wrapper.GetPointer<D3D11Light>(args.This());

		const Attributes& a = self->attributes();

		v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

		const XMFLOAT4& p = a.direction;
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