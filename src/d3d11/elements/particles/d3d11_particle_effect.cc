#include "../../../d3d11/elements/particles/d3d11_particle_effect.h"
#include "../../../content/content_manager.h"
#include "../../../application/game.h"
#include "../../../platform/platform_text_file.h"

using namespace v8;

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	RangedValue::RangedValue(const float& a, const float& b) : 
		min_val(a), 
		max_val(b)
	{
	
	}

	//---------------------------------------------------------------------------------------------------------
	float RangedValue::Get()
	{
		return 0.0f;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleEffect::D3D11ParticleEffect() :
		Content(ContentTypes::kParticleEffect)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleEffect::Load(const std::string& path)
	{
		std::string full_path = Game::Instance()->path() + "/" + path;
		TextFile file;
		bool valid = file.Open(full_path);

		if (valid == false)
		{
			SNUFF_LOG_ERROR("Could not open particle effect file '" + path + "'");
			return;
		}

		CreateFromJson(file.Read(), path);
	}

	//---------------------------------------------------------------------------------------------------------
	void D3D11ParticleEffect::CreateFromJson(const std::string& str, const std::string& path)
	{
		JSStateWrapper* wrapper = JSStateWrapper::Instance();
		Isolate* isolate = wrapper->isolate();
		HandleScope scope(wrapper->isolate());

		TryCatch try_catch;
		Local<Value> json = JSON::Parse(String::NewFromUtf8(isolate, str.c_str()));

		if (json.IsEmpty())
		{
			std::string buffer;
			JSStateWrapper::Instance()->GetException(&try_catch, &buffer);

			SNUFF_LOG_ERROR(buffer);
			SNUFF_LOG_ERROR("Invalid JSON for particle effect '" + path + "'");
			return;
		}

		if (json->IsObject() == false)
		{
			SNUFF_LOG_ERROR("Input JSON is not of an object type for a particle effect '" + path + "'");
			return;
		}

		Local<Object> obj = json->ToObject();

		auto GetString = [obj, isolate, path](const std::string& key, const std::string& def, const bool& required, bool* found)
		{
			Handle<Value> val = obj->Get(String::NewFromUtf8(isolate, key.c_str()));

			if (val.IsEmpty() == true || val->IsUndefined() == true || val->IsString() == false)
			{
				if (required == true)
				{
					SNUFF_LOG_WARNING("Could not find key '" + key + "', or it is not of a string value.\nIn particle effect '" + path + "'. Defaulting to '" + def + "'");
				}

				*found = false;
				return def;
			}
			
			*found = true;
			return const std::string(*String::Utf8Value(val->ToString()));
		};

		auto GetNumber = [obj, isolate, path](const std::string& key, const float& def, const bool& required, bool* found)
		{
			Handle<Value> val = obj->Get(String::NewFromUtf8(isolate, key.c_str()));

			if (val.IsEmpty() == true || val->IsUndefined() == true || val->IsNumber() == false)
			{
				if (required == true)
				{
					SNUFF_LOG_WARNING("Could not find key '" + key + "', or it is not of a number value.\nIn particle effect '" + path + "'. Defaulting to '" + std::to_string(def) + "'");
				}

				*found = false;
				return def;
			}

			*found = true;
			return static_cast<float>(val->ToNumber()->Value());
		};

		bool found = false;
		definition_.max_particles = static_cast<int>(GetNumber("MaxParticles", 1000, true, &found));
		definition_.spawn_type = StringToSpawnType(GetString("SpawnType", "PerSecond", true, &found));
		definition_.particles_per_second = static_cast<int>(GetNumber("ParticlesPerSecond", 5, definition_.spawn_type == ParticleSpawnType::kPerSecond, &found));
		definition_.shape = StringToShape(GetString("Shape", "Sphere", true, &found));
		definition_.loop = GetString("Loop", "true", false, &found) == "true";

	}

	//---------------------------------------------------------------------------------------------------------
	ParticleSpawnType D3D11ParticleEffect::StringToSpawnType(const std::string& str)
	{
		if (str == "PerSecond")
		{
			return ParticleSpawnType::kPerSecond;
		}
		else if (str == "Instant")
		{
			return ParticleSpawnType::kInstant;
		}
		else if (str == "Overtime")
		{
			return ParticleSpawnType::kOvertime;
		}
		else
		{
			SNUFF_LOG_WARNING("Could not find spawn type '" + str + "' defaulting to 'PerSecond'");
		}

		return ParticleSpawnType::kPerSecond;
	}

	//---------------------------------------------------------------------------------------------------------
	ParticleShape D3D11ParticleEffect::StringToShape(const std::string& str)
	{
		if (str == "Sphere")
		{
			return ParticleShape::kSphere;
		}
		else if (str == "Cone")
		{
			return ParticleShape::kCone;
		}
		else if (str == "Linear")
		{
			return ParticleShape::kLinear;
		}
		else
		{
			SNUFF_LOG_WARNING("Could not find particle shape '" + str + "' defaulting to 'Sphere'");
		}

		return ParticleShape::kSphere;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleEffect::~D3D11ParticleEffect()
	{

	}
}