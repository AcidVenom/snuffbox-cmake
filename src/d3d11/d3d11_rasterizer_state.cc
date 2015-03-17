#include "../d3d11/d3d11_rasterizer_state.h"
#include "../d3d11/d3d11_render_settings.h"
#include "../d3d11/d3d11_render_target.h"
#include "../d3d11/d3d11_viewport.h"

using namespace v8;

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11RasterizerState::D3D11RasterizerState()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RasterizerState::Create(const D3D11_RASTERIZER_DESC& desc)
	{
		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
		HRESULT hr = render_device->device()->CreateRasterizerState(&desc, &rasterizer_);

		SNUFF_XASSERT(hr == S_OK, render_device->HRToString(hr, "CreateRasterizerState"), "D3D11RasterizerState::Create");

		SetScissorRect(0, 0, 1.0f, 1.0f);
		valid_ = true;
		desc_ = desc;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RasterizerState::CreateFromJson(const std::string& src)
	{
		JSStateWrapper* wrapper = JSStateWrapper::Instance();
		HandleScope scope(wrapper->isolate());

		TryCatch try_catch;
		Local<Value> json = JSON::Parse(String::NewFromUtf8(wrapper->isolate(), src.c_str()));

		if (json.IsEmpty())
		{
			std::string buffer;
			JSStateWrapper::Instance()->GetException(&try_catch, &buffer);

			SNUFF_LOG_ERROR(buffer);
			SNUFF_LOG_ERROR("Invalid JSON for rasterizer state");
			return;
		}
		if (json->IsObject() == false)
		{
			SNUFF_LOG_ERROR("Input JSON is not of an object type for a rasterizer state");
			return;
		}

		CreateFromJson(json->ToObject());

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RasterizerState::CreateFromJson(const v8::Handle<v8::Object>& obj)
	{
		if (obj.IsEmpty())
		{
			CreateFromJson(std::string("{}"));
			return;
		}

		Local<Array> properties = obj->GetPropertyNames();

		std::string field;
		std::string value;

		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		desc.ScissorEnable = true;

		for (unsigned int i = 0; i < properties->Length(); ++i)
		{
			field = *String::Utf8Value(properties->Get(i)->ToString());
			value = *String::Utf8Value(obj->Get(properties->Get(i)->ToString()));

			if (field == "CullMode")
			{
				StringToCullMode(value);
			}
			else if (field == "FillMode")
			{
				StringToFillMode(value);
			}
			else if (field == "ScissorEnable")
			{
				desc.ScissorEnable = value == "true";
			}
			else
			{
				SNUFF_LOG_ERROR("Unknown field for rasterizer state '" + field + "'");
				continue;
			}
		}

		Create(desc);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RasterizerState::SetScissorRect(const float& x, const float& y, const float& w, const float& h)
	{
		scissor_.x = x;
		scissor_.y = y;
		scissor_.z = w;
		scissor_.w = h;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11RasterizerState::Set()
	{
		if (valid_ == false)
		{
			SNUFF_LOG_WARNING("Attempted to set an invalid rasterizer state");
			return;
		}

		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();

		if (EqualsTo(render_device->current_rasterizer_state()) == false)
		{
			render_device->set_current_rasterizer_state(this);
			ID3D11DeviceContext* ctx = render_device->context();
			ctx->RSSetState(rasterizer_);
			D3D11_RECT r;
			
			D3D11Viewport* vp = render_device->viewport_render_target();

			r.left = static_cast<int>(vp->x());
			r.top = static_cast<int>(vp->y());

			r.right = static_cast<int>(scissor_.z * vp->width());
			r.bottom = static_cast<int>(scissor_.w * vp->height());

			ctx->RSSetScissorRects(1, &r);
		}
	}

	//---------------------------------------------------------------------------------------------------------
	bool D3D11RasterizerState::EqualsTo(D3D11RasterizerState* other)
	{
		if (other == nullptr)
		{
			return false;
		}

		const D3D11_RASTERIZER_DESC& other_desc = other->description();

		if (other_desc.AntialiasedLineEnable != desc_.AntialiasedLineEnable ||
			other_desc.CullMode != desc_.CullMode ||
			other_desc.DepthBias != desc_.DepthBias ||
			other_desc.DepthBiasClamp != desc_.DepthBiasClamp ||
			other_desc.FillMode != desc_.FillMode ||
			other_desc.FrontCounterClockwise != desc_.FrontCounterClockwise ||
			other_desc.MultisampleEnable != desc_.MultisampleEnable ||
			other_desc.ScissorEnable != desc_.ScissorEnable ||
			other_desc.SlopeScaledDepthBias != desc_.SlopeScaledDepthBias
			)
		{
			return false;
		}

		const XMFLOAT4& other_scissor = other->scissor();

		if (other_scissor.x != scissor_.x ||
			other_scissor.y != scissor_.y ||
			other_scissor.z != scissor_.z ||
			other_scissor.w != scissor_.w
			)
		{
			return false;
		}

		return true;
	}

	//---------------------------------------------------------------------------------------------------------
	const D3D11_RASTERIZER_DESC& D3D11RasterizerState::description() const
	{
		return desc_;
	}

	//---------------------------------------------------------------------------------------------------------
	const XMFLOAT4& D3D11RasterizerState::scissor() const
	{
		return scissor_;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11RasterizerState::~D3D11RasterizerState()
	{
		if (valid_ == false)
		{
			return;
		}

		SNUFF_SAFE_RELEASE(rasterizer_, "D3D11RasterizerState::~D3D11RasterizerState::rasterizer_");
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11_CULL_MODE D3D11RasterizerState::StringToCullMode(const std::string& str)
	{
		if (str == "Front")
		{
			return D3D11_CULL_MODE::D3D11_CULL_FRONT;
		}
		else if (str == "Back")
		{
			return D3D11_CULL_MODE::D3D11_CULL_BACK;
		}
		else if (str == "None")
		{
			return D3D11_CULL_MODE::D3D11_CULL_NONE;
		}
		else
		{
			SNUFF_LOG_ERROR("Unknown cull mode '" + str + "', defaulting to 'Front'");
			return D3D11_CULL_MODE::D3D11_CULL_FRONT;
		}
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11_FILL_MODE D3D11RasterizerState::StringToFillMode(const std::string& str)
	{
		if (str == "Solid")
		{
			return D3D11_FILL_MODE::D3D11_FILL_SOLID;
		}
		else if (str == "Wire")
		{
			return D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		}
		else
		{
			SNUFF_LOG_ERROR("Unknown fill mode '" + str + "', defaulting to 'Solid'");
			return D3D11_FILL_MODE::D3D11_FILL_SOLID;
		}
	}
}