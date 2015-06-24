#include "../../d3d11/elements/d3d11_model_element.h"
#include "../../fbx/fbx_model.h"
#include "../../d3d11/d3d11_vertex_buffer.h"
#include "../../content/content_manager.h"
#include "../../d3d11/d3d11_effect.h"
#include "../../d3d11/d3d11_material.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
	D3D11Model::D3D11Model() :
		model_(nullptr)
  {
    material_groups_.resize(20);
  }

  //-------------------------------------------------------------------------------------------
	D3D11Model::D3D11Model(JS_ARGS args) :
		D3D11RenderElement(args),
		model_(nullptr)
  {
    material_groups_.resize(20);

		JSWrapper wrapper(args);

    wrapper.set_error_checks(false);

		if (wrapper.Check("S"))
		{
			SetModel(wrapper.GetValue<std::string>(0, "undefined"));
		}
		else
		{
			path_ = "undefined";
		}

    if (wrapper.Check("SO"))
    {
      set_parent(wrapper.GetPointer<D3D11RenderElement>(1));
    }
    else
    {
      if (wrapper.Check("O"))
      {
        set_parent(wrapper.GetPointer<D3D11RenderElement>(0));
      }
    }

    v8::Local<v8::Object> self = args.This();

    v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();
    self->Set(v8::String::NewFromUtf8(isolate, "setDiffuseMap"), v8::Function::New(isolate, D3D11Model::JSSetDiffuseMap));
    self->Set(v8::String::NewFromUtf8(isolate, "setNormalMap"), v8::Function::New(isolate, D3D11Model::JSSetNormalMap));
    self->Set(v8::String::NewFromUtf8(isolate, "setSpecularMap"), v8::Function::New(isolate, D3D11Model::JSSetSpecularMap));
    self->Set(v8::String::NewFromUtf8(isolate, "setLightMap"), v8::Function::New(isolate, D3D11Model::JSSetLightMap));
    self->Set(v8::String::NewFromUtf8(isolate, "setEffect"), v8::Function::New(isolate, D3D11Model::JSSetEffect));
    self->Set(v8::String::NewFromUtf8(isolate, "setMaterial"), v8::Function::New(isolate, D3D11Model::JSSetMaterial));
  }

	//-------------------------------------------------------------------------------------------
	void D3D11Model::SetModel(const std::string& path)
	{
		model_ = ContentManager::Instance()->Get<FBXModel>(path);

		if (model_ == nullptr)
		{
			path_ = path;
			SNUFF_LOG_WARNING("Attempted to set an invalid model '" + path + "', element will not be rendered");
		}
		else
		{
			path_ = "undefined";
		}
	}

  //-------------------------------------------------------------------------------------------
	D3D11VertexBuffer* D3D11Model::vertex_buffer()
  {
		if (model_ == nullptr || model_->is_valid() == false)
		{
			model_ = nullptr;
		}
    return model_ == nullptr ? nullptr : model_->vertex_buffer();
  }

  //-------------------------------------------------------------------------------------------
  std::vector<D3D11RenderElement::MaterialGroup>& D3D11Model::material_groups()
  {
    return material_groups_;
  }

	//-------------------------------------------------------------------------------------------
	const std::string& D3D11Model::path() const
	{
		return path_;
	}

  //-------------------------------------------------------------------------------------------
  FBXModel* D3D11Model::model()
  {
    return model_;
  }

  //-------------------------------------------------------------------------------------------
	D3D11Model::~D3D11Model()
  {

  }

  //-------------------------------------------------------------------------------------------
	void D3D11Model::RegisterJS(JS_CONSTRUCTABLE obj)
  {
    D3D11RenderElement::Register(obj);

		JSFunctionRegister funcs[] = {
			{ "setModel", JSSetModel },
			{ "model", JSModel }
		};

		JSFunctionRegister::Register(funcs, sizeof(funcs) / sizeof(JSFunctionRegister), obj);
  }

	//-------------------------------------------------------------------------------------------
	void D3D11Model::JSSetModel(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Model* self = wrapper.GetPointer<D3D11Model>(args.This());

		if (wrapper.Check("S"))
		{
			self->SetModel(wrapper.GetValue<std::string>(0, "undefined"));
		}
	}

	//-------------------------------------------------------------------------------------------
	void D3D11Model::JSModel(JS_ARGS args)
	{
		JSWrapper wrapper(args);
		D3D11Model* self = wrapper.GetPointer<D3D11Model>(args.This());

		wrapper.ReturnValue<std::string>(self->path());
	}

  //-------------------------------------------------------------------------------------------
  void D3D11Model::JSSetDiffuseMap(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Model* self = wrapper.GetPointer<D3D11Model>(args.This());

    wrapper.set_error_checks(false);

    if (wrapper.Check("S") == true)
    {
      self->material_groups().at(0).override_diffuse = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(0, "undefined"));
    }
    else
    {
      wrapper.set_error_checks(true);
      if (wrapper.Check("NS") == true)
      {
        self->material_groups().at(wrapper.GetValue<int>(0, 0)).override_diffuse = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(1, "undefined"));
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Model::JSSetNormalMap(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Model* self = wrapper.GetPointer<D3D11Model>(args.This());

    wrapper.set_error_checks(false);

    if (wrapper.Check("S") == true)
    {
      self->material_groups().at(0).override_normal = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(0, "undefined"));
    }
    else
    {
      wrapper.set_error_checks(true);
      if (wrapper.Check("NS") == true)
      {
        self->material_groups().at(wrapper.GetValue<int>(0, 0)).override_normal = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(1, "undefined"));
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Model::JSSetSpecularMap(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Model* self = wrapper.GetPointer<D3D11Model>(args.This());

    wrapper.set_error_checks(false);

    if (wrapper.Check("S") == true)
    {
      self->material_groups().at(0).override_specular = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(0, "undefined"));
    }
    else
    {
      wrapper.set_error_checks(true);
      if (wrapper.Check("NS") == true)
      {
        self->material_groups().at(wrapper.GetValue<int>(0, 0)).override_specular = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(1, "undefined"));
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Model::JSSetLightMap(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Model* self = wrapper.GetPointer<D3D11Model>(args.This());

    wrapper.set_error_checks(false);

    if (wrapper.Check("S") == true)
    {
      self->material_groups().at(0).override_light = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(0, "undefined"));
    }
    else 
    {
      wrapper.set_error_checks(true);
      if (wrapper.Check("NS") == true)
      {
        self->material_groups().at(wrapper.GetValue<int>(0, 0)).override_light = ContentManager::Instance()->Get<D3D11Texture>(wrapper.GetValue<std::string>(1, "undefined"));
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Model::JSSetEffect(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Model* self = wrapper.GetPointer<D3D11Model>(args.This());

    wrapper.set_error_checks(false);

    if (wrapper.Check("S") == true)
    {
      self->material_groups().at(0).override_effect = ContentManager::Instance()->Get<D3D11Effect>(wrapper.GetValue<std::string>(0, "undefined"));
    }
    else
    {
      wrapper.set_error_checks(true);
      if (wrapper.Check("NS") == true)
      {
        self->material_groups().at(wrapper.GetValue<int>(0, 0)).override_effect = ContentManager::Instance()->Get<D3D11Effect>(wrapper.GetValue<std::string>(1, "undefined"));
      }
    }
  }

  //-------------------------------------------------------------------------------------------
  void D3D11Model::JSSetMaterial(JS_ARGS args)
  {
    JSWrapper wrapper(args);
    D3D11Model* self = wrapper.GetPointer<D3D11Model>(args.This());

    wrapper.set_error_checks(false);

    if (wrapper.Check("S") == true)
    {
      self->material_groups().at(0).material = ContentManager::Instance()->Get<D3D11Material>(wrapper.GetValue<std::string>(0, "undefined"));
    }
    else
    {
      wrapper.set_error_checks(true);
      if (wrapper.Check("NS") == true)
      {
        self->material_groups().at(wrapper.GetValue<int>(0, 0)).material = ContentManager::Instance()->Get<D3D11Material>(wrapper.GetValue<std::string>(1, "undefined"));
      }
    }
  }
}