#include "../../d3d11/elements/d3d11_render_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"
#include "../../d3d11/d3d11_render_target.h"
#include "../../d3d11/d3d11_render_queue.h"
#include "../../d3d11/d3d11_effect.h"

#include "../../content/content_manager.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
	D3D11RenderElement::D3D11RenderElement() :
		translation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		rotation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		scale_(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
		offset_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		size_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		world_matrix_(XMMatrixIdentity()),
		spawned_(false),
		alpha_(1.0f),
		blend_(1.0f, 1.0f, 1.0f),
		effect_(nullptr),
		technique_("Default")
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::Spawn(const std::string& target)
  {
		if (spawned_ == false)
		{
			D3D11RenderDevice::Instance()->GetTarget(target)->queue()->Add(this);
			spawned_ = true;
		}
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::Destroy()
  {
    spawned_ = false;
  }

  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::TranslateBy(const float& x, const float& y, const float& z)
  {
    translation_ += XMVectorSet(x, y, z, 0.0f);
  }

  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::RotateBy(const float& x, const float& y, const float& z)
  {
    rotation_ += XMVectorSet(x, y, z, 0.0f);
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::ApplyEffect(const unsigned int& pass)
	{
		if (effect_ != nullptr)
		{
			effect_->Apply(technique_, pass);
		}
	}

	//-------------------------------------------------------------------------------------------
	unsigned int D3D11RenderElement::NumPasses()
	{
		if (effect_ != nullptr)
		{
			return effect_->NumPasses(technique_);
		}

		return 0;
	}

  //-------------------------------------------------------------------------------------------
  const XMVECTOR& D3D11RenderElement::translation() const
  {
    return translation_;
  }

  //-------------------------------------------------------------------------------------------
  const XMVECTOR& D3D11RenderElement::rotation() const
  {
    return rotation_;
  }

  //-------------------------------------------------------------------------------------------
  const XMVECTOR& D3D11RenderElement::scale() const
  {
    return scale_;
  }

  //-------------------------------------------------------------------------------------------
  const XMVECTOR& D3D11RenderElement::offset() const
  {
    return offset_;
  }

  //-------------------------------------------------------------------------------------------
  const XMVECTOR& D3D11RenderElement::size() const
  {
    return size_;
  }

  //-------------------------------------------------------------------------------------------
  const XMMATRIX& D3D11RenderElement::world_matrix()
  {
    world_matrix_ =
    XMMatrixScalingFromVector(scale_) * 
    XMMatrixTranslationFromVector(offset_) * 
    XMMatrixRotationRollPitchYawFromVector(rotation_) * 
    XMMatrixTranslationFromVector(translation_);

    return world_matrix_;
  }

  //-------------------------------------------------------------------------------------------
  const bool& D3D11RenderElement::spawned() const
  {
    return spawned_;
  }

	//-------------------------------------------------------------------------------------------
	const float& D3D11RenderElement::alpha() const
	{
		return alpha_;
	}

	//-------------------------------------------------------------------------------------------
	const XMFLOAT3& D3D11RenderElement::blend() const
	{
		return blend_;
	}

  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_translation(const float& x, const float& y, const float& z)
  {
    translation_ = XMVectorSet(x, y, z, 1.0f);
  }

  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_rotation(const float& x, const float& y, const float& z)
  {
    rotation_ = XMVectorSet(x, y, z, 1.0f);
  }

  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_scale(const float& x, const float& y, const float& z)
  {
    scale_ = XMVectorSet(x, y, z, 1.0f);
  }

  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_offset(const float& x, const float& y, const float& z)
  {
    offset_ = XMVectorSet(x, y, z, 1.0f);
  }

  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_size(const float& x, const float& y, const float& z)
  {
    size_ = XMVectorSet(x, y, z, 1.0f);
  }

  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_alpha(const float& a)
  {
    alpha_ = a;
  }
	
  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_blend(const float& r, const float& g, const float& b)
  {
    blend_.x = r;
    blend_.y = g;
    blend_.z = b;
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_effect(const std::string& path)
	{
		effect_ = ContentManager::Instance()->Get<D3D11Effect>(path);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_technique(const std::string& technique)
	{
		technique_ = technique;
	}

  //-------------------------------------------------------------------------------------------
  D3D11RenderElement::~D3D11RenderElement()
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::Register(JS_CONSTRUCTABLE obj)
  {
    JSFunctionRegister funcs[] = {
      { "setTranslation", JSSetTranslation },
      { "translateBy", JSTranslateBy },
      { "translation", JSTranslation },
      { "setRotation", JSSetRotation },
      { "rotateBy", JSRotateBy },
      { "rotation", JSRotation },
      { "setScale", JSSetScale },
      { "scale", JSScale },
      { "setOffset", JSSetOffset },
      { "offset", JSOffset },
      { "setSize", JSSetSize },
      { "size", JSSize },
      { "setAlpha", JSSetAlpha },
      { "alpha", JSAlpha },
      { "setBlend", JSSetBlend },
      { "blend", JSBlend },
			{ "setEffect", JSSetEffect },
			{ "setTechnique", JSSetTechnique },
      { "spawn", JSSpawn },
      { "destroy", JSDestroy }
    };

    JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSetTranslation(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("NNN") == true)
    {
      self->set_translation(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSTranslateBy(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("NNN") == true)
    {
      self->TranslateBy(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSTranslation(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    const XMVECTOR& v = self->translation();
    float x = XMVectorGetX(v);
    float y = XMVectorGetY(v);
    float z = XMVectorGetZ(v);

    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

    JSWrapper::SetObjectValue(obj, "x", x);
    JSWrapper::SetObjectValue(obj, "y", y);
    JSWrapper::SetObjectValue(obj, "z", z);

    wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSetRotation(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("NNN") == true)
    {
      self->set_rotation(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSRotateBy(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("NNN") == true)
    {
      self->RotateBy(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSRotation(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    const XMVECTOR& v = self->rotation();
    float x = XMVectorGetX(v);
    float y = XMVectorGetY(v);
    float z = XMVectorGetZ(v);

    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

    JSWrapper::SetObjectValue(obj, "x", x);
    JSWrapper::SetObjectValue(obj, "y", y);
    JSWrapper::SetObjectValue(obj, "z", z);

    wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSetScale(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("NNN") == true)
    {
      self->set_scale(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSScale(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    const XMVECTOR& v = self->scale();
    float x = XMVectorGetX(v);
    float y = XMVectorGetY(v);
    float z = XMVectorGetZ(v);

    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

    JSWrapper::SetObjectValue(obj, "x", x);
    JSWrapper::SetObjectValue(obj, "y", y);
    JSWrapper::SetObjectValue(obj, "z", z);

    wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSetOffset(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("NNN") == true)
    {
      self->set_offset(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSOffset(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    const XMVECTOR& v = self->offset();
    float x = XMVectorGetX(v);
    float y = XMVectorGetY(v);
    float z = XMVectorGetZ(v);

    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

    JSWrapper::SetObjectValue(obj, "x", x);
    JSWrapper::SetObjectValue(obj, "y", y);
    JSWrapper::SetObjectValue(obj, "z", z);

    wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSetSize(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("NNN") == true)
    {
      self->set_size(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSize(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    const XMVECTOR& v = self->size();
    float x = XMVectorGetX(v);
    float y = XMVectorGetY(v);
    float z = XMVectorGetZ(v);

    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

    JSWrapper::SetObjectValue(obj, "x", x);
    JSWrapper::SetObjectValue(obj, "y", y);
    JSWrapper::SetObjectValue(obj, "z", z);

    wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSAlpha(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    wrapper.ReturnValue<double>(self->alpha());
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSetAlpha(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("N"))
    {
      self->set_alpha(wrapper.GetValue<float>(0, 1.0));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSetBlend(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("NNN") == true)
    {
      self->set_blend(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSBlend(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    const XMFLOAT3& v = self->blend();

    v8::Handle<v8::Object> obj = JSWrapper::CreateObject();

    JSWrapper::SetObjectValue(obj, "r", v.x);
    JSWrapper::SetObjectValue(obj, "g", v.y);
    JSWrapper::SetObjectValue(obj, "b", v.z);

    wrapper.ReturnValue<v8::Handle<v8::Object>>(obj);
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSSetEffect(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		if (wrapper.Check("S"))
		{
			self->set_effect(wrapper.GetValue<std::string>(0, "undefined"));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSSetTechnique(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		if (wrapper.Check("S"))
		{
			self->set_technique(wrapper.GetValue<std::string>(0, "undefined"));
		}
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSpawn(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("S") == true)
    {
      self->Spawn(wrapper.GetValue<std::string>(0, "undefined"));
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSDestroy(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());
    self->Destroy();
  }
}