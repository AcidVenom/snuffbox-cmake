#include "../../../d3d11/elements/particles/d3d11_particle_effect.h"
#include "../../../content/content_manager.h"
#include "../../../application/game.h"
#include "../../../platform/platform_text_file.h"

using namespace v8;

#undef min
#undef max

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------------
	RangedValue::RangedValue()
	{
	
	}

	//---------------------------------------------------------------------------------------------------------
	RangedValue::RangedValue(const float& a, const float& b) :
		min_val(a),
		max_val(b)
	{
		Randomise();
	}

	//---------------------------------------------------------------------------------------------------------
	RangedValue::RangedValue(const v8::Handle<v8::Value>& val)
	{
		if (val->IsNumber() == true)
		{
			min_val = max_val = static_cast<float>(val->ToNumber()->Value());
			
			Randomise();
			
			return;
		}
		else if (val->IsArray() == true)
		{
			v8::Handle<v8::Object> arr = val->ToObject();
			v8::Handle<v8::Value> num;

			num = arr->Get(0);
			min_val = num->IsNumber() == true ? static_cast<float>(num->ToNumber()->Value()) : 0.0f;

			num = arr->Get(1);
			max_val = num->IsNumber() == true ? static_cast<float>(num->ToNumber()->Value()) : 0.0f;

			Randomise();
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void RangedValue::Randomise()
	{
		value = min_val + (max_val - min_val) * (static_cast<float>(rand()) / 0x7FFF);
	}

	//---------------------------------------------------------------------------------------------------------
	RangedVec3::RangedVec3()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	RangedVec3::RangedVec3(const XMFLOAT3& a, const XMFLOAT3& b) :
		x(a.x, b.x),
		y(a.y, b.y),
		z(a.z, b.z)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	RangedVec3::RangedVec3(const v8::Handle<v8::Value>& vec)
	{
		if (vec->IsObject() == true)
		{
			v8::Handle<v8::Object> inner = vec->ToObject();
			v8::Handle<v8::Value> is_number = inner->Get(0);
			if (is_number->IsNumber() == true)
			{
				x = RangedValue(is_number->ToNumber());
				is_number = inner->Get(1);
				y = RangedValue(is_number->ToNumber());
				is_number = inner->Get(2);
				z = RangedValue(is_number->ToNumber());
			}
			else if (is_number->IsObject() == true)
			{
				v8::Handle<v8::Object> first = is_number->ToObject();
				v8::Handle<v8::Object> second = inner->Get(1)->ToObject();

				x = RangedValue(static_cast<float>(first->Get(0)->ToNumber()->Value()), static_cast<float>(second->Get(0)->ToNumber()->Value()));
				y = RangedValue(static_cast<float>(first->Get(1)->ToNumber()->Value()), static_cast<float>(second->Get(1)->ToNumber()->Value()));
				z = RangedValue(static_cast<float>(first->Get(2)->ToNumber()->Value()), static_cast<float>(second->Get(2)->ToNumber()->Value()));
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void RangedVec3::Randomise()
	{
		x.Randomise();
		y.Randomise();
		z.Randomise();
	}

	//---------------------------------------------------------------------------------------------------------
	XMFLOAT3 RangedVec3::value() const
	{
		return XMFLOAT3(x.value, y.value, z.value);
	}

	//---------------------------------------------------------------------------------------------------------
	RangedVec4::RangedVec4()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	RangedVec4::RangedVec4(const XMFLOAT4& a, const XMFLOAT4& b) :
		x(a.x, b.x),
		y(a.y, b.y),
		z(a.z, b.z),
		w(a.w, b.w)
	{

	}

	//---------------------------------------------------------------------------------------------------------
	RangedVec4::RangedVec4(const v8::Handle<v8::Value>& vec)
	{
		if (vec->IsObject() == true)
		{
			v8::Handle<v8::Object> inner = vec->ToObject();
			v8::Handle<v8::Value> is_number = inner->Get(0);
			if (is_number->IsNumber() == true)
			{
				x = RangedValue(is_number->ToNumber());
				is_number = inner->Get(1);
				y = RangedValue(is_number->ToNumber());
				is_number = inner->Get(2);
				z = RangedValue(is_number->ToNumber());
				is_number = inner->Get(3);
				w = RangedValue(is_number->ToNumber());
			}
			else if (is_number->IsObject() == true)
			{
				v8::Handle<v8::Object> first = is_number->ToObject();
				v8::Handle<v8::Object> second = inner->Get(1)->ToObject();

				x = RangedValue(static_cast<float>(first->Get(0)->ToNumber()->Value()), static_cast<float>(second->Get(0)->ToNumber()->Value()));
				y = RangedValue(static_cast<float>(first->Get(1)->ToNumber()->Value()), static_cast<float>(second->Get(1)->ToNumber()->Value()));
				z = RangedValue(static_cast<float>(first->Get(2)->ToNumber()->Value()), static_cast<float>(second->Get(2)->ToNumber()->Value()));
				w = RangedValue(static_cast<float>(first->Get(3)->ToNumber()->Value()), static_cast<float>(second->Get(3)->ToNumber()->Value()));
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------
	void RangedVec4::Randomise()
	{
		x.Randomise();
		y.Randomise();
		z.Randomise();
		w.Randomise();
	}

	//---------------------------------------------------------------------------------------------------------
	XMFLOAT4 RangedVec4::value() const
	{
		return XMFLOAT4(x.value, y.value, z.value, w.value);
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleEffect::ControlPoint::ControlPoint(const v8::Handle<v8::Value>& val, const std::string& path, const int& idx)
	{
		if (val->IsObject() == false)
		{
			SNUFF_LOG_ERROR("Attempted to create an invalid control point in particle effect '" + path + "' at index " + std::to_string(idx));
			return;
		}

		v8::Handle<v8::Object> obj = val->ToObject();

		v8::Isolate* isolate = JSStateWrapper::Instance()->isolate();

		auto GetNumber = [obj, isolate](const std::string& key, const float& def)
		{
			v8::Handle<v8::Value> num = obj->Get(String::NewFromUtf8(isolate, key.c_str()));

			if (num->IsNumber() == false)
			{
				return def;
			}

			return static_cast<float>(num->ToNumber()->Value());
		};

		ratio = GetNumber("Ratio", 0.0f);
		velocity = RangedVec3(obj->Get(String::NewFromUtf8(isolate, "Velocity")));
		colour = RangedVec4(obj->Get(String::NewFromUtf8(isolate, "Colour")));
		size = RangedValue(obj->Get(String::NewFromUtf8(isolate, "Size")));
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleEffect::ControlPoint::Result D3D11ParticleEffect::ControlPoint::Interpolate(const D3D11ParticleEffect::ControlPoint& other, const float& current)
	{
		float d = other.ratio - ratio;
		float r = (current - ratio) / d;

		r = std::max(0.0f, std::min(r, 1.0f));

		Result ret_val;
		ret_val.size = D3D11ParticleEffect::Lerp(size.value, other.size.value, r);
		ret_val.velocity = D3D11ParticleEffect::Lerp3(velocity.value(), other.velocity.value(), r);
		ret_val.colour = D3D11ParticleEffect::Lerp4(colour.value(), other.colour.value(), r);

		return ret_val;
	}

	//---------------------------------------------------------------------------------------------------------
	bool D3D11ParticleEffect::ControlPoint::Sorter::operator()(D3D11ParticleEffect::ControlPoint& a, D3D11ParticleEffect::ControlPoint& b)
	{
		return a.ratio < b.ratio;
	}

	//---------------------------------------------------------------------------------------------------------
	D3D11ParticleEffect::D3D11ParticleEffect() :
		Content(ContentTypes::kParticleEffect),
		valid_(false)
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

		control_points_.clear();
		valid_ = false;

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
				return std::string(def.c_str());
			}
			
			*found = true;
			return std::string(*String::Utf8Value(val->ToString()));
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
		definition_.life_time = GetNumber("LifeTime", 1.0f, true, &found);
		definition_.max_particles = static_cast<int>(GetNumber("MaxParticles", 1000, true, &found));
		definition_.spawn_type = StringToSpawnType(GetString("SpawnType", "PerSecond", true, &found));
		definition_.particles_per_second = static_cast<int>(GetNumber("ParticlesPerSecond", 5, definition_.spawn_type == ParticleSpawnType::kPerSecond, &found));
		definition_.loop = GetString("Loop", "true", false, &found) == "true";
		definition_.loop_length = static_cast<int>(GetNumber("LoopLength", 0, found, &found));
		definition_.start_position = RangedVec3(obj->Get(String::NewFromUtf8(isolate, "StartPosition")));

		v8::Handle<v8::Value> cp = obj->Get(String::NewFromUtf8(isolate, "ControlPoints"));
		if (cp->IsArray() == true)
		{
			v8::Handle<v8::Object> arr = cp->ToObject();

			for (unsigned int i = 0; i < arr->GetPropertyNames()->Length(); ++i)
			{
				control_points_.push_back(ControlPoint(arr->Get(i), path, i));
			}
		}

		std::sort(control_points_.begin(), control_points_.end(), ControlPoint::Sorter());
		valid_ = true;
	}

	//---------------------------------------------------------------------------------------------------------
	ParticleDefinition& D3D11ParticleEffect::definition()
	{
		return definition_;
	}

	//---------------------------------------------------------------------------------------------------------
	const bool& D3D11ParticleEffect::valid() const
	{
		return valid_;
	}

	//---------------------------------------------------------------------------------------------------------
	std::vector<D3D11ParticleEffect::ControlPoint>& D3D11ParticleEffect::control_points()
	{
		return control_points_;
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
	D3D11ParticleEffect::~D3D11ParticleEffect()
	{

	}

	//---------------------------------------------------------------------------------------------------------
	float D3D11ParticleEffect::Lerp(const float& a, const float& b, const float& r)
	{
		float ratio = std::fminf(std::fmaxf(0.0f, r), 1.0f);
		return a * (1 - ratio) + ratio * b;
	}

	//---------------------------------------------------------------------------------------------------------
	XMFLOAT3 D3D11ParticleEffect::Lerp3(const XMFLOAT3& a, const XMFLOAT3& b, const float& r)
	{
		return XMFLOAT3(Lerp(a.x, b.x, r), Lerp(a.y, b.y, r), Lerp(a.z, b.z, r));
	}

	//---------------------------------------------------------------------------------------------------------
	XMFLOAT4 D3D11ParticleEffect::Lerp4(const XMFLOAT4& a, const XMFLOAT4& b, const float& r)
	{
		return XMFLOAT4(Lerp(a.x, b.x, r), Lerp(a.y, b.y, r), Lerp(a.z, b.z, r), Lerp(a.w, b.w, r));
	}
}