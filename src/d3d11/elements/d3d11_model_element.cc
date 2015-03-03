#include "../../d3d11/elements/d3d11_model_element.h"
#include "../../fbx/fbx_model.h"
#include "../../d3d11/d3d11_vertex_buffer.h"
#include "../../content/content_manager.h"

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
	D3D11Model::D3D11Model() :
		model_(nullptr)
  {
    
  }

  //-------------------------------------------------------------------------------------------
	D3D11Model::D3D11Model(JS_ARGS args) :
		D3D11RenderElement(args),
		model_(nullptr)
  {
		JSWrapper wrapper(args);

		if (wrapper.Check("S"))
		{
			SetModel(wrapper.GetValue<std::string>(0, "undefined"));
		}
		else
		{
			path_ = "undefined";
		}
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
		if (model_->is_valid() == false)
		{
			model_ = nullptr;
		}
    return model_ == nullptr ? nullptr : model_->vertex_buffer();
  }

	//-------------------------------------------------------------------------------------------
	const std::string& D3D11Model::path() const
	{
		return path_;
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
}