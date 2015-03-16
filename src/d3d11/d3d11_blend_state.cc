#include "../d3d11/d3d11_blend_state.h"

using namespace v8;

namespace snuffbox
{
  //-------------------------------------------------------------------------------------------
  D3D11BlendState::D3D11BlendState() :
    valid_(false),
    blend_state_(false)
  {
    ZeroMemory(&desc_, sizeof(D3D11_BLEND_DESC));
  }

  //-------------------------------------------------------------------------------------------
  void D3D11BlendState::Create(const D3D11_BLEND_DESC& desc)
  {
    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    ID3D11Device* device = render_device->device();

    HRESULT result = device->CreateBlendState(&desc, &blend_state_);

    SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateBlendState"), "D3D11BlendState::Create");

    desc_ = desc;
    valid_ = true;
  }

	//-------------------------------------------------------------------------------------------
	void D3D11BlendState::CreateFromJson(const std::string& str)
	{
		JSStateWrapper* wrapper = JSStateWrapper::Instance();
		HandleScope scope(wrapper->isolate());

		TryCatch try_catch;
		Local<Value> json = JSON::Parse(String::NewFromUtf8(wrapper->isolate(), str.c_str()));

		if (json.IsEmpty())
		{
			std::string buffer;
			JSStateWrapper::Instance()->GetException(&try_catch, &buffer);

			SNUFF_LOG_ERROR(buffer);
			SNUFF_LOG_ERROR("Invalid JSON for blend state");
			return;
		}
		if (json->IsObject() == false)
		{
			SNUFF_LOG_ERROR("Input JSON is not of an object type for a blend state");
			return;
		}

		CreateFromJson(json->ToObject());
	}

  //-------------------------------------------------------------------------------------------
  void D3D11BlendState::CreateFromJson(const Local<Object>& obj)
  {
		if (obj.IsEmpty())
		{
			CreateFromJson(std::string("{}"));
			return;
		}
    Local<Array> properties = obj->GetPropertyNames();

    std::string field;
    std::string value;

    D3D11_BLEND_DESC desc;

    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    for (unsigned int i = 0; i < properties->Length(); ++i)
    {
      field = *String::Utf8Value(properties->Get(i)->ToString());
      value = *String::Utf8Value(obj->Get(properties->Get(i)->ToString()));

      if (field == "AlphaToCoverageEnable")
      {
        desc.AlphaToCoverageEnable = value == "true" ? true : false;
      }
      else if (field == "BlendEnable")
      {
        desc.RenderTarget[0].BlendEnable = value == "true" ? true : false;
      }
      else if (field == "SrcBlend")
      {
        desc.RenderTarget[0].SrcBlend = D3D11BlendState::StringToBlend(value);
      }
      else if (field == "DestBlend")
      {
        desc.RenderTarget[0].DestBlend = D3D11BlendState::StringToBlend(value);
      }
      else if (field == "BlendOp")
      {
        desc.RenderTarget[0].BlendOp = D3D11BlendState::StringToBlendOp(value);
      }
      else if (field == "SrcBlendAlpha")
      {
        desc.RenderTarget[0].SrcBlendAlpha = D3D11BlendState::StringToBlend(value);
      }
      else if (field == "DestBlendAlpha")
      {
        desc.RenderTarget[0].DestBlendAlpha = D3D11BlendState::StringToBlend(value);
      }
      else if (field == "BlendOpAlpha")
      {
        desc.RenderTarget[0].BlendOpAlpha = D3D11BlendState::StringToBlendOp(value);
      }
      else
      {
        SNUFF_LOG_ERROR("Unknown field for blend state '" + field + "'");
      }
    }

    Create(desc);
  }

  //-------------------------------------------------------------------------------------------
  void D3D11BlendState::Set()
  {
    if (valid_ == false)
    {
      SNUFF_LOG_WARNING("Attempted to set an invalid blend state, blending will fail");
      return;
    }

    D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();

    if (EqualsTo(render_device->current_blend_state()) == false)
    {
      render_device->set_current_blend_state(this);
      render_device->context()->OMSetBlendState(blend_state_, NULL, 0x000000FF);
    }
  }

