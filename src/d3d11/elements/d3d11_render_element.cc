#include "../../d3d11/elements/d3d11_render_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"
#include "../../d3d11/d3d11_render_target.h"
#include "../../d3d11/d3d11_render_queue.h"
#include "../../d3d11/d3d11_effect.h"
#include "../../d3d11/d3d11_material.h"
#include "../../d3d11/d3d11_render_settings.h"
#include "../../d3d11/d3d11_camera.h"

#include "../../content/content_manager.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
	D3D11RenderElement::D3D11RenderElement() :
		translation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		rotation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		scale_(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
		offset_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		size_(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
		world_matrix_(XMMatrixIdentity()),
		spawned_(false),
    material_(nullptr),
    billboarding_(false),
		technique_("Default"),
		layer_type_(LayerType::kWorld),
		blend_(1.0f, 1.0f, 1.0f),
		alpha_(1.0f)
  {

  }

	//-------------------------------------------------------------------------------------------
	D3D11RenderElement::D3D11RenderElement(JS_ARGS args) :
		translation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		rotation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		scale_(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
		offset_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		size_(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
		world_matrix_(XMMatrixIdentity()),
		spawned_(false),
		material_(nullptr),
		billboarding_(false),
		technique_("Default"),
		layer_type_(LayerType::kWorld),
		blend_(1.0f, 1.0f, 1.0f),
		alpha_(1.0f)
	{
		JSWrapper wrapper(args);
		wrapper.set_error_checks(false);

		if (wrapper.Check("O") == false)
		{
			set_parent(nullptr);
			return;
		}

		wrapper.set_error_checks(true);

		if (wrapper.Check("O"))
		{
			set_parent(wrapper.GetPointer<D3D11RenderElement>(0));
		}
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
	void D3D11RenderElement::CalculateWorldMatrix(XMMATRIX* world, const bool& invert_y)
	{
		XMMATRIX trans = XMMatrixTranslationFromVector(translation_);

		XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(rotation_);

    if (billboarding_ == true)
    {
      XMMATRIX billboard = XMMatrixTranspose(D3D11RenderDevice::Instance()->camera()->view());

      billboard._14 = billboard._24 = billboard._34 = billboard._41 = billboard._42 = billboard._43 = 0;
      billboard._44 = 1;

      rotation *= billboard;
    }

		trans._42 *= invert_y == false ? -1 : 1;

		*world =
			XMMatrixScalingFromVector(scale_ * size_) *
			XMMatrixTranslationFromVector(offset_ * scale_ * size_) *
			rotation *
			trans;

		if (parent_ != nullptr)
		{
			D3D11RenderElement* parent = parent_;
			XMVECTOR t = XMVectorSet(0, 0, 0, 0);
			XMMATRIX rot = XMMatrixIdentity();
			XMMATRIX s = XMMatrixIdentity();

			while (parent->parent() != nullptr)
			{
				t += parent->translation();
				
				rot *= XMMatrixRotationRollPitchYawFromVector(parent->rotation());
				
				s *= XMMatrixScalingFromVector(parent->scale());
				parent = parent->parent();
			}

			XMVectorSetW(t, 1.0f);

			XMMATRIX parent_trans = XMMatrixTranslationFromVector(t);
			parent_trans._42 *= invert_y == false ? -1 : 1;

			XMMATRIX root_trans = XMMatrixTranslationFromVector(parent->translation());
			root_trans._42 *= invert_y == false ? -1 : 1;

			*world *=
				s * rot * parent_trans *
				XMMatrixScalingFromVector(parent->scale()) *
				XMMatrixRotationRollPitchYawFromVector(parent->rotation()) *
				root_trans;
		}
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
		CalculateWorldMatrix(&world_matrix_, D3D11RenderSettings::Instance()->invert_y());
    return world_matrix_;
  }

  //-------------------------------------------------------------------------------------------
  const bool& D3D11RenderElement::spawned() const
  {
    return spawned_;
  }

	//-------------------------------------------------------------------------------------------
	D3D11Material* D3D11RenderElement::material()
	{
		return material_;
	}

	//-------------------------------------------------------------------------------------------
	const std::string& D3D11RenderElement::technique() const
	{
		return technique_;
	}

	//-------------------------------------------------------------------------------------------
	D3D11RenderElement* D3D11RenderElement::parent()
	{
		return parent_;
	}

	//-------------------------------------------------------------------------------------------
	const XMFLOAT3& D3D11RenderElement::blend() const
	{
		return blend_;
	}

	//-------------------------------------------------------------------------------------------
	const float& D3D11RenderElement::alpha() const
	{
		return alpha_;
	}

	//-------------------------------------------------------------------------------------------
	const D3D11RenderElement::LayerType& D3D11RenderElement::layer_type() const
	{
		return layer_type_;
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
    offset_ = XMVectorSet(-x, -y, -z, 1.0f);
  }

  //-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_size(const float& x, const float& y, const float& z)
  {
    size_ = XMVectorSet(x, y, z, 1.0f);
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_material(const std::string& path)
	{
		material_ = ContentManager::Instance()->Get<D3D11Material>(path);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_material(D3D11Material* effect)
	{
		material_ = effect;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_technique(const std::string& technique)
	{
		technique_ = technique;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_parent(D3D11RenderElement* parent)
	{
		parent_ = parent;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_layer_type(const D3D11RenderElement::LayerType& type)
	{
		layer_type_ = type;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_world_matrix(const XMMATRIX& matrix)
	{
		world_matrix_ = matrix;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_billboarding(const bool& billboarding)
	{
		billboarding_ = billboarding;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_blend(const float& r, const float& g, const float& b)
	{
		blend_.x = r;
		blend_.y = g;
		blend_.z = b;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_alpha(const float& a)
	{
		alpha_ = a;
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
			{ "setZ", JSSetZ },
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
			{ "setMaterial", JSSetMaterial },
			{ "setTechnique", JSSetTechnique },
      { "spawn", JSSpawn },
			{ "spawned", JSSpawned },
			{ "setBlend", JSSetBlend },
			{ "blend", JSBlend },
			{ "setAlpha", JSSetAlpha },
			{ "alpha", JSAlpha },
      { "destroy", JSDestroy }
    };

    JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSetTranslation(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		if (wrapper.Check("NN") == true)
		{
			const XMVECTOR& v = self->translation();
			self->set_translation(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, XMVectorGetZ(v)));
		}
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSTranslateBy(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		if (wrapper.Check("NN") == true)
		{
			self->TranslateBy(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, 0.0f));
		}
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSSetZ(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		if (wrapper.Check("N") == true)
		{
			const XMVECTOR& v = self->translation();
			self->set_translation(XMVectorGetX(v), XMVectorGetY(v), wrapper.GetValue<float>(0, 0.0f));
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

		if (wrapper.Check("NN") == true)
		{
			const XMVECTOR& v = self->scale();
			self->set_scale(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, XMVectorGetZ(v)));
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

		if (wrapper.Check("NN") == true)
		{
			const XMVECTOR& v = self->offset();
			self->set_offset(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, XMVectorGetZ(v)));
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

    if (wrapper.Check("NN") == true)
    {
			const XMVECTOR& v = self->size();
      self->set_size(wrapper.GetValue<float>(0, 0.0f), wrapper.GetValue<float>(1, 0.0f), wrapper.GetValue<float>(2, XMVectorGetZ(v)));
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
	void D3D11RenderElement::JSSetMaterial(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		if (wrapper.Check("S"))
		{
			self->set_material(wrapper.GetValue<std::string>(0, "undefined"));
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
	void D3D11RenderElement::JSSetParent(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		wrapper.set_error_checks(false);

		if (wrapper.Check("O") == false)
		{
			self->set_parent(nullptr);
		}

		wrapper.set_error_checks(true);

		if (wrapper.Check("O"))
		{
			self->set_parent(wrapper.GetPointer<D3D11RenderElement>(0));
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
	void D3D11RenderElement::JSSetAlpha(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		if (wrapper.Check("N") == true)
		{
			self->set_alpha(wrapper.GetValue<float>(0, 1.0f));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSAlpha(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		wrapper.ReturnValue<float>(self->alpha());
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
	void D3D11RenderElement::JSSpawned(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		wrapper.ReturnValue<bool>(self->spawned());
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSDestroy(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());
    self->Destroy();
  }
}