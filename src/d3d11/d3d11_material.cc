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
		specular_(nullptr),
		light_map_(nullptr)
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

		auto GetTexture = [this, isolate](const Local<Object>& v, const std::string& field, D3D11Texture** out)
		{
			Local<Value> value = v->Get(String::NewFromUtf8(isolate, field.c_str()));

			if (value.IsEmpty() == false && value->IsUndefined() == false)
			{
				*out = ContentManager::Instance()->Get<D3D11Texture>(*String::Utf8Value(value->ToString()));
			}
		};

		GetTexture(obj, "diffuse_map", &diffuse_);
		GetTexture(obj, "normal_map", &normal_);
		GetTexture(obj, "specular_map", &specular_);
		GetTexture(obj, "light_map", &light_map_);

		Local<Value> cube = obj->Get(String::NewFromUtf8(isolate, "cube_map"));

		if (cube.IsEmpty() == false && cube->IsObject() == true)
		{
			Local<Object> cube_map = cube->ToObject();
			GetTexture(cube_map, "left", &cube_.left);
			GetTexture(cube_map, "right", &cube_.right);
			GetTexture(cube_map, "top", &cube_.top);
			GetTexture(cube_map, "bottom", &cube_.bottom);
			GetTexture(cube_map, "front", &cube_.front);
			GetTexture(cube_map, "back", &cube_.back);

			cube_map_ = AllocatedMemory::Instance().Construct<D3D11Texture>();
			cube_map_->CreateCubeMap(cube_);
		}

		attributes_ = {
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			1.0f,
			1.0f,
			0.0f,
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

		val = obj->Get(String::NewFromUtf8(isolate, "specular_power"));

		if (val.IsEmpty() == false && val->IsUndefined() == false)
		{
			attributes_.specular_power = static_cast<float>(val->ToNumber()->Value());
		}

		val = obj->Get(String::NewFromUtf8(isolate, "specular_intensity"));

		if (val.IsEmpty() == false && val->IsUndefined() == false)
		{
			attributes_.specular_intensity = static_cast<float>(val->ToNumber()->Value());
		}

		val = obj->Get(String::NewFromUtf8(isolate, "reflectivity"));

		if (val.IsEmpty() == false && val->IsUndefined() == false)
		{
			attributes_.reflectivity = static_cast<float>(val->ToNumber()->Value());
		}

		val = obj->Get(String::NewFromUtf8(isolate, "normal_scale"));

		if (val.IsEmpty() == false && val->IsUndefined() == false)
		{
			attributes_.normal_scale = static_cast<float>(val->ToNumber()->Value());
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Material::Apply()
	{
		D3D11RenderDevice* render_device = D3D11RenderDevice::Instance();
    D3D11Texture* default_tex = render_device->default_texture();
    D3D11Texture* default_normal = render_device->default_normal();
		std::vector<ID3D11ShaderResourceView*> resources;

    if (diffuse_ != nullptr && (diffuse_->is_valid() == false || diffuse_->texture() == nullptr))
    {
      diffuse_ = nullptr;
    }

    if (normal_ != nullptr && (normal_->is_valid() == false || normal_->texture() == nullptr))
    {
      normal_ = nullptr;
    }

    if (specular_ != nullptr && (specular_->is_valid() == false || specular_->texture() == nullptr))
    {
      specular_ = nullptr;
    }

    if (light_map_ != nullptr && (light_map_->is_valid() == false || light_map_->texture() == nullptr))
    {
      light_map_ = nullptr;
    }

		resources.push_back(diffuse_ == nullptr ? default_tex->texture() : diffuse_->texture());
    resources.push_back(normal_ == nullptr ? default_normal->texture() : normal_->texture());
    resources.push_back(specular_ == nullptr ? default_tex->texture() : specular_->texture());
    resources.push_back(light_map_ == nullptr ? default_tex->texture() : light_map_->texture());

		D3D11Texture::SetMultipleTextures(1, static_cast<int>(resources.size()), resources);

		if (cube_map_ != nullptr)
		{
			cube_map_->Set(0);
		}
    else
    {
      render_device->default_cube_map()->Set(0);
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
	D3D11Material::Attributes& D3D11Material::attributes()
	{
		return attributes_;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Material::set_effect(D3D11Effect* effect)
	{
		effect_ = effect;
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11Material::set_textures(D3D11Texture* diffuse, D3D11Texture* normal)
	{
		diffuse_ = diffuse;
		normal_ = normal;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11Material::~D3D11Material()
	{

	}
}