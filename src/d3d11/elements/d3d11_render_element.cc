#include "../../d3d11/elements/d3d11_render_element.h"
#include "../../d3d11/d3d11_vertex_buffer.h"
#include "../../d3d11/d3d11_render_target.h"
#include "../../d3d11/d3d11_render_queue.h"
#include "../../d3d11/d3d11_effect.h"
#include "../../d3d11/d3d11_material.h"
#include "../../d3d11/d3d11_render_settings.h"
#include "../../d3d11/d3d11_camera.h"
#include "../../d3d11/d3d11_uniforms.h"
#include "../../d3d11/d3d11_scroll_area.h"

#include "../../content/content_manager.h"
#include "../../animation/animation_base.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11RenderElement::MaterialGroup::MaterialGroup() :
    material(nullptr),
    override_diffuse(nullptr),
    override_normal(nullptr),
    override_specular(nullptr),
    override_light(nullptr),
    override_effect(nullptr)
  {

  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::MaterialGroup::Apply()
  {
    if (override_diffuse != nullptr && override_diffuse->is_valid() == false)
    {
      override_diffuse = nullptr;
    }

    if (override_normal != nullptr && override_normal->is_valid() == false)
    {
      override_diffuse = nullptr;
    }

    if (override_specular != nullptr && override_specular->is_valid() == false)
    {
      override_diffuse = nullptr;
    }

    if (override_light != nullptr && override_light->is_valid() == false)
    {
      override_diffuse = nullptr;
    }

    if (override_effect != nullptr && override_effect->is_valid() == false)
    {
      override_effect = nullptr;
    }

    material->Apply(override_diffuse, override_normal, override_specular, override_light);
  }

  //-------------------------------------------------------------------------------------------
	D3D11RenderElement::D3D11RenderElement() :
		translation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		rotation_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		scale_(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
		offset_(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		size_(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
		world_matrix_(XMMatrixIdentity()),
		spawned_(false),
    billboarding_(false),
		technique_("Default"),
		layer_type_(LayerType::kWorld),
		blend_(1.0f, 1.0f, 1.0f),
		alpha_(1.0f),
		animation_coordinates_(0.0f, 0.0f, 1.0f, 1.0f),
		animation_(nullptr),
		alpha_changed_(false),
		blend_changed_(false),
		target_(nullptr),
    parent_(nullptr),
    scroll_area_(nullptr)
  {
    material_group_.material = D3D11RenderDevice::Instance()->default_material();
		uniforms_ = AllocatedMemory::Instance().Construct<D3D11Uniforms>();
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
		billboarding_(false),
		technique_("Default"),
		layer_type_(LayerType::kWorld),
		blend_(1.0f, 1.0f, 1.0f),
		alpha_(1.0f),
		animation_coordinates_(0.0f, 0.0f, 1.0f, 1.0f),
		animation_(nullptr),
		alpha_changed_(false),
		blend_changed_(false),
		target_(nullptr),
    scroll_area_(nullptr)
	{
    material_group_.material = D3D11RenderDevice::Instance()->default_material();
		uniforms_ = AllocatedMemory::Instance().Construct<D3D11Uniforms>();

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
			D3D11RenderTarget* t = D3D11RenderDevice::Instance()->GetTarget(target);
			if (t == nullptr)
			{
				return;
			}
			t->queue()->Add(this);
      target_ = t;
			spawned_ = true;
		}
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::Spawn()
  {
    if (spawned_ == false)
    {
      spawned_ = true;
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::Destroy()
  {
    spawned_ = false;
    target_ = nullptr;
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
      XMMATRIX billboard = XMMatrixInverse(&XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), D3D11RenderDevice::Instance()->camera()->view());
      XMFLOAT4X4 matrix;
      XMStoreFloat4x4(&matrix, billboard);
      matrix._14 = matrix._24 = matrix._34 = matrix._41 = matrix._42 = matrix._43 = 0;
      matrix._44 = 1;

      rotation *= XMLoadFloat4x4(&matrix);
    }

    XMFLOAT4X4 matrix;
    XMStoreFloat4x4(&matrix, trans);
		matrix._42 *= invert_y == false ? -1 : 1;

    trans = XMLoadFloat4x4(&matrix);

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
      XMStoreFloat4x4(&matrix, parent_trans);
      matrix._42 *= invert_y == false ? -1 : 1;

      parent_trans = XMLoadFloat4x4(&matrix);

			XMMATRIX root_trans = XMMatrixTranslationFromVector(parent->translation());
      XMStoreFloat4x4(&matrix, root_trans);
      matrix._42 *= invert_y == false ? -1 : 1;

      root_trans = XMLoadFloat4x4(&matrix);

			*world *=
				s * rot * parent_trans *
				XMMatrixScalingFromVector(parent->scale()) *
				XMMatrixRotationRollPitchYawFromVector(parent->rotation()) *
				root_trans;
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::AddChild(D3D11RenderElement* child)
	{
		children_.push_back(child);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::RemoveChild(D3D11RenderElement* child)
	{
		for (unsigned int i = 0; i < children_.size(); ++i)
		{
			if (children_.at(i) == child)
			{
				children_.erase(children_.begin() + i);
				break;
			}
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::Propagate()
	{
		if (blend_changed_ == false && alpha_changed_ == false)
		{
			return;
		}

		for (D3D11RenderElement* it : children_)
		{
			if (alpha_changed_ == true)
			{
				it->set_alpha(alpha_);
			}

			if (blend_changed_ == true)
			{
				it->set_blend(blend_.x, blend_.y, blend_.z);
			}
		}

		blend_changed_ = false;
		alpha_changed_ = false;
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::SetZ(const float& z)
  {
    translation_ = XMVectorSetZ(translation_, z);
  }

  //-------------------------------------------------------------------------------------------
  BoundingBox D3D11RenderElement::Bounds()
  {
    const XMMATRIX& world = world_matrix();

    const BoundingBox& bounds = vertex_buffer()->bounds();
    BoundingBox out;
    bounds.Transform(out, world);

    return out;
  }

  //-------------------------------------------------------------------------------------------
  bool D3D11RenderElement::RayIntersection(const XMFLOAT3& origin, const XMFLOAT3& dir, float* distance)
  {
    BoundingBox bounds = Bounds();

    bool intersects = bounds.Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&dir), *distance);
    
    if (intersects == true)
    {
      return vertex_buffer()->Pick(origin, dir, distance, world_matrix());
    }

    return false;
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
  D3D11RenderElement::MaterialGroup& D3D11RenderElement::material_group()
	{
		return material_group_;
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
	D3D11Uniforms* D3D11RenderElement::uniforms()
	{
		return uniforms_.get();
	}

  //-------------------------------------------------------------------------------------------
  D3D11RenderTarget* D3D11RenderElement::target()
  {
    return target_;
  }

	//-------------------------------------------------------------------------------------------
	const bool& D3D11RenderElement::billboarding() const
	{
		return billboarding_;
	}

	//-------------------------------------------------------------------------------------------
	const XMFLOAT4& D3D11RenderElement::animation_coordinates() const
	{
		return animation_coordinates_;
	}

	//-------------------------------------------------------------------------------------------
  Animation* D3D11RenderElement::animation()
  {
    return animation_;
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
		material_group_.material = ContentManager::Instance()->Get<D3D11Material>(path);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_material(D3D11Material* mat)
	{
    material_group_.material = mat;
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

		if (parent_ != nullptr)
		{
			parent_->AddChild(this);
		}
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

		blend_changed_ = true;
		Propagate();
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_alpha(const float& a)
	{
		alpha_ = a;

		alpha_changed_ = true;
		Propagate();
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_animation_coordinates(SpriteAnimation::Frame* frame, D3D11Texture* texture)
	{
		if (frame == nullptr || texture == nullptr)
		{
			return;
		}

		float width = static_cast<float>(frame->w) / static_cast<float>(texture->width());
		float height = static_cast<float>(frame->h) / static_cast<float>(texture->height());

		float offset_x = static_cast<float>(frame->x) / static_cast<float>(texture->width());
		float offset_y = static_cast<float>(frame->y) / static_cast<float>(texture->height());

		animation_coordinates_.x = offset_x;
		animation_coordinates_.y = offset_y;
		animation_coordinates_.z = width;
		animation_coordinates_.w = height;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_animation(Animation* animation)
	{
		animation_ = animation;
		
		if (animation_ != nullptr)
		{
			animation_->set_parent(this);
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_override_diffuse(D3D11Texture* diffuse)
	{
		material_group_.override_diffuse = diffuse;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_override_normal(D3D11Texture* normal)
	{
    material_group_.override_normal = normal;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_override_specular(D3D11Texture* specular)
	{
    material_group_.override_specular = specular;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_override_light(D3D11Texture* light)
	{
    material_group_.override_light = light;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::set_override_effect(D3D11Effect* effect)
	{
    material_group_.override_effect = effect;
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::set_scroll_area(D3D11ScrollArea* area)
  {
    scroll_area_ = area;
  }
  
  //-------------------------------------------------------------------------------------------
  const std::vector<D3D11RenderElement*>& D3D11RenderElement::children() const
  {
    return children_;
  }

  //-------------------------------------------------------------------------------------------
  D3D11ScrollArea* D3D11RenderElement::scroll_area()
  {
    return scroll_area_;
  }

  //-------------------------------------------------------------------------------------------
  D3D11RenderElement::~D3D11RenderElement()
  {
		for (unsigned int i = 0; i < children_.size(); ++i)
		{
			children_.at(i)->set_parent(nullptr);
		}

		if (parent_ != nullptr)
		{
			parent_->RemoveChild(this);
		}

		if (animation_ != nullptr)
		{
			animation_->set_parent(nullptr);
		}

    if (target_ != nullptr)
    {
      target_->FindAndRemove(this);
    }

		if (scroll_area_ != nullptr)
		{
			scroll_area_->RemoveChild(this);
		}
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
			{ "setUniform", JSSetUniform },
			{ "setAnimation", JSSetAnimation },
			{ "setDiffuseMap", JSSetDiffuseMap },
			{ "setNormalMap", JSSetNormalMap },
			{ "setSpecularMap", JSSetSpecularMap },
			{ "setLightMap", JSSetLightMap },
      { "setEffect", JSSetEffect },
      { "destroy", JSDestroy },
      { "rayIntersection", JSRayIntersection },
      { "bounds", JSBounds }
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
			self->SetZ(wrapper.GetValue<float>(0, 0.0f));
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
    float x = XMVectorGetX(v) * -1.0f;
		float y = XMVectorGetY(v) * -1.0f;
		float z = XMVectorGetZ(v) * -1.0f;

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
	void D3D11RenderElement::JSSetUniform(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		if (wrapper.Check("NSN") == true)
		{
			float buffer[4] = {
				wrapper.GetValue<float>(2, 0.0f),
				wrapper.GetValue<float>(3, 0.0f),
				wrapper.GetValue<float>(4, 0.0f),
				wrapper.GetValue<float>(5, 0.0f),
			};

			self->uniforms()->SetUniform(static_cast<D3D11Uniforms::UniformTypes>(
				wrapper.GetValue<int>(0, 1)),
				wrapper.GetValue<std::string>(1, "undefined"),
				buffer);
		}
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSSpawn(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    wrapper.set_error_checks(false);
    if (wrapper.Check("S") == true)
    {
      self->Spawn(wrapper.GetValue<std::string>(0, "undefined"));
    }
    else
    {
      self->Spawn();
    }
  }

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSSetAnimation(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

		if (wrapper.Check("O") == true)
		{
			self->set_animation(wrapper.GetPointer<Animation>(0));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSSetDiffuseMap(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());
		D3D11Texture* tex = nullptr;
		wrapper.set_error_checks(false);

		if (wrapper.Check("S") == true)
		{
			tex = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(0, "undefined"));
		}
		
		self->set_override_diffuse(tex);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSSetNormalMap(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());
		D3D11Texture* tex = nullptr;
		wrapper.set_error_checks(false);

		if (wrapper.Check("S") == true)
		{
			tex = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(0, "undefined"));
		}

		self->set_override_normal(tex);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSSetSpecularMap(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());
		D3D11Texture* tex = nullptr;
		wrapper.set_error_checks(false);

		if (wrapper.Check("S") == true)
		{
			tex = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(0, "undefined"));
		}

		self->set_override_specular(tex);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSSetLightMap(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());
		D3D11Texture* tex = nullptr;
		wrapper.set_error_checks(false);

		if (wrapper.Check("S") == true)
		{
			tex = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(0, "undefined"));
		}

		self->set_override_light(tex);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11RenderElement::JSSetEffect(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());
		D3D11Effect* fx = nullptr;
		wrapper.set_error_checks(false);

		if (wrapper.Check("S") == true)
		{
			fx = ContentManager::Instance()->Get<D3D11Effect>(wrapper.GetValue<std::string>(0, "undefined"));
		}

		self->set_override_effect(fx);
	}

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSRayIntersection(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());

    if (wrapper.Check("NNNNNN") == true)
    {
      float distance;
      bool result = self->RayIntersection(
        XMFLOAT3(
        wrapper.GetValue<float>(0, 0.0f),
        wrapper.GetValue<float>(1, 0.0f),
        wrapper.GetValue<float>(2, 0.0f)),

        XMFLOAT3(
        wrapper.GetValue<float>(3, 0.0f),
        wrapper.GetValue<float>(4, 0.0f),
        wrapper.GetValue<float>(5, 0.0f)
        ), &distance);

      if (result == true)
      {
        wrapper.ReturnValue<float>(distance);
      }
      else
      {
        wrapper.ReturnValue<bool>(false);
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11RenderElement::JSBounds(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11RenderElement* self = wrapper.GetPointer<D3D11RenderElement>(args.This());
    
    BoundingBox bounds = self->Bounds();
    v8::Handle<v8::Object> center = wrapper.CreateObject();

    wrapper.SetObjectValue(center, "x", bounds.Center.x);
    wrapper.SetObjectValue(center, "y", bounds.Center.y);
    wrapper.SetObjectValue(center, "z", bounds.Center.z);

    v8::Handle<v8::Object> extents = wrapper.CreateObject();

    wrapper.SetObjectValue(extents, "x", bounds.Extents.x);
    wrapper.SetObjectValue(extents, "y", bounds.Extents.y);
    wrapper.SetObjectValue(extents, "z", bounds.Extents.z);

    v8::Handle<v8::Object> to_return = wrapper.CreateObject();
    wrapper.SetObjectValue(to_return, "center", center);
    wrapper.SetObjectValue(to_return, "extents", extents);

    wrapper.ReturnValue<v8::Handle<v8::Object>>(to_return);
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