  //-------------------------------------------------------------------------------------------
  bool D3D11BlendState::EqualsTo(D3D11BlendState* other)
  {
    if (other == nullptr)
    {
      return false;
    }

    const D3D11_BLEND_DESC& other_desc = other->description();

    if (other_desc.AlphaToCoverageEnable != desc_.AlphaToCoverageEnable ||
      other_desc.IndependentBlendEnable != desc_.IndependentBlendEnable ||
      other_desc.RenderTarget[0].BlendEnable != desc_.RenderTarget[0].BlendEnable ||
      other_desc.RenderTarget[0].BlendOp != desc_.RenderTarget[0].BlendOp ||
      other_desc.RenderTarget[0].SrcBlend != desc_.RenderTarget[0].SrcBlend ||
      other_desc.RenderTarget[0].DestBlend != desc_.RenderTarget[0].DestBlend ||
      other_desc.RenderTarget[0].BlendOpAlpha != desc_.RenderTarget[0].BlendOpAlpha ||
      other_desc.RenderTarget[0].SrcBlendAlpha != desc_.RenderTarget[0].SrcBlendAlpha ||
      other_desc.RenderTarget[0].DestBlendAlpha != desc_.RenderTarget[0].DestBlendAlpha ||
      other_desc.RenderTarget[0].RenderTargetWriteMask != desc_.RenderTarget[0].RenderTargetWriteMask)
    {
      return false;
    }

    return true;
  }

  //-------------------------------------------------------------------------------------------
  const D3D11_BLEND_DESC& D3D11BlendState::description() const
  {
    return desc_;
  }

  //-------------------------------------------------------------------------------------------
  D3D11BlendState::~D3D11BlendState()
  {
    if (valid_ == false)
    {
      return;
    }

    SNUFF_SAFE_RELEASE(blend_state_, "D3D11BlendState::~D3D11BlendState::blend_state_");
  }

  //-------------------------------------------------------------------------------------------
  D3D11_BLEND D3D11BlendState::StringToBlend(const std::string& str)
  {
    if (str == "Zero")
    {
      return D3D11_BLEND_ZERO;
    }
    else if (str == "One")
    {
      return D3D11_BLEND_ONE;
    }
    else if (str == "SrcColour")
    {
      return D3D11_BLEND_SRC_COLOR;
    }
    else if (str == "InvSrcColour")
    {
      return D3D11_BLEND_INV_SRC_COLOR;
    }
    else if (str == "SrcAlpha")
    {
      return D3D11_BLEND_SRC_ALPHA;
    }
    else if (str == "InvSrcAlpha")
    {
      return D3D11_BLEND_INV_SRC_ALPHA;
    }
    else if (str == "DestAlpha")
    {
      return D3D11_BLEND_DEST_ALPHA;
    }
    else if (str == "DestAlpha")
    {
      return D3D11_BLEND_DEST_ALPHA;
    }
    else if (str == "InvDestAlpha")
    {
      return D3D11_BLEND_INV_DEST_ALPHA;
    }
    else if (str == "DestColour")
    {
      return D3D11_BLEND_DEST_COLOR;
    }
    else if (str == "InvDestColour")
    {
      return D3D11_BLEND_INV_DEST_COLOR;
    }
    else if (str == "BlendFactor")
    {
      return D3D11_BLEND_BLEND_FACTOR;
    }
    else if (str == "SrcAlphaSat")
    {
      return D3D11_BLEND_SRC_ALPHA_SAT;
    }
    else if (str == "InvBlendFactor")
    {
      return D3D11_BLEND_INV_BLEND_FACTOR;
    }
    else if (str == "Src1Colour")
    {
      return D3D11_BLEND_SRC1_COLOR;
    }
    else if (str == "InvSrc1Colour")
    {
      return D3D11_BLEND_INV_SRC1_COLOR;
    }
    else if (str == "Src1Alpha")
    {
      return D3D11_BLEND_SRC1_ALPHA;
    }
    else if (str == "InvSrcAlpha")
    {
      return D3D11_BLEND_INV_SRC1_ALPHA;
    }
    else
    {
      SNUFF_LOG_WARNING("Unknown blend mode '" + str + "', defaulting to 'Zero'");
      return D3D11_BLEND_ZERO;
    }
  }

  //-------------------------------------------------------------------------------------------
  D3D11_BLEND_OP D3D11BlendState::StringToBlendOp(const std::string& str)
  {
    if (str == "Add")
    {
      return D3D11_BLEND_OP_ADD;
    }
    else if (str == "Subtract")
    {
      return D3D11_BLEND_OP_SUBTRACT;
    }
    else if (str == "RevSubtract")
    {
      return D3D11_BLEND_OP_REV_SUBTRACT;
    }
    else if (str == "Min")
    {
      return D3D11_BLEND_OP_MIN;
    }
    else if (str == "Max")
    {
      return D3D11_BLEND_OP_MAX;
    }
    else
    {
      SNUFF_LOG_WARNING("Unknown blend operation '" + str + "', defaulting to 'Add'");
      return D3D11_BLEND_OP_ADD;
    }
  }
}