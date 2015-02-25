#include "../d3d11/d3d11_depth_state.h"

using namespace v8;
namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	D3D11DepthState::D3D11DepthState() :
		valid_(false)
	{

	}

	//-------------------------------------------------------------------------------------------
	void D3D11DepthState::Create(const D3D11_DEPTH_STENCIL_DESC& desc)
	{
		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		ID3D11Device* device = render_device->device();

		HRESULT result = device->CreateDepthStencilState(&desc, &depth_state_);
		SNUFF_XASSERT(result == S_OK, render_device->HRToString(result, "CreateDepthStencilState"), "D3D11DepthState::Create");
	
		valid_ = true;
	}

	//-------------------------------------------------------------------------------------------
	void D3D11DepthState::CreateFromJson(const std::string& str)
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
			SNUFF_LOG_ERROR("Invalid JSON for depth state");
			return;
		}
		if (json->IsObject() == false)
		{
			SNUFF_LOG_ERROR("Input JSON is not of an object type for a depth state");
			return;
		}

		Local<Object> obj = json->ToObject();

		CreateFromJson(obj);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11DepthState::CreateFromJson(const Local<Object>& obj)
	{
		if (obj.IsEmpty())
		{
			CreateFromJson(std::string("{}"));
			return;
		}
		Local<Array> properties = obj->GetPropertyNames();

		D3D11_DEPTH_STENCIL_DESC desc;

		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;

		desc.StencilEnable = true;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;

		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		std::string field;
		std::string value;

		for (unsigned int i = 0; i < properties->Length(); ++i)
		{
			field = *String::Utf8Value(properties->Get(i)->ToString());
			value = *String::Utf8Value(obj->Get(properties->Get(i)->ToString()));

			if (field == "DepthEnable")
			{
				desc.DepthEnable = value == "true";
			}
			else if (field == "DepthWriteMask")
			{
				desc.DepthWriteMask = StringToWriteMask(value);
			}
			else if (field == "DepthFunc")
			{
				desc.DepthFunc = StringToComparison(value);
			}
			else if (field == "StencilEnable")
			{
				desc.StencilEnable = value == "true";
			}
			else if (field == "FrontFaceFail")
			{
				desc.FrontFace.StencilFailOp = StringToOp(value);
			}
			else if (field == "FrontFaceDepthFail")
			{
				desc.FrontFace.StencilDepthFailOp = StringToOp(value);
			}
			else if (field == "FrontFacePass")
			{
				desc.FrontFace.StencilPassOp = StringToOp(value);
			}
			else if (field == "FrontFaceFunc")
			{
				desc.FrontFace.StencilFunc = StringToComparison(value);
			}
			else if (field == "BackFaceFail")
			{
				desc.BackFace.StencilFailOp = StringToOp(value);
			}
			else if (field == "BackFaceDepthFail")
			{
				desc.BackFace.StencilDepthFailOp = StringToOp(value);
			}
			else if (field == "BackFacePass")
			{
				desc.BackFace.StencilPassOp = StringToOp(value);
			}
			else if (field == "BackFaceFunc")
			{
				desc.BackFace.StencilFunc = StringToComparison(value);
			}
			else
			{
				SNUFF_LOG_ERROR("Unknown field '" + field + "' for a depth state");
			}
		}

		Create(desc);
	}

	//-------------------------------------------------------------------------------------------
	void D3D11DepthState::Set()
	{
		if (valid_ == false)
		{
			SNUFF_LOG_WARNING("Attempted to set an invalid depth state");
			return;
		}
		D3D11RenderDevice::Instance()->context()->OMSetDepthStencilState(depth_state_, 1);
	}

	//-------------------------------------------------------------------------------------------
	D3D11DepthState::~D3D11DepthState()
	{
		if (valid_ == false)
		{
			return;
		}

		SNUFF_SAFE_RELEASE(depth_state_, "D3D11DepthState::~D3D11DepthState::depth_state_");
	}

	//-------------------------------------------------------------------------------------------
	D3D11_STENCIL_OP D3D11DepthState::StringToOp(const std::string& str)
	{
		if (str == "Keep")
		{
			return D3D11_STENCIL_OP_KEEP;
		}
		else if (str == "Zero")
		{
			return D3D11_STENCIL_OP_ZERO;
		}
		else if (str == "Replace")
		{
			return D3D11_STENCIL_OP_REPLACE;
		}
		else if (str == "IncrSat")
		{
			return D3D11_STENCIL_OP_INCR_SAT;
		}
		else if (str == "DecrSat")
		{
			return D3D11_STENCIL_OP_DECR_SAT;
		}
		else if (str == "Invert")
		{
			return D3D11_STENCIL_OP_INVERT;
		}
		else if (str == "Incr")
		{
			return D3D11_STENCIL_OP_INCR;
		}
		else if (str == "Decr")
		{
			return D3D11_STENCIL_OP_DECR;
		}
		else
		{
			SNUFF_LOG_WARNING("Unknown depth operation '" + str + "', defaulting to 'Keep'");
			return D3D11_STENCIL_OP_KEEP;
		}
	}

	//-------------------------------------------------------------------------------------------
	D3D11_DEPTH_WRITE_MASK D3D11DepthState::StringToWriteMask(const std::string& str)
	{
		if (str == "All")
		{
			return D3D11_DEPTH_WRITE_MASK_ALL;
		}
		else if (str == "Zero")
		{
			return D3D11_DEPTH_WRITE_MASK_ZERO;
		}
		else
		{
			SNUFF_LOG_WARNING("Unknown depth write mask '" + str + "', defaulting to 'All'");
			return D3D11_DEPTH_WRITE_MASK_ALL;
		}
	}

	//-------------------------------------------------------------------------------------------
	D3D11_COMPARISON_FUNC D3D11DepthState::StringToComparison(const std::string& str)
	{
		if (str == "Never")
		{
			return D3D11_COMPARISON_NEVER;
		}
		else if (str == "Less")
		{
			return D3D11_COMPARISON_LESS;
		}
		else if (str == "Equal")
		{
			return D3D11_COMPARISON_EQUAL;
		}
		else if (str == "LessEqual")
		{
			return D3D11_COMPARISON_LESS_EQUAL;
		}
		else if (str == "Greater")
		{
			return D3D11_COMPARISON_GREATER;
		}
		else if (str == "NotEqual")
		{
			return D3D11_COMPARISON_NOT_EQUAL;
		}
		else if (str == "GreaterEqual")
		{
			return D3D11_COMPARISON_GREATER_EQUAL;
		}
		else if (str == "Always")
		{
			return D3D11_COMPARISON_ALWAYS;
		}
		else
		{
			SNUFF_LOG_WARNING("Unknown depth comparison function '" + str + "', defaulting to 'Always'");
			return D3D11_COMPARISON_ALWAYS;
		}
	}
}