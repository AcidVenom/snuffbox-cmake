#include "../d3d11/d3d11_material.h"
#include "../d3d11/d3d11_effect.h"
#include "../d3d11/d3d11_texture.h"

#include "../content/content_manager.h"
#include "../application/game.h"
#include "../platform/platform_text_file.h"

using namespace v8;

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	D3D11Material::D3D11Material() :
		Content(ContentTypes::kMaterial),
		effect_(nullptr),
		diffuse_(nullptr),
		normal_(nullptr),
		bump_(nullptr)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Material::Load(const std::string& path)
	{
		std::string full_path = Game::Instance()->path() + "/" + path;
		TextFile file;
		bool valid = file.Open(full_path);

		if (valid == false)
		{
			SNUFF_LOG_ERROR("Could not open material file '" + path + "'");
			return;
		}

		JSStateWrapper* wrapper = JSStateWrapper::Instance();
		Isolate* isolate = wrapper->isolate();
		HandleScope scope(wrapper->isolate());

		TryCatch try_catch;
		Local<Value> json = JSON::Parse(String::NewFromUtf8(isolate, file.Read().c_str()));

		if (json.IsEmpty())
		{
			std::string buffer;
			JSStateWrapper::Instance()->GetException(&try_catch, &buffer);

			SNUFF_LOG_ERROR(buffer);
			SNUFF_LOG_ERROR("Invalid JSON for material '" + path + "'");
			return;
		}

		if (json->IsObject() == false)
		{
			SNUFF_LOG_ERROR("Input JSON is not of an object type for a material '" + path + "'");
			return;
		}

		Local<Object> obj = json->ToObject();

		Local<Value> val = obj->Get(String::NewFromUtf8(isolate, "effect"));

		if (val.IsEmpty() == false && val->IsUndefined() == false)
		{
			effect_ = ContentManager::Instance()->Get<D3D11Effect>(*String::Utf8Value(val->ToString()));
		}
		else
		{
			SNUFF_LOG_WARNING("Even though not required, the material '" + path + "' doesn't have an 'effect' field");
		}

		val = obj->Get(String::NewFromUtf8(isolate, "diffuse_map"));

		if (val.IsEmpty() == false && val->IsUndefined() == false)
		{
			diffuse_ = ContentManager::Instance()->Get<D3D11Texture>(*String::Utf8Value(val->ToString()));
		}

		val = obj->Get(String::NewFromUtf8(isolate, "normal_map"));

		if (val.IsEmpty() == false && val->IsUndefined() == false)
		{
			normal_ = ContentManager::Instance()->Get<D3D11Texture>(*String::Utf8Value(val->ToString()));
		}

		val = obj->Get(String::NewFromUtf8(isolate, "bump_map"));

		if (val.IsEmpty() == false && val->IsUndefined() == false)
		{
			bump_ = ContentManager::Instance()->Get<D3D11Texture>(*String::Utf8Value(val->ToString()));
		}

		attributes_ = {
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			1.0f
		};

		auto GetFloat4 = [this, obj, isolate, path](const std::string& field, XMFLOAT4& store)
		{
			Local<Value> val = obj->Get(String::NewFromUtf8(isolate, field.c_str()));

			if (val.IsEmpty() == false && val->IsUndefined() == false)
			{
				bool result = GetFloat4FromArray(val->ToObject(), &store);
				if (result == false)
				{
					store = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
					SNUFF_LOG_WARNING("'" + field + "' of material '" + path + "' is invalid, notation: [r,g,b,a]");
				}
			}
		};

		GetFloat4("emissive", attributes_.emissive);
		GetFloat4("diffuse", attributes_.diffuse);
		GetFloat4("ambient", attributes_.ambient);
		GetFloat4("specular", attributes_.specular);

		val = obj->Get(String::NewFromUtf8(isolate, "specular_intensity"));

		if (val.IsEmpty() == false && val->IsUndefined() == false)
		{
			attributes_.specular_intensity = static_cast<float>(val->ToNumber()->Value());
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Material::Apply()
	{
		if (diffuse_ != nullptr && diffuse_->is_valid())
		{
			diffuse_->Set(0);
		}
		else
		{
			diffuse_ = nullptr;
		}

		if (normal_ != nullptr && normal_->is_valid())
		{
			normal_->Set(1);
		}
		else
		{
			normal_ = nullptr;
		}
		
		if (bump_ != nullptr && bump_->is_valid())
		{
			bump_->Set(2);
		}
		else
		{
			bump_ = nullptr;
		}
	}

	//---------------------------------------------------------------------------------------------------------
	bool D3D11Material::GetFloat4FromArray(const v8::Local<v8::Object>& arr, XMFLOAT4* store)
	{
		if (arr->IsArray() == false || arr->IsUndefined())
		{
			SNUFF_LOG_ERROR("Float4 value was not of an array type for a JSON");
			return false;
		}

		*store = { 0.0f, 0.0f, 0.0f, 0.0f };
		float v = 0.0f;
		for (unsigned int i = 0; i < arr->GetPropertyNames()->Length(); ++i)
		{
			v = static_cast<float>(arr->Get(i)->ToNumber()->Value());

			switch (i)
			{
			case 0:
				store->x = v;
				break;
			case 1:
				store->y = v;
				break;
			case 2:
				store->z = v;
				break;
			case 3:
				store->w = v;
				break;
			}
		}

		return true;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Effect* D3D11Material::effect()
	{
		return effect_;
	}

	//---------------------------------------------------------------------------------------------------------
	const D3D11Material::Attributes& D3D11Material::attributes() const
	{
		return attributes_;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Material::set_effect(D3D11Effect* effect)
	{
		effect_ = effect;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Material::~D3D11Material()
	{

	}
